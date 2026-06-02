#ifndef FIXEDTHREADPOOL_HPP
#define FIXEDTHREADPOOL_HPP
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include "SyncQueue.hpp"
class FixedThreadPool
{
    public:
    using Task = std::function<void()>;
    private:
    std::list<std::shared_ptr<std::thread>> threads_;
    SyncQueue<Task> taskQueue_;
    std::atomic<bool> isRunning_;
    std::once_flag stopFlag_;
    void Start(int numThreads){
        isRunning_ = true;
        for(int i = 0; i < numThreads; ++i){
            threads_.emplace_back(std::make_shared<std::thread>(&FixedThreadPool::Worker,this));
        }
    }
    void Worker(){
        while(isRunning_){
            Task task;
            taskQueue_.Take(task);
            if(task && isRunning_){
                task();
            }
        }
    }
    void StopThreadGroup(){
        isRunning_ = false;
        taskQueue_.Stop();
        for(auto& thread : threads_){
            if(thread->joinable()){
                thread->join();
            }
        }
    }
    public:
    FixedThreadPool(int numThreads): taskQueue_(MAX_TASK_NUM), isRunning_(false){
        Start(numThreads);
    }
    ~FixedThreadPool(){
        Stop();
    }
    void Stop(){
        std::call_once(stopFlag_,[this](){StopThreadGroup();});
    }
    void AddTask(Task&&task){
        taskQueue_.Put(std::forward<Task>(task));
    }
    void AddTask(const Task& task){
        taskQueue_.Put(task);
    }
};
#endif