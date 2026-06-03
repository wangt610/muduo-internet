# ThreadPool 线程池模块

## 主要介绍

```
线程池 = 预先创建好的一组工作线程 + 线程安全的任务队列

优点：
1. 避免反复创建/销毁线程，降低内核开销与上下文切换
2. 控制最大并发数，防止线程过多压垮系统
3. 统一管理工作线程生命周期

本目录实现两种线程池：
- FixedThreadPool：固定数量线程，适合负载稳定、并发可控的场景
- CacheThread：核心线程 + 弹性扩容 + 空闲回收，适合任务量波动较大的场景

底层均依赖 SyncQueue 模板队列，生产者 Put 任务，消费者 Take 阻塞取任务。
```

## 文件结构

```
threadpool/
├── SyncQueue.hpp        阻塞任务队列（FixedThreadPool 使用）
├── SyncQueuecache.hpp   带超时返回的任务队列（CacheThread 使用）
├── FixedThreadPool.hpp  固定大小线程池
├── CacheThread.hpp      缓存/弹性线程池
└── README.md
```

## 模块1 SyncQueue.hpp（固定池队列）

```
template<typename T> class SyncQueue

常量：MAX_TASK_NUM = 100（FixedThreadPool 构造时使用）

成员：
  queue_          std::list<T> 任务链表
  mutex_          互斥锁
  m_notEmpty      消费者等待「队列非空」
  m_notFull       生产者等待「队列未满」
  maxSize_        队列容量上限
  m_needStop      停止标志

核心接口：
  Put(task)       队满则阻塞，直到有空位或 Stop
  Take(task)      队空则阻塞，直到有任务或 Stop
  Take(list)      一次性取走队列中全部任务（批量）
  Stop()          置 m_needStop，唤醒所有等待线程
  Empty/Full/Size 查询队列状态

设计：经典「有界阻塞队列 + 条件变量」，Take/Put 在 Stop 后安全退出。
```

## 模块2 SyncQueuecache.hpp（缓存池队列）

```
template<typename T> class SyncQueue

与 SyncQueue.hpp 同名但实现不同，专供 CacheThread 使用。

差异：
1. wait_for 带超时（m_waitTime 秒），不无限阻塞
2. put / Take 返回 int 状态码：
   0 = 成功
   1 = 超时（队列满或空且未到 Stop）
   2 = 已 Stop
3. HasTask() 供工作线程判断「队列里是否还有积压任务」

用途：工作线程 Take 超时后可做扩容/缩容决策，而不是一直挂起。
```

## 模块3 FixedThreadPool.hpp

```
固定大小线程池：构造时创建 numThreads 个线程，数量不再变化。

成员：
  threads_     std::list<std::shared_ptr<std::thread>>
  taskQueue_   SyncQueue<Task>，容量 MAX_TASK_NUM
  isRunning_   原子运行标志
  stopFlag_    std::once_flag，保证 Stop 只执行一次

流程：
1. 构造 → Start(numThreads) → 每个线程执行 Worker()
2. Worker 循环：taskQueue_.Take(task) → 执行 task()
3. AddTask     提交 std::function<void()> 任务（支持左值/右值）
4. Stop / 析构 call_once → StopThreadGroup()
   - isRunning_ = false
   - taskQueue_.Stop() 唤醒阻塞中的 Worker
   - join 所有线程

典型用法（见 main.cpp）：
  FixedThreadPool pool(4);
  pool.AddTask([i]() { LOG_INFO << "Task " << i; });
```

## 模块4 CacheThread.hpp

```
缓存线程池（类似 Java ThreadPoolExecutor 的简化版）：
核心线程常驻，任务多时扩容，空闲过久且超过核心数时回收线程。

成员：
  m_threads        thread::id → shared_ptr<thread> 映射
  m_queue          SyncQueuecache 版任务队列
  m_coreThreads    核心线程数（保底不回收）
  m_maxThreads     最大线程数
  m_queueWaitSec   队列 wait 超时（秒）
  m_keepAliveSec   空闲超过该秒数且线程数 > core 则回收
  m_idleThreads    当前空闲线程计数
  m_currentThreads 当前总线程数

RunThread 逻辑：
1. Take 成功 (ret==0) → 执行任务，更新 idle 计数
2. Take 停止 (ret==2) → 退出循环
3. Take 超时 (ret==1)：
   - 若队列仍有任务且线程数 < max → AddThread() 扩容
   - 若空闲时间 ≥ keepAlive 且线程数 > core → detach 并回收本线程

StopGroup：
  m_running = false → m_queue.Stop() → join 所有线程 → 计数清零

适用：短时突发任务多、平时较闲的服务端场景。
```

## 两种线程池对比

| 对比项 | FixedThreadPool | CacheThread |
|--------|-----------------|-------------|
| 线程数 | 固定，构造时确定 | core ~ max 弹性伸缩 |
| 任务队列 | SyncQueue.hpp | SyncQueuecache.hpp |
| Take 行为 | 阻塞直到有任务 | 可超时返回 |
| 实现复杂度 | 低 | 较高 |
| 适用场景 | 稳定并发、逻辑简单 | 负载波动、需按需扩缩 |

## 与主工程的关系

```
main.cpp 当前示例使用 FixedThreadPool：
  #include "FixedThreadPool.hpp"
  FixedThreadPool pool(4);
  pool.AddTask(...);

编译时需将 threadpool 目录加入 include 路径（若尚未加入 CMakeLists.txt）。
CacheThread 为进阶实现，需单独 include 并传入 core/max/超时等参数。
```

## 后续可扩展方向

```
1. WerkStealingPool：多队列 + 窃取，降低锁竞争
2. ScheduledThreadPool：延迟/周期任务（timer + 线程池）
3. 统一 ThreadPool 接口，Fixed / Cache 可替换
4. 任务提交返回 future，支持结果回调
```
