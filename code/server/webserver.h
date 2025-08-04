#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "../log/log.h"
#include "../timer/heaptimer.h"
#include "../pool/sqlconnpool.h"
#include "../pool/threadpool.h"
#include "../pool/sqlconnRAII.h"
#include "../http/httpconn.h"

class WebServer {
public:
    WebServer(
        int port, int trigMode, int timeoutMS, bool OptLinger, 
        int sqlPort, const char* sqlUser, const  char* sqlPwd, 
        const char* dbName, int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize);

    ~WebServer();
    void Start();

private:
    bool InitSocket_();           // 初始化监听套接字
    void InitEventMode_(int trigMode);  // 初始化事件触发模式
    void AddClient_(int fd, sockaddr_in addr);  // 添加新客户端连接
  
    void DealListen_();           // 处理监听套接字事件（新连接）
    void DealWrite_(HttpConn* client);   // 处理写事件
    void DealRead_(HttpConn* client);    // 处理读事件

    void SendError_(int fd, const char* info);   // 发送错误信息
    void ExtentTime_(HttpConn* client);          // 延长连接超时时间
    void CloseConn_(HttpConn* client);           // 关闭客户端连接

    void OnRead_(HttpConn* client);      // 处理读事件的具体逻辑
    void OnWrite_(HttpConn* client);     // 处理写事件的具体逻辑
    void OnProcess(HttpConn* client);    // 处理HTTP请求

    static const int MAX_FD = 65536;

    static int SetFdNonblock(int fd);

    int port_;                    // 服务器监听端口
    bool openLinger_;             // 是否启用Linger选项
    int timeoutMS_;               // 连接超时时间（毫秒）
    bool isClose_;                // 服务器是否关闭
    int listenFd_;                // 监听套接字文件描述符
    char* srcDir_;                // 静态资源目录路径
    
    uint32_t listenEvent_;        // 监听套接字的事件类型
    uint32_t connEvent_;          // 连接套接字的事件类型
   
    std::unique_ptr<HeapTimer> timer_;           // 定时器（管理连接超时）
    std::unique_ptr<ThreadPool> threadpool_;     // 线程池（处理HTTP请求）
    std::unique_ptr<Epoller> epoller_;           // epoll事件监听器
    std::unordered_map<int, HttpConn> users_;    // 客户端连接映射表
};


#endif //WEBSERVER_H