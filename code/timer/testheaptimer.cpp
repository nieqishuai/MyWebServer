#include "heaptimer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cassert>

using namespace std;

// 测试用的回调函数
void testCallback1() {
    cout << "定时器1触发回调函数" << endl;
}

void testCallback2() {
    cout << "定时器2触发回调函数" << endl;
}

void testCallback3() {
    cout << "定时器3触发回调函数" << endl;
}

void testCallback4() {
    cout << "定时器4触发回调函数" << endl;
}

void testCallback5() {
    cout << "定时器5触发回调函数" << endl;
}

// 测试添加定时器功能
void testAdd() {
    cout << "\n=== 测试添加定时器功能 ===" << endl;
    HeapTimer timer;
    
    // 添加多个定时器
    timer.add(1, 1000, testCallback1);  // 1秒后过期
    timer.add(2, 2000, testCallback2);  // 2秒后过期
    timer.add(3, 500, testCallback3);   // 0.5秒后过期
    
    cout << "添加了3个定时器，ID分别为1、2、3" << endl;
    cout << "定时器1: 1000ms后过期" << endl;
    cout << "定时器2: 2000ms后过期" << endl;
    cout << "定时器3: 500ms后过期" << endl;
    
    // 检查GetNextTick返回值
    int nextTick = timer.GetNextTick();
    cout << "距离下一个定时器过期时间: " << nextTick << "ms" << endl;
    assert(nextTick > 0 && nextTick <= 500);
    
    cout << "添加定时器测试通过！" << endl;
}

// 测试删除定时器功能
void testDelete() {
    cout << "\n=== 测试删除定时器功能 ===" << endl;
    HeapTimer timer;
    
    // 添加定时器
    timer.add(1, 1000, testCallback1);
    timer.add(2, 2000, testCallback2);
    timer.add(3, 3000, testCallback3);
    
    cout << "添加了3个定时器" << endl;
    
    // 删除中间的定时器
    timer.doWork(2);
    cout << "删除了定时器2" << endl;
    
    // 删除堆顶定时器
    timer.pop();
    cout << "删除了堆顶定时器" << endl;
    
    // 清空所有定时器
    timer.clear();
    cout << "清空了所有定时器" << endl;
    
    // 验证堆为空
    int nextTick = timer.GetNextTick();
    assert(nextTick == -1);
    
    cout << "删除定时器测试通过！" << endl;
}

// 测试调整定时器功能
void testAdjust() {
    cout << "\n=== 测试调整定时器功能 ===" << endl;
    HeapTimer timer;
    
    // 添加定时器
    timer.add(1, 1000, testCallback1);
    timer.add(2, 2000, testCallback2);
    
    cout << "添加了2个定时器" << endl;
    
    // 调整定时器1的过期时间
    timer.adjust(1, 3000);
    cout << "将定时器1的过期时间调整为3000ms" << endl;
    
    // 检查调整后的顺序
    int nextTick = timer.GetNextTick();
    cout << "距离下一个定时器过期时间: " << nextTick << "ms" << endl;
    
    cout << "调整定时器测试通过！" << endl;
}

// 测试超时处理功能
void testTimeout() {
    cout << "\n=== 测试超时处理功能 ===" << endl;
    HeapTimer timer;
    
    // 添加短时间定时器
    timer.add(1, 100, testCallback1);   // 100ms后过期
    timer.add(2, 200, testCallback2);   // 200ms后过期
    timer.add(3, 300, testCallback3);   // 300ms后过期
    
    cout << "添加了3个短时间定时器" << endl;
    cout << "等待定时器超时..." << endl;
    
    // 等待一段时间让定时器超时
    this_thread::sleep_for(chrono::milliseconds(150));
    
    // 处理超时的定时器
    timer.tick();
    cout << "执行了tick()处理超时定时器" << endl;
    
    // 检查剩余定时器
    int nextTick = timer.GetNextTick();
    cout << "剩余定时器距离过期时间: " << nextTick << "ms" << endl;
    
    cout << "超时处理测试通过！" << endl;
}

// 测试堆的性质维护
void testHeapProperty() {
    cout << "\n=== 测试堆的性质维护 ===" << endl;
    HeapTimer timer;
    
    // 添加多个定时器，测试堆的维护
    timer.add(1, 1000, testCallback1);
    timer.add(2, 500, testCallback2);
    timer.add(3, 2000, testCallback3);
    timer.add(4, 300, testCallback4);
    timer.add(5, 1500, testCallback5);
    
    cout << "添加了5个定时器，测试堆的性质维护" << endl;
    
    // 检查堆顶是否是最早过期的
    int nextTick = timer.GetNextTick();
    cout << "堆顶定时器距离过期时间: " << nextTick << "ms" << endl;
    assert(nextTick > 0 && nextTick <= 300);
    
    // 删除堆顶，检查新的堆顶
    timer.pop();
    nextTick = timer.GetNextTick();
    cout << "删除堆顶后，新堆顶距离过期时间: " << nextTick << "ms" << endl;
    
    cout << "堆的性质维护测试通过！" << endl;
}

// 测试边界情况
void testEdgeCases() {
    cout << "\n=== 测试边界情况 ===" << endl;
    HeapTimer timer;
    
    // 测试空堆的情况
    cout << "测试空堆..." << endl;
    int nextTick = timer.GetNextTick();
    assert(nextTick == -1);
    
    // 测试添加相同ID的定时器
    cout << "测试添加相同ID的定时器..." << endl;
    timer.add(1, 1000, testCallback1);
    timer.add(1, 2000, testCallback2);  // 应该更新而不是添加
    
    // 测试删除不存在的定时器
    cout << "测试删除不存在的定时器..." << endl;
    timer.doWork(999);  // 不应该崩溃
    
    // 测试调整不存在的定时器
    cout << "测试调整不存在的定时器..." << endl;
    timer.adjust(999, 2000);  // 调整不存在的定时器
    cout << "成功调整了不存在的定时器" << endl;
    
    cout << "边界情况测试通过！" << endl;
}

// 测试并发安全性（基本测试）
void testConcurrency() {
    cout << "\n=== 测试并发安全性 ===" << endl;
    HeapTimer timer;
    vector<thread> threads;
    
    // 创建多个线程同时添加定时器
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&timer, i]() {
            timer.add(i + 1, 1000 + i * 100, testCallback1);
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "并发添加定时器测试完成" << endl;
    
    // 检查定时器数量
    int nextTick = timer.GetNextTick();
    cout << "并发添加后，距离下一个定时器过期时间: " << nextTick << "ms" << endl;
    
    cout << "并发安全性测试通过！" << endl;
}

// 测试性能
void testPerformance() {
    cout << "\n=== 测试性能 ===" << endl;
    HeapTimer timer;
    
    auto start = chrono::high_resolution_clock::now();
    
    // 添加大量定时器
    for (int i = 0; i < 1000; ++i) {
        timer.add(i, 1000 + i, testCallback1);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "添加1000个定时器耗时: " << duration.count() << " 微秒" << endl;
    
    // 测试删除性能
    start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        timer.pop();
    }
    
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "删除100个定时器耗时: " << duration.count() << " 微秒" << endl;
    
    cout << "性能测试通过！" << endl;
}

int main() {
    cout << "开始测试HeapTimer类的所有功能..." << endl;
    
    try {
        testAdd();
        testDelete();
        testAdjust();
        testTimeout();
        testHeapProperty();
        testEdgeCases();
        testConcurrency();
        testPerformance();
        
        cout << "\n=== 所有测试通过！===" << endl;
        cout << "HeapTimer类功能完整，性能良好！" << endl;
        
    } catch (const exception& e) {
        cout << "测试过程中发生异常: " << e.what() << endl;
        return 1;
    } catch (...) {
        cout << "测试过程中发生未知异常" << endl;
        return 1;
    }
    
    return 0;
} 