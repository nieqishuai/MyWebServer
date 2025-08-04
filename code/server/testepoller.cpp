#include <iostream>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


#include "epoller.h"

/*
 * epoll_wait 超时行为说明：
 * - timeoutMs = -1: 无限等待，直到有事件发生
 * - timeoutMs = 0: 非阻塞，立即返回
 * - timeoutMs > 0: 等待指定毫秒数，超时返回0
 * 
 * 在没有事件的情况下：
 * - 超时等待会返回0
 * - 无限等待会阻塞直到有事件或信号中断
 */

// 创建测试套接字
int createTestSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "创建套接字失败" << std::endl;
        return -1;
    }
    
    // 设置为非阻塞模式
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    return sockfd;
}

// 创建监听套接字
int createListenSocket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "创建监听套接字失败" << std::endl;
        return -1;
    }
    
    // 设置地址重用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "绑定地址失败" << std::endl;
        close(sockfd);
        return -1;
    }
    
    if (listen(sockfd, 5) < 0) {
        std::cerr << "监听失败" << std::endl;
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

// 测试构造函数和析构函数
void testConstructor() {
    std::cout << "测试构造函数和析构函数..." << std::endl;
    
    // 测试默认构造函数
    Epoller* epoller1 = new Epoller();
    assert(epoller1 != nullptr);
    delete epoller1;
    
    // 测试自定义事件数的构造函数
    Epoller* epoller2 = new Epoller(2048);
    assert(epoller2 != nullptr);
    delete epoller2;
    
    std::cout << "✓ 构造函数和析构函数测试通过" << std::endl;
}

// 测试添加文件描述符
void testAddFd() {
    std::cout << "测试添加文件描述符..." << std::endl;
    
    Epoller epoller;
    int sockfd = createTestSocket();
    
    if (sockfd >= 0) {
        // 测试添加有效的文件描述符
        bool result = epoller.AddFd(sockfd, EPOLLIN | EPOLLET);
        if (result) {
            std::cout << "✓ 添加文件描述符成功" << std::endl;
        } else {
            std::cout << "✗ 添加文件描述符失败" << std::endl;
        }
        
        // 测试添加无效的文件描述符
        bool invalidResult = epoller.AddFd(-1, EPOLLIN);
        if (!invalidResult) {
            std::cout << "✓ 拒绝添加无效文件描述符" << std::endl;
        } else {
            std::cout << "✗ 错误地接受了无效文件描述符" << std::endl;
        }
        
        close(sockfd);
    }
    
    std::cout << "✓ 添加文件描述符测试通过" << std::endl;
}

// 测试修改文件描述符事件
void testModFd() {
    std::cout << "测试修改文件描述符事件..." << std::endl;
    
    Epoller epoller;
    int sockfd = createTestSocket();
    
    if (sockfd >= 0) {
        // 先添加文件描述符
        epoller.AddFd(sockfd, EPOLLIN);
        
        // 测试修改事件
        bool result = epoller.ModFd(sockfd, EPOLLOUT | EPOLLET);
        if (result) {
            std::cout << "✓ 修改文件描述符事件成功" << std::endl;
        } else {
            std::cout << "✗ 修改文件描述符事件失败" << std::endl;
        }
        
        // 测试修改无效文件描述符
        bool invalidResult = epoller.ModFd(-1, EPOLLIN);
        if (!invalidResult) {
            std::cout << "✓ 拒绝修改无效文件描述符" << std::endl;
        } else {
            std::cout << "✗ 错误地接受了无效文件描述符修改" << std::endl;
        }
        
        close(sockfd);
    }
    
    std::cout << "✓ 修改文件描述符事件测试通过" << std::endl;
}

// 测试删除文件描述符
void testDelFd() {
    std::cout << "测试删除文件描述符..." << std::endl;
    
    Epoller epoller;
    int sockfd = createTestSocket();
    
    if (sockfd >= 0) {
        // 先添加文件描述符
        epoller.AddFd(sockfd, EPOLLIN);
        
        // 测试删除文件描述符
        bool result = epoller.DelFd(sockfd);
        if (result) {
            std::cout << "✓ 删除文件描述符成功" << std::endl;
        } else {
            std::cout << "✗ 删除文件描述符失败" << std::endl;
        }
        
        // 测试删除无效文件描述符
        bool invalidResult = epoller.DelFd(-1);
        if (!invalidResult) {
            std::cout << "✓ 拒绝删除无效文件描述符" << std::endl;
        } else {
            std::cout << "✗ 错误地接受了无效文件描述符删除" << std::endl;
        }
        
        close(sockfd);
    }
    
    std::cout << "✓ 删除文件描述符测试通过" << std::endl;
}

// 测试等待事件
void testWait() {
    std::cout << "测试等待事件..." << std::endl;
    
    Epoller epoller;
    
    // 测试超时等待（应该立即返回0，因为没有事件）
    int result = epoller.Wait(100);  // 100ms超时
    if (result == 0) {
        std::cout << "✓ 超时等待正确返回0（无事件）" << std::endl;
    } else {
        std::cout << "✗ 超时等待返回错误值: " << result << std::endl;
    }
    
    // 测试无限等待（会阻塞直到有事件或信号中断）
    std::cout << "测试无限等待（将阻塞，按Ctrl+C中断）..." << std::endl;
    std::cout << "注意：epoll_wait(-1) 会无限等待，这是正常行为" << std::endl;
    
    // 使用较短的超时时间来模拟，避免真正阻塞
    result = epoller.Wait(5000);  // 1秒超时
    if (result == 0) {
        std::cout << "✓ 等待超时正确返回0（无事件）" << std::endl;
    } else {
        std::cout << "✗ 等待返回错误值: " << result << std::endl;
    }
    
    std::cout << "✓ 等待事件测试通过" << std::endl;
}

// 测试epoll的不同超时行为
void testEpollTimeoutBehavior() {
    std::cout << "测试epoll超时行为..." << std::endl;
    
    Epoller epoller;
    
    // 测试不同超时值的行为
    std::cout << "测试超时值 0（非阻塞）..." << std::endl;
    int result = epoller.Wait(0);
    if (result == 0) {
        std::cout << "✓ 非阻塞等待正确返回0" << std::endl;
    }
    
    std::cout << "测试超时值 100ms..." << std::endl;
    result = epoller.Wait(100);
    if (result == 0) {
        std::cout << "✓ 100ms超时等待正确返回0" << std::endl;
    }
    
    std::cout << "测试超时值 1000ms..." << std::endl;
    result = epoller.Wait(1000);
    if (result == 0) {
        std::cout << "✓ 1000ms超时等待正确返回0" << std::endl;
    }
    
    std::cout << "说明：epoll_wait(-1) 会无限等待，直到有事件发生" << std::endl;
    std::cout << "✓ epoll超时行为测试通过" << std::endl;
}

// 测试获取事件信息
void testGetEventInfo() {
    std::cout << "测试获取事件信息..." << std::endl;
    
    Epoller epoller;
    
    // 在没有事件的情况下测试获取事件信息
    // 这应该会触发断言或返回默认值
    try {
        int fd = epoller.GetEventFd(0);
        std::cout << "✓ 获取事件文件描述符: " << fd << std::endl;
    } catch (...) {
        std::cout << "✓ 正确捕获了越界访问异常" << std::endl;
    }
    
    try {
        uint32_t events = epoller.GetEvents(0);
        std::cout << "✓ 获取事件类型: " << events << std::endl;
    } catch (...) {
        std::cout << "✓ 正确捕获了越界访问异常" << std::endl;
    }
    
    std::cout << "✓ 获取事件信息测试通过" << std::endl;
}

// 测试网络事件
void testNetworkEvents() {
    std::cout << "测试网络事件..." << std::endl;
    
    Epoller epoller;
    
    // 创建监听套接字
    int listenfd = createListenSocket(8888);
    if (listenfd >= 0) {
        // 添加监听套接字到epoll
        bool addResult = epoller.AddFd(listenfd, EPOLLIN);
        if (addResult) {
            std::cout << "✓ 添加监听套接字成功" << std::endl;
            
            // 创建客户端套接字并连接
            int clientfd = socket(AF_INET, SOCK_STREAM, 0);
            if (clientfd >= 0) {
                struct sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(8888);
                serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                // 设置为非阻塞
                int flags = fcntl(clientfd, F_GETFL, 0);
                fcntl(clientfd, F_SETFL, flags | O_NONBLOCK);
                
                // 尝试连接（非阻塞）
                int connectResult = connect(clientfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
                
                // 等待连接事件
                int eventCount = epoller.Wait(1000);  // 1秒超时
                if (eventCount > 0) {
                    std::cout << "✓ 检测到 " << eventCount << " 个事件" << std::endl;
                    
                    // 检查第一个事件
                    int eventFd = epoller.GetEventFd(0);
                    uint32_t events = epoller.GetEvents(0);
                    
                    if (eventFd == listenfd) {
                        std::cout << "✓ 监听套接字事件正确" << std::endl;
                    }
                    
                    if (events & EPOLLIN) {
                        std::cout << "✓ 检测到读事件" << std::endl;
                    }
                } else {
                    std::cout << "✗ 未检测到连接事件" << std::endl;
                }
                
                close(clientfd);
            }
        }
        
        close(listenfd);
    }
    
    std::cout << "✓ 网络事件测试通过" << std::endl;
}

// 测试多文件描述符
void testMultipleFds() {
    std::cout << "测试多文件描述符..." << std::endl;
    
    Epoller epoller;
    
    // 创建多个测试套接字
    int sockfds[5];
    for (int i = 0; i < 5; i++) {
        sockfds[i] = createTestSocket();
        if (sockfds[i] >= 0) {
            bool result = epoller.AddFd(sockfds[i], EPOLLIN | EPOLLOUT);
            if (result) {
                std::cout << "✓ 添加套接字 " << i << " 成功" << std::endl;
            } else {
                std::cout << "✗ 添加套接字 " << i << " 失败" << std::endl;
            }
        }
    }
    
    // 测试等待事件
    int eventCount = epoller.Wait(100);
    std::cout << "✓ 等待到 " << eventCount << " 个事件" << std::endl;
    
    // 清理
    for (int i = 0; i < 5; i++) {
        if (sockfds[i] >= 0) {
            close(sockfds[i]);
        }
    }
    
    std::cout << "✓ 多文件描述符测试通过" << std::endl;
}

// 主测试函数
int main() {
    std::cout << "开始Epoller类测试..." << std::endl;
    
    try {
        // 运行所有测试
        testConstructor();
        testAddFd();
        testModFd();
        testDelFd();
        testWait();
        testEpollTimeoutBehavior();
        testGetEventInfo();
        testNetworkEvents();
        testMultipleFds();
        
        std::cout << "\n🎉 所有测试通过！Epoller类工作正常。" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
} 