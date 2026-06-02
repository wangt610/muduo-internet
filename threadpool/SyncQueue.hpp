#ifndef SYNCQUEUE_HPP
#define SYNCQUEUE_HPP
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
const int MAX_TASK_NUM = 100;
template <typename T>
class SyncQueue {
private:
    std::list<T> queue_;
    std::mutex mutex_;
    std::condition_variable m_notEmpty;// 生产者等待队列不满
    std::condition_variable m_notFull;// 消费者等待队列不空
    int maxSize_;
    bool m_needStop;
    bool isFull()const;
    bool isEmpty()const;
    template<F &&task>
    void Add(F &&task);
    public:
    SyncQueue(int maxSize);
    ~SyncQueue();
    void Put(const T &task);
    void Put(T &&task);
    void Take(std::list<T> &list);
    void Take(T &task);
    void Stop();
    bool Empty()const;
    bool Full()const;
    int Size()const;
    int Count()const;

};
#endif // SYNCQUEUE_HPP
