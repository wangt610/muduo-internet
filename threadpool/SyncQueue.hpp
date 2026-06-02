#ifndef SYNCQUEUE_HPP
#define SYNCQUEUE_HPP
#include <list>
#include <mutex>
#include <condition_variable>
#include <chrono>
const int MAX_TASK_NUM = 100;
template <typename T>
class SyncQueue {
private:
    std::list<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable m_notEmpty;// 生产者等待队列不满
    std::condition_variable m_notFull;// 消费者等待队列不空
    int maxSize_;
    bool m_needStop;
    bool isFull()const{
        return queue_.size() >= maxSize_;
    }
    bool isEmpty()const{
        return queue_.empty();
    }
    template<typename F>
    void Add(F&& task){
        std::unique_lock<std::mutex> lock(mutex_);
        m_notFull.wait(lock,[this](){return !isFull() || m_needStop;});
        if(m_needStop){
            return;
        }
        queue_.emplace_back(std::forward<F>(task));
        m_notEmpty.notify_one();
    }
    public:
    SyncQueue(int maxSize): maxSize_(maxSize), m_needStop(false) {}
    ~SyncQueue(){
        Stop();
    }
    void Put(const T &task){
        Add(task);
    }
    void Put(T &&task){
        Add(std::forward<T>(task));
    }
    void Take(std::list<T> &list){
        std::unique_lock<std::mutex> lock(mutex_);
        m_notEmpty.wait(lock,[this](){return !isEmpty() || m_needStop;});
        if(m_needStop){
            return;
        }
        list.splice(list.end(),queue_);
        m_notFull.notify_all();
    }
    void Take(T &task){
        std::unique_lock<std::mutex> lock(mutex_);
        m_notEmpty.wait(lock,[this](){return !isEmpty() || m_needStop;});
        if(m_needStop){
            return;
        }
        task = std::move(queue_.front());
        queue_.pop_front();
        m_notFull.notify_one();
    }
    void Stop(){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            m_needStop = true;
        }
        m_notEmpty.notify_all();
        m_notFull.notify_all();
    }
    bool Empty()const{
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    bool Full()const{
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size() >= maxSize_;
    }
    int Size()const{
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    int Count()const{
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

};
#endif // SYNCQUEUE_HPP
