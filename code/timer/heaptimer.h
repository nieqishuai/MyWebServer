#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>
#include "../log/log.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};
class HeapTimer {
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { clear(); }
    
    void adjust(int id, int newExpires); //调整指定ID的定时器过期时间

    void add(int id, int timeOut, const TimeoutCallBack& cb); //添加定时器

    void doWork(int id); //执行指定ID的定时器回调函数

    void clear(); //清空定时器

    void tick(); //处理已到期的定时器

    void pop(); //删除堆顶的定时器

    int GetNextTick(); //获取下一个定时器的到期时间

private:
    void del_(size_t i); //删除指定位置的节点
    
    void siftup_(size_t i); //向上调整堆

    bool siftdown_(size_t index, size_t n); //向下调整堆

    void SwapNode_(size_t i, size_t j); //交换两个节点在堆中的位置

    std::vector<TimerNode> heap_;//存储定时器节点的向量，每个节点包含 ID、过期时间和回调函数，
                                // 存储定时器节点的最小堆，按过期时间排序，堆顶元素为最近要执行的定时器

    std::unordered_map<int, size_t> ref_;//哈希表，用于快速查找 ID 对应的堆中位置，
                                        // 存储定时器 ID 到其在堆中位置的映射，用于快速查找和更新定时器
};

#endif //HEAP_TIMER_H