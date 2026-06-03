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

template<class T>
class SyncQueue {
private:
    std::condition_variable m_not_full;
    std::condition_variable m_not_empty;
    mutable std::mutex mutex;
    std::list<T> m_queue;
    size_t m_waitTime;
    std::atomic<int> m_maxsize;
    std::atomic<bool> m_needStop;

public:
    bool IsFull() const {
        return m_queue.size() >= static_cast<size_t>(m_maxsize.load());
    }

    bool IsEmpty() const {
        return m_queue.empty();
    }

    bool HasTask() const {
        std::lock_guard<std::mutex> lock(mutex);
        return !m_queue.empty();
    }

    template<typename F>
    int AddTask(F&& task) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!m_not_full.wait_for(lock, std::chrono::seconds(m_waitTime),
            [this] { return !IsFull() || m_needStop; })) {
            return 1;
        }
        if (m_needStop) {
            return 2;
        }
        m_queue.emplace_back(std::forward<F>(task));
        m_not_empty.notify_one();
        return 0;
    }

    SyncQueue(int maxsize, int waitTime)
        : m_maxsize(maxsize), m_waitTime(waitTime), m_needStop(false) {}

    int put(const T& task) {
        return AddTask(task);
    }

    int put(T&& task) {
        return AddTask(std::forward<T>(task));
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            m_needStop = true;
        }
        m_not_full.notify_all();
        m_not_empty.notify_all();
    }

    ~SyncQueue() {
        Stop();
    }

    int Take(T& task) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!m_not_empty.wait_for(lock, std::chrono::seconds(m_waitTime),
            [this] { return !IsEmpty() || m_needStop; })) {
            return 1;
        }
        if (m_needStop) {
            return 2;
        }
        task = std::move(m_queue.front());
        m_queue.pop_front();
        m_not_full.notify_one();
        return 0;
    }
};
