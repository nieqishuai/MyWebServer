#ifndef HTTP_CONN_H
#define HTTP_CONN_H

// 包含必要的系统头文件
#include <sys/types.h>      // 基本系统数据类型
#include <sys/uio.h>        // readv/writev - 向量化I/O操作
#include <arpa/inet.h>      // sockaddr_in - 网络地址结构
#include <stdlib.h>         // atoi() - 字符串转整数
#include <errno.h>          // 错误码定义

// 包含项目相关的头文件
#include "../log/log.h"         // 日志系统
#include "../pool/sqlconnRAII.h" // 数据库连接RAII包装器
#include "../buffer/buffer.h"    // 自定义缓冲区类
#include "httprequest.h"         // HTTP请求处理类
#include "httpresponse.h"        // HTTP响应处理类

class HttpConn {
public:
    HttpConn();   // 构造函数
    ~HttpConn();  // 析构函数

    // 初始化HTTP连接
    void init(int sockFd, const sockaddr_in& addr);

    // 从套接字读取数据
    ssize_t read(int* saveErrno);

    // 向套接字写入数据
    ssize_t write(int* saveErrno);

    // 关闭连接
    void Close();

    // 获取套接字文件描述符
    int GetFd() const;

    // 获取客户端端口号
    int GetPort() const;

    // 获取客户端IP地址
    const char* GetIP() const;
    
    // 获取客户端地址结构
    sockaddr_in GetAddr() const;
    
    // 处理HTTP请求
    bool process();

    // 获取待写入的字节数
    int ToWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    // 检查是否为Keep-Alive连接
    bool IsKeepAlive() const {
        return request_.IsKeepAlive();
    }

    // 静态成员变量
    static bool isET;                    // 是否为边缘触发模式
    static const char* srcDir;           // 服务器根目录
    static std::atomic<int> userCount;   // 当前连接用户数（原子操作）
    
private:
   
    int fd_;                    // 套接字文件描述符
    struct sockaddr_in addr_;   // 客户端地址结构

    bool isClose_;              // 连接是否已关闭
    
    int iovCnt_;                // iovec数组中的元素个数
    struct iovec iov_[2];       // 向量化I/O结构数组，用于writev操作
    
    Buffer readBuff_;           // 读缓冲区，存储从客户端接收的数据
    Buffer writeBuff_;          // 写缓冲区，存储要发送给客户端的数据

    HttpRequest request_;       // HTTP请求处理对象
    HttpResponse response_;     // HTTP响应处理对象
};


#endif //HTTP_CONN_H