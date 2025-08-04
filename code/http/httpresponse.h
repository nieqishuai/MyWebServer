#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

// 包含必要的头文件
#include <unordered_map>  // 用于存储映射表
#include <fcntl.h>       // open - 文件操作
#include <unistd.h>      // close - 文件关闭
#include <sys/stat.h>    // stat - 获取文件状态
#include <sys/mman.h>    // mmap, munmap - 内存映射

#include "../buffer/buffer.h"  // 自定义缓冲区类
#include "../log/log.h"        // 日志系统

class HttpResponse {
public:
    HttpResponse();   // 构造函数
    ~HttpResponse();  // 析构函数

    // 初始化HTTP响应对象
    void Init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);
    // 构建完整的HTTP响应
    void MakeResponse(Buffer& buff);
    // 解除内存映射
    void UnmapFile();
    // 获取内存映射文件的指针
    char* File();
    // 获取文件长度
    size_t FileLen() const;
    // 添加错误内容到缓冲区
    void ErrorContent(Buffer& buff, std::string message);
    // 获取HTTP状态码
    int Code() const { return code_; }

private:
    // 添加HTTP状态行到缓冲区
    void AddStateLine_(Buffer &buff);
    // 添加HTTP头部到缓冲区
    void AddHeader_(Buffer &buff);
    // 添加响应内容到缓冲区
    void AddContent_(Buffer &buff);

    // 生成错误页面的HTML内容
    void ErrorHtml_();
    // 根据文件后缀获取MIME类型
    std::string GetFileType_();

    int code_;              // HTTP状态码
    bool isKeepAlive_;      // 是否保持连接

    std::string path_;      // 请求的文件路径
    std::string srcDir_;    // 服务器根目录
    
    char* mmFile_;          // 内存映射文件的指针
    struct stat mmFileStat_; // 文件状态信息

    // 静态映射表：文件后缀到MIME类型的映射
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    // 静态映射表：状态码到状态描述的映射
    static const std::unordered_map<int, std::string> CODE_STATUS;
    // 静态映射表：状态码到错误页面路径的映射
    static const std::unordered_map<int, std::string> CODE_PATH;
};


#endif //HTTP_RESPONSE_H