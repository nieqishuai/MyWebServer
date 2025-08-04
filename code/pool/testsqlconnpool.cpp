#include "sqlconnpool.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>

// 测试单个连接的基本操作
void testSingleConnection() {
    std::cout << "=== 测试单连接基本操作 ===" << std::endl;
    auto pool = SqlConnPool::Instance();
    MYSQL* conn = pool->GetConn();
    if (!conn) {
        std::cerr << "获取连接失败!" << std::endl;
        return;
    }

    // 执行简单SQL查询
    if (mysql_query(conn, "SELECT 1")) {
        std::cerr << "SQL执行失败: " << mysql_error(conn) << std::endl;
    } else {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row && row[0]) {
                std::cout << "查询结果: " << row[0] << std::endl;
            }
            mysql_free_result(res); // 释放结果集
        }
    }

    pool->FreeConn(conn);
    std::cout << "单连接测试完成" << std::endl;
}

// 线程函数：模拟并发操作
std::atomic_int successCount(0);
std::atomic_int failCount(0);

void threadTask(int id, int opCount) {
    auto pool = SqlConnPool::Instance();
    for (int i = 0; i < opCount; ++i) {
        MYSQL* conn = pool->GetConn();
        if (!conn) {
            std::cerr << "线程" << id << "获取连接失败!" << std::endl;
            ++failCount;
            continue;
        }

        // 执行SQL查询
        if (mysql_query(conn, "SELECT SLEEP(0.01)")) {
            std::cerr << "线程" << id << "查询失败: " << mysql_error(conn) << std::endl;
            ++failCount;
        } else {
            // 关键：获取并释放结果集，确保连接状态干净
            MYSQL_RES* res = mysql_store_result(conn);
            if (res) {
                mysql_free_result(res); // 释放结果集
            } else {
                // 处理无结果集的情况（如SLEEP实际无返回，但仍需确认无错误）
                if (mysql_field_count(conn) != 0) {
                    std::cerr << "线程" << id << "结果集获取失败: " << mysql_error(conn) << std::endl;
                    ++failCount;
                    pool->FreeConn(conn);
                    continue;
                }
            }
            ++successCount;
        }

        pool->FreeConn(conn);
    }
}

// 测试多线程并发
void testMultiThread(int threadCount = 10, int opPerThread = 20) {
    std::cout << "\n=== 测试多线程并发 (" 
              << threadCount << "个线程，每个线程" << opPerThread << "次操作) ===" << std::endl;
    
    auto pool = SqlConnPool::Instance();
    std::vector<std::thread> threads;
    
    // 启动多个线程
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(threadTask, i, opPerThread);
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "成功操作次数: " << successCount << std::endl;
    std::cout << "失败操作次数: " << failCount << std::endl;
    std::cout << "空闲连接数: " << pool->GetFreeConnCount() << std::endl;
    std::cout << "多线程测试完成" << std::endl;
}

int main() {
    // 初始化连接池（请根据实际数据库信息修改）
    auto pool = SqlConnPool::Instance();
    pool->Init("localhost", 3306, "nieqishuai", "1", "tinyweb", 5);

    // 测试单连接
    testSingleConnection();

    // 测试多线程并发
    testMultiThread(10, 20);

    // 关闭连接池（程序退出时自动调用析构函数）
    return 0;
}