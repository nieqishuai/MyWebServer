#include "threadpool.h"
#include <iostream>
#include <vector>
#include <atomic>
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

// 功能测试：验证基本任务执行
void testBasicFunctionality() {
    cout << "=== 测试基本功能 ===" << endl;
    ThreadPool pool(4);
    atomic<int> counter(0);

    // 提交10个递增计数器的任务
    for (int i = 0; i < 10; ++i) {
        pool.AddTask([&counter]() {
            this_thread::sleep_for(milliseconds(10));
            counter.fetch_add(1, memory_order_relaxed);
        });
    }

    // 等待所有任务完成（简单等待，实际中应使用更可靠的同步机制）
    this_thread::sleep_for(seconds(1));
    cout << "计数器值: " << counter << " (预期: 10)" << endl;
}

// 测试任务参数传递
void testParameterPassing() {
    cout << "\n=== 测试参数传递 ===" << endl;
    ThreadPool pool(2);
    vector<int> results;
    mutex mtx;

    // 提交带参数的任务
    for (int i = 0; i < 5; ++i) {
        pool.AddTask([i, &results, &mtx]() {
            this_thread::sleep_for(milliseconds(50));
            lock_guard<mutex> lock(mtx);
            results.push_back(i * 2);
        });
    }

    // 等待所有任务完成
    this_thread::sleep_for(seconds(1));

    // 验证结果
    cout << "结果: ";
    for (int val : results) {
        cout << val << " ";
    }
    cout << endl;
}

// 测试异常处理
void testExceptionHandling() {
    cout << "\n=== 测试异常处理 ===" << endl;
    ThreadPool pool(2);
    atomic<int> successCount(0);
    atomic<int> exceptionCount(0);

    // 提交会抛出异常的任务
    for (int i = 0; i < 3; ++i) {
        pool.AddTask([&]() {
            try {
                if (rand() % 2 == 0) {
                    throw runtime_error("测试异常");
                }
                successCount++;
            } catch (const exception& e) {
                exceptionCount++;
            }
        });
    }

    // 等待所有任务完成
    this_thread::sleep_for(seconds(1));
    cout << "成功任务数: " << successCount << endl;
    cout << "异常任务数: " << exceptionCount << endl;
}

// 压力测试：高并发场景
void testStress() {
    cout << "\n=== 压力测试 ===" << endl;
    const int TASK_COUNT = 10000;
    const int THREAD_COUNT = 16;
    ThreadPool pool(THREAD_COUNT);
    atomic<int> completed(0);

    auto start = high_resolution_clock::now();

    // 提交大量任务
    for (int i = 0; i < TASK_COUNT; ++i) {
        pool.AddTask([&completed]() {
            // 模拟一些工作
            for (int j = 0; j < 1000; ++j) {
                // 空循环
            }
            completed.fetch_add(1, memory_order_relaxed);
        });
    }

    // 等待所有任务完成
    while (completed < TASK_COUNT) {
        this_thread::sleep_for(milliseconds(10));
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "完成 " << TASK_COUNT << " 个任务耗时: " << duration << "ms" << endl;
    cout << "吞吐量: " << TASK_COUNT * 1000.0 / duration << " 任务/秒" << endl;
}

// 测试线程池析构时的行为
void testDestructorBehavior() {
    cout << "\n=== 测试线程池析构 ===" << endl;
    atomic<bool> taskStarted(false);
    atomic<bool> taskCompleted(false);

    {
        // 创建临时线程池
        ThreadPool pool(1);
        
        // 提交一个长时间运行的任务
        pool.AddTask([&]() {
            taskStarted = true;
            this_thread::sleep_for(seconds(2));
            taskCompleted = true;
        });

        // 等待任务开始
        while (!taskStarted) {
            this_thread::sleep_for(milliseconds(10));
        }

        // 线程池在此处析构
    }

    // 验证任务是否被中断
    cout << "任务完成状态: " << (taskCompleted ? "已完成" : "被中断") << endl;
    cout << "注意: 线程池析构时应中断未完成的任务" << endl;
}

int main() {
    srand(time(nullptr));
    
    testBasicFunctionality();
    testParameterPassing();
    testExceptionHandling();
    testStress();
    testDestructorBehavior();

    cout << "\n=== 所有测试完成 ===" << endl;
    return 0;
}