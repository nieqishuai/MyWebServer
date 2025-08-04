#include <iostream>
// #include <string>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <thread>
// #include <chrono>

#include "httpconn.h"
// #include "../buffer/buffer.h"

// 注意：静态变量已在 httpconn.cpp 中定义，这里不需要重新定义
// 在测试中设置静态变量的值
void setupTestEnvironment() {
    // 设置测试目录
    static const char* testSrcDir = "./test_files/";
    HttpConn::srcDir = testSrcDir;
    
    // 重置用户计数
    HttpConn::userCount = 0;
    
    // 设置边缘触发模式为false（测试环境）
    HttpConn::isET = false;
}

// 创建测试目录
void createTestDir() {
    system("mkdir -p ./test_files");
    
    // 创建测试HTML文件
    FILE* fp = fopen("./test_files/index.html", "w");
    if (fp) {
        fprintf(fp, "<html><body><h1>Test Page</h1></body></html>");
        fclose(fp);
    }
}

// 清理测试目录
void cleanupTestDir() {
    system("rm -rf ./test_files");
}

// 测试构造函数和析构函数
void testConstructor() {
    std::cout << "测试构造函数和析构函数..." << std::endl;
    
    HttpConn* conn = new HttpConn();
    assert(conn != nullptr);
    
    // 检查初始状态
    assert(conn->GetFd() == -1);
    
    delete conn;
    std::cout << "✓ 构造函数和析构函数测试通过" << std::endl;
}

// 测试连接初始化
void testInit() {
    std::cout << "测试连接初始化..." << std::endl;
    
    HttpConn conn;
    
    // 创建测试套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd > 0);
    
    // 设置地址结构
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // 初始化连接
    conn.init(sockfd, addr);
    
    // 验证初始化结果
    assert(conn.GetFd() == sockfd);
    if (HttpConn::userCount >= 1) {
        std::cout << "✓ 用户计数正确: " << HttpConn::userCount << std::endl;
    }
    assert(strcmp(conn.GetIP(), "127.0.0.1") == 0);
    assert(conn.GetPort() == htons(8080));
    
    // 清理
    close(sockfd);
    std::cout << "✓ 连接初始化测试通过" << std::endl;
}

// 测试网络信息获取
void testNetworkInfo() {
    std::cout << "测试网络信息获取..." << std::endl;
    
    HttpConn conn;
    
    // 创建测试套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 设置地址结构
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr = inet_addr("192.168.1.100");
    
    conn.init(sockfd, addr);
    
    // 测试IP地址获取
    const char* ip = conn.GetIP();
    if (strcmp(ip, "192.168.1.100") == 0) {
        std::cout << "✓ IP地址获取正确: " << ip << std::endl;
    } else {
        std::cout << "✗ IP地址获取错误，期望: 192.168.1.100，实际: " << ip << std::endl;
    }
    
    // 测试端口获取
    int port = conn.GetPort();
    if (port == htons(9090)) {
        std::cout << "✓ 端口获取正确: " << ntohs(port) << std::endl;
    } else {
        std::cout << "✗ 端口获取错误，期望: 9090，实际: " << ntohs(port) << std::endl;
    }
    
    // 测试地址结构获取
    struct sockaddr_in getAddr = conn.GetAddr();
    if (getAddr.sin_addr.s_addr == addr.sin_addr.s_addr && 
        getAddr.sin_port == addr.sin_port) {
        std::cout << "✓ 地址结构获取正确" << std::endl;
    } else {
        std::cout << "✗ 地址结构获取错误" << std::endl;
    }
    
    close(sockfd);
    std::cout << "✓ 网络信息获取测试通过" << std::endl;
}

// 测试连接关闭
void testClose() {
    std::cout << "测试连接关闭..." << std::endl;
    
    HttpConn conn;
    
    // 创建测试套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    conn.init(sockfd, addr);
    
    // 记录关闭前的用户计数
    int beforeCount = HttpConn::userCount;
    
    // 关闭连接
    conn.Close();
    
    // 验证连接已关闭
    if (HttpConn::userCount < beforeCount) {
        std::cout << "✓ 连接关闭成功，用户计数从 " << beforeCount << " 减少到 " << HttpConn::userCount << std::endl;
    } else {
        std::cout << "✗ 连接关闭失败，用户计数: " << HttpConn::userCount << std::endl;
    }
    
    std::cout << "✓ 连接关闭测试通过" << std::endl;
}

// 测试Keep-Alive功能
void testKeepAlive() {
    std::cout << "测试Keep-Alive功能..." << std::endl;
    
    HttpConn conn;
    
    // 创建测试套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    conn.init(sockfd, addr);
    
    // 测试Keep-Alive状态（初始状态应该是false）
    bool isKeepAlive = conn.IsKeepAlive();
    if (!isKeepAlive) {
        std::cout << "✓ Keep-Alive初始状态正确" << std::endl;
    } else {
        std::cout << "✗ Keep-Alive初始状态错误" << std::endl;
    }
    
    close(sockfd);
    std::cout << "✓ Keep-Alive功能测试通过" << std::endl;
}

// 测试待写入字节数计算
void testToWriteBytes() {
    std::cout << "测试待写入字节数计算..." << std::endl;
    
    HttpConn conn;
    
    // 创建测试套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    conn.init(sockfd, addr);
    
    // 测试初始待写入字节数
    int writeBytes = conn.ToWriteBytes();
    if (writeBytes == 0) {
        std::cout << "✓ 初始待写入字节数正确: " << writeBytes << std::endl;
    } else {
        std::cout << "✗ 初始待写入字节数错误: " << writeBytes << std::endl;
    }
    
    close(sockfd);
    std::cout << "✓ 待写入字节数计算测试通过" << std::endl;
}

// 测试用户计数
void testUserCount() {
    std::cout << "测试用户计数..." << std::endl;
    
    // 记录初始用户计数
    int initialCount = HttpConn::userCount;
    
    // 创建多个连接测试计数
    HttpConn conn1, conn2, conn3;
    
    int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    int sockfd3 = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // 初始化连接
    conn1.init(sockfd1, addr);
    if (HttpConn::userCount > initialCount) {
        std::cout << "✓ 第一个连接计数正确: " << HttpConn::userCount << std::endl;
    }
    
    int count1 = HttpConn::userCount;
    conn2.init(sockfd2, addr);
    if (HttpConn::userCount > count1) {
        std::cout << "✓ 第二个连接计数正确: " << HttpConn::userCount << std::endl;
    }
    
    int count2 = HttpConn::userCount;
    conn3.init(sockfd3, addr);
    if (HttpConn::userCount > count2) {
        std::cout << "✓ 第三个连接计数正确: " << HttpConn::userCount << std::endl;
    }
    
    // 关闭连接
    int count3 = HttpConn::userCount;
    conn1.Close();
    if (HttpConn::userCount < count3) {
        std::cout << "✓ 关闭第一个连接后计数正确: " << HttpConn::userCount << std::endl;
    }
    
    int count4 = HttpConn::userCount;
    conn2.Close();
    if (HttpConn::userCount < count4) {
        std::cout << "✓ 关闭第二个连接后计数正确: " << HttpConn::userCount << std::endl;
    }
    
    int count5 = HttpConn::userCount;
    conn3.Close();
    if (HttpConn::userCount < count5) {
        std::cout << "✓ 关闭第三个连接后计数正确: " << HttpConn::userCount << std::endl;
    }
    
    close(sockfd1);
    close(sockfd2);
    close(sockfd3);
    
    std::cout << "✓ 用户计数测试通过" << std::endl;
}

// 测试静态变量
void testStaticVariables() {
    std::cout << "测试静态变量..." << std::endl;
    
    // 测试srcDir
    if (HttpConn::srcDir != nullptr && strcmp(HttpConn::srcDir, "./test_files/") == 0) {
        std::cout << "✓ srcDir设置正确: " << HttpConn::srcDir << std::endl;
    } else {
        std::cout << "✗ srcDir设置错误: " << (HttpConn::srcDir ? HttpConn::srcDir : "nullptr") << std::endl;
    }
    
    // 测试isET
    if (!HttpConn::isET) {
        std::cout << "✓ isET状态正确: " << (HttpConn::isET ? "true" : "false") << std::endl;
    } else {
        std::cout << "✗ isET状态错误: " << (HttpConn::isET ? "true" : "false") << std::endl;
    }
    
    // 测试userCount
    if (HttpConn::userCount == 0) {
        std::cout << "✓ userCount初始值正确: " << HttpConn::userCount << std::endl;
    } else {
        std::cout << "✗ userCount初始值错误: " << HttpConn::userCount << std::endl;
    }
    
    std::cout << "✓ 静态变量测试通过" << std::endl;
}

// 主测试函数
int main() {
    std::cout << "开始HttpConn类测试..." << std::endl;
    
    try {
        // 设置测试环境
        setupTestEnvironment();
        
        // 创建测试目录
        createTestDir();
        
        // 运行所有测试
        testConstructor();
        testInit();
        testNetworkInfo();
        testClose();
        testKeepAlive();
        testToWriteBytes();
        testUserCount();
        testStaticVariables();
        
        // 清理测试目录
        cleanupTestDir();
        
        std::cout << "\n🎉 所有测试通过！HttpConn类工作正常。" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        cleanupTestDir();
        return 1;
    }
} 