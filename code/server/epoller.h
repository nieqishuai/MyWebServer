#ifndef EPOLLER_H
#define EPOLLER_H

// 包含必要的系统头文件
#include <sys/epoll.h> // epoll_ctl() - epoll系统调用
#include <fcntl.h>     // fcntl() - 文件控制操作
#include <unistd.h>    // close() - 关闭文件描述符
#include <assert.h>    // assert() - 断言宏
#include <vector>      // std::vector - 动态数组容器
#include <errno.h>     // errno - 错误码定义

class Epoller {
public:
    // 构造函数，创建epoll实例，maxEvent为最大监听事件数
    explicit Epoller(int maxEvent = 1024);

    // 析构函数，关闭epoll实例
    ~Epoller();

    // 添加文件描述符到epoll监听列表
    bool AddFd(int fd, uint32_t events);

    // 修改文件描述符的监听事件
    bool ModFd(int fd, uint32_t events);

    // 从epoll监听列表中删除文件描述符
    bool DelFd(int fd);

    // 等待事件发生，timeoutMs为超时时间（毫秒），-1表示无限等待
    int Wait(int timeoutMs = -1);

    // 获取第i个事件的文件描述符
    int GetEventFd(size_t i) const;

    // 获取第i个事件的事件类型
    uint32_t GetEvents(size_t i) const;
        
private:
    int epollFd_;                                    // epoll实例的文件描述符

    std::vector<struct epoll_event> events_;        // 存储epoll事件的动态数组
};

#endif //EPOLLER_H