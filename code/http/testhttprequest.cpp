
#include "../http/httprequest.h"
#include <iostream>

void TestHttpRequestParse() {
    // 构造一个HTTP POST请求（x-www-form-urlencoded）
    std::string raw_request = 
        "POST /login HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 29\r\n"
        "\r\n"
        "username=root&password=1";

    Buffer buffer;
    buffer.Append(raw_request.c_str(), raw_request.size());

    HttpRequest request;
    bool success = request.parse(buffer);
    
    if (success) {
        std::cout << "✅ 请求解析成功！" << std::endl;
        std::cout << "方法: " << request.method() << std::endl;
        std::cout << "路径: " << request.path() << std::endl;
        std::cout << "版本: " << request.version() << std::endl;
        std::cout << "是否KeepAlive: " << (request.IsKeepAlive() ? "是" : "否") << std::endl;

        std::string username = request.GetPost("username");
        std::string password = request.GetPost("password");

        std::cout << "username: " << username << std::endl;
        std::cout << "password: " << password << std::endl;

        std::cout << "最终页面跳转路径: " << request.path() << std::endl;
    } else {
        std::cerr << "❌ 请求解析失败！" << std::endl;
    }
}

int main() {
    LOG_INFO("./log", 0, 8000, 0); // 初始化日志系统（如有）
    SqlConnPool::Instance()->Init("localhost", 3306, "nieqishuai", "1", "tinyweb", 10); // 初始化连接池

    TestHttpRequestParse();

    // SqlConnPool::Instance()->ClosePool(); // 释放资源
    return 0;
}
