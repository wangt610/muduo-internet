#pragma once
#include <iostream>
#include <list>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <map>
#include <memory>
#include <mutex>

class CacheThread {
public:
    using task = std::function<void()>;

private:
    std::map<std::thread::id, std::shared_ptr<std::thread>> m_threads;
    SyncQueue<task> m_queue;
    std::once_flag m_onceFlag;
    std::atomic_bool m_running{ false };
    std::atomic_int m_idleThreads{ 0 };
    std::atomic_int m_currentThreads{ 0 };
    mutable std::mutex m_mutex;
    int m_maxThreads;
    int m_coreThreads;
    int m_queueWaitSec;
    int m_keepAliveSec;

    void AddThread() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_currentThreads.load() >= m_maxThreads) {
            return;
        }
        auto t = std::make_shared<std::thread>(&CacheThread::RunThread, this);
        m_threads.emplace(t->get_id(), t);
        m_currentThreads++;
        m_idleThreads++;
    }

    void Start(int num) {
        m_running = true;
        for (int i = 0; i < num; ++i) {
            AddThread();
        }
    }

    void RunThread() {
        const auto tid = std::this_thread::get_id();
        auto lastWork = std::chrono::steady_clock::now();

        while (m_running) {
            task t;
            const int ret = m_queue.Take(t);

            if (ret == 0) {
                lastWork = std::chrono::steady_clock::now();
                if (m_idleThreads > 0) {
                    m_idleThreads--;
                }
                t();
                m_idleThreads++;
                continue;
            }

            if (ret == 2) {
                break;
            }

            // Take 超时：队列里仍有任务但没人取 → 扩容
            if (m_queue.HasTask() && m_currentThreads < m_maxThreads) {
                AddThread();
                continue;
            }

            // 空闲超过 keepAlive，且当前线程数大于核心数 → 回收本线程
            const auto now = std::chrono::steady_clock::now();
            const bool idleTooLong =
                std::chrono::duration_cast<std::chrono::seconds>(now - lastWork).count()
                >= m_keepAliveSec;

            if (idleTooLong && m_currentThreads > m_coreThreads) {
                std::shared_ptr<std::thread> self;
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    auto it = m_threads.find(tid);
                    if (it != m_threads.end()) {
                        self = it->second;
                        m_threads.erase(it);
                    }
                }
                if (self) {
                    m_currentThreads--;
                    if (m_idleThreads > 0) {
                        m_idleThreads--;
                    }
                    self->detach();
                }
                return;
            }
        }
    }

    void StopGroup() {
        m_running = false;
        m_queue.Stop();
        std::map<std::thread::id, std::shared_ptr<std::thread>> threads;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            threads.swap(m_threads);
        }
        for (auto& kv : threads) {
            if (kv.second->joinable()) {
                kv.second->join();
            }
        }
        m_idleThreads = 0;
        m_currentThreads = 0;
    }

public:
    CacheThread(int maxThreads, int coreThreads, int queueWaitSec = 10, int keepAliveSec = 10)
        : m_maxThreads(maxThreads)
        , m_coreThreads(coreThreads)
        , m_queue(maxThreads, queueWaitSec)
        , m_queueWaitSec(queueWaitSec)
        , m_keepAliveSec(keepAliveSec) {
        Start(coreThreads);
    }

    ~CacheThread() {
        Stop();
    }

    void Stop() {
        std::call_once(m_onceFlag, [this]() {
            StopGroup();
        });
    }

    template<class F, class... Args>
    void AddTask(F&& func, Args&&... args) {
        auto bound = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        auto wrapped = std::make_shared<std::function<void()>>(std::move(bound));

        int ret = m_queue.put([wrapped]() { (*wrapped)(); });
        if (ret == 1 && m_currentThreads < m_maxThreads) {
            AddThread();
            ret = m_queue.put([wrapped]() { (*wrapped)(); });
        }
        if (ret != 0) {
            (*wrapped)();
            std::cout << "AddTask: queue blocked, ran on caller thread" << std::endl;
            return;
        }

        if (m_idleThreads == 0 && m_currentThreads < m_maxThreads) {
            AddThread();
        }
    }
};
