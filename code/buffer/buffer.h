#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
class Buffer {
public:
    // 构造函数，初始化缓冲区大小
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;       //返回可写入的字节数
    size_t ReadableBytes() const ;      //返回可读的字节数
    size_t PrependableBytes() const;    //返回可追加的字节数

    const char* Peek() const;           //返回缓冲区当前读取位置的指针
    void EnsureWriteable(size_t len);   //确保缓冲区有足够的空间写入len字节
    void HasWritten(size_t len);        //标记len字节已被写入

    void Retrieve(size_t len);          //读取 len 个字节并移动读取位置
    void RetrieveUntil(const char* end);//读取直到遇到 end 指针所指的位置

    void RetrieveAll() ;                //清空缓冲区
    std::string RetrieveAllToStr();     //读取整个缓冲区并返回一个字符串

    const char* BeginWriteConst() const;//返回缓冲区当前写入位置的指针
    char* BeginWrite();                 //返回缓冲区当前写入位置的指针

    void Append(const std::string& str);        //追加一个字符串到缓冲区
    void Append(const char* str, size_t len);   //追加一个字符数组到缓冲区
    void Append(const void* data, size_t len);  //追加一个二进制数据块到缓冲区
    void Append(const Buffer& buff);            //追加另一个缓冲区的内容到当前缓冲区

    ssize_t ReadFd(int fd, int* Errno);     // 从文件描述符 fd 读取数据到缓冲区
    ssize_t WriteFd(int fd, int* Errno);    // 从文件描述符 fd 写入数据到缓冲区

private:
    char* BeginPtr_();              //返回缓冲区的起始指针
    const char* BeginPtr_() const;  //返回缓冲区的起始指针（常量版本）
    void MakeSpace_(size_t len);    //确保缓冲区有足够的空间写入len字节，如果没有则扩展缓冲区

    std::vector<char> buffer_;           //缓冲区数据存储
    std::atomic<std::size_t> readPos_;   //读取位置
    std::atomic<std::size_t> writePos_;  //写入位置
};

#endif //BUFFER_H