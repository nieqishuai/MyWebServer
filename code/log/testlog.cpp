#include "log.h"
#include <thread>
#include <chrono>

void WriteSampleLogs(int threadId) {
    for(int i = 0; i < 100; ++i) {
        LOG_DEBUG("Thread %d - Debug message %d", threadId, i);
        LOG_INFO("Thread %d - Info message %d", threadId, i);
        LOG_WARN("Thread %d - Warning message %d", threadId, i);
        LOG_ERROR("Thread %d - Error message %d", threadId, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    // 初始化日志系统（异步模式，日志等级设为0：DEBUG）
    Log::Instance()->init(3, "./log_test", ".log", 1024);

    // 启动多个线程模拟高并发写日志
    std::thread t1(WriteSampleLogs, 1);
    std::thread t2(WriteSampleLogs, 2);
    std::thread t3(WriteSampleLogs, 3);

    t1.join();
    t2.join();
    t3.join();

    // 等待异步日志线程写完所有日志
    Log::Instance()->flush();

    return 0;
}
