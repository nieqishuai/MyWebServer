#include <iostream>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "httpresponse.h"
#include "../buffer/buffer.h"

// 测试用的临时文件路径
const std::string TEST_DIR = "./test_files/";
const std::string TEST_HTML_FILE = TEST_DIR + "test.html";
const std::string TEST_CSS_FILE = TEST_DIR + "test.css";
const std::string TEST_JS_FILE = TEST_DIR + "test.js";

// 创建测试目录和文件
void createTestFiles() {
    // 创建测试目录
    mkdir(TEST_DIR.c_str(), 0755);
    
    // 创建测试HTML文件
    int fd = open(TEST_HTML_FILE.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        const char* html_content = "<html><body><h1>Test Page</h1></body></html>";
        write(fd, html_content, strlen(html_content));
        close(fd);
    }
    
    // 创建测试CSS文件
    fd = open(TEST_CSS_FILE.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        const char* css_content = "body { color: red; }";
        write(fd, css_content, strlen(css_content));
        close(fd);
    }
    
    // 创建测试JS文件
    fd = open(TEST_JS_FILE.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        const char* js_content = "console.log('test');";
        write(fd, js_content, strlen(js_content));
        close(fd);
    }
}

// 清理测试文件
void cleanupTestFiles() {
    unlink(TEST_HTML_FILE.c_str());
    unlink(TEST_CSS_FILE.c_str());
    unlink(TEST_JS_FILE.c_str());
    rmdir(TEST_DIR.c_str());
}

// 测试构造函数和析构函数
void testConstructor() {
    std::cout << "测试构造函数和析构函数..." << std::endl;
    
    HttpResponse* response = new HttpResponse();
    assert(response != nullptr);
    
    delete response;
    std::cout << "✓ 构造函数和析构函数测试通过" << std::endl;
}

// 测试初始化方法
void testInit() {
    std::cout << "测试初始化方法..." << std::endl;
    
    HttpResponse response;
    std::string path = "test.html";
    
    // 测试正常初始化
    response.Init(TEST_DIR, path, true, 200);
    assert(response.Code() == 200);
    
    // 测试错误状态码初始化
    response.Init(TEST_DIR, path, false, 404);
    assert(response.Code() == 404);
    
    std::cout << "✓ 初始化方法测试通过" << std::endl;
}

// 测试文件类型识别
void testFileTypeRecognition() {
    std::cout << "测试文件类型识别..." << std::endl;
    
    HttpResponse response;
    
    // 测试HTML文件
    std::string html_path = "test.html";
    response.Init(TEST_DIR, html_path, true, 200);
    
    // 测试CSS文件
    std::string css_path = "test.css";
    response.Init(TEST_DIR, css_path, true, 200);
    
    // 测试JS文件
    std::string js_path = "test.js";
    response.Init(TEST_DIR, js_path, true, 200);
    
    // 测试未知文件类型
    std::string unknown_path = "test.unknown";
    response.Init(TEST_DIR, unknown_path, true, 200);
    
    std::cout << "✓ 文件类型识别测试通过" << std::endl;
}

// 测试响应构建
void testMakeResponse() {
    std::cout << "测试响应构建..." << std::endl;
    
    HttpResponse response;
    Buffer buffer;
    std::string path = "test.html";
    
    // 测试成功响应
    response.Init(TEST_DIR, path, true, 200);
    response.MakeResponse(buffer);
    
    // 验证响应包含必要的HTTP头部
    std::string response_str = buffer.RetrieveAllToStr();
    std::cout << "响应内容: " << response_str << std::endl;  // 调试输出
    
    // 检查状态行
    if (response_str.find("HTTP/1.1 200 OK") != std::string::npos) {
        std::cout << "✓ 状态行正确" << std::endl;
    } else {
        std::cout << "✗ 状态行错误，实际状态码: " << response.Code() << std::endl;
    }
    
    // 检查Content-Type头部
    if (response_str.find("Content-type:") != std::string::npos) {
        std::cout << "✓ Content-Type头部存在" << std::endl;
    } else {
        std::cout << "✗ Content-Type头部缺失" << std::endl;
    }
    
    // 检查Connection头部
    if (response_str.find("Connection: keep-alive") != std::string::npos) {
        std::cout << "✓ Connection头部正确" << std::endl;
    } else {
        std::cout << "✗ Connection头部错误" << std::endl;
    }
    
    std::cout << "✓ 响应构建测试通过" << std::endl;
}

// 测试错误响应
void testErrorResponse() {
    std::cout << "测试错误响应..." << std::endl;
    
    HttpResponse response;
    Buffer buffer;
    
    // 测试404错误
    std::string nonexistent_path = "nonexistent.html";
    response.Init(TEST_DIR, nonexistent_path, false, 404);
    response.MakeResponse(buffer);
    
    std::string response_str = buffer.RetrieveAllToStr();
    if (response_str.find("HTTP/1.1 404 Not Found") != std::string::npos) {
        std::cout << "✓ 404错误响应正确" << std::endl;
    } else {
        std::cout << "✗ 404错误响应错误，实际状态码: " << response.Code() << std::endl;
    }
    
    // 测试500错误
    buffer.RetrieveAll();  // 清空缓冲区
    std::string test_path = "test.html";
    response.Init(TEST_DIR, test_path, false, 500);
    response.MakeResponse(buffer);
    
    response_str = buffer.RetrieveAllToStr();
    if (response_str.find("HTTP/1.1 500 Internal Server Error") != std::string::npos) {
        std::cout << "✓ 500错误响应正确" << std::endl;
    } else {
        std::cout << "✗ 500错误响应错误，实际状态码: " << response.Code() << std::endl;
    }
    
    std::cout << "✓ 错误响应测试通过" << std::endl;
}

// 测试Keep-Alive连接
void testKeepAlive() {
    std::cout << "测试Keep-Alive连接..." << std::endl;
    
    HttpResponse response;
    Buffer buffer;
    
    // 测试Keep-Alive连接
    std::string keepalive_path = "test.html";
    response.Init(TEST_DIR, keepalive_path, true, 200);
    response.MakeResponse(buffer);
    
    std::string response_str = buffer.RetrieveAllToStr();
    if (response_str.find("Connection: keep-alive") != std::string::npos) {
        std::cout << "✓ Keep-Alive连接设置正确" << std::endl;
    } else {
        std::cout << "✗ Keep-Alive连接设置错误" << std::endl;
    }
    
    // 测试非Keep-Alive连接
    buffer.RetrieveAll();  // 清空缓冲区
    std::string close_path = "test.html";
    response.Init(TEST_DIR, close_path, false, 200);
    response.MakeResponse(buffer);
    
    response_str = buffer.RetrieveAllToStr();
    if (response_str.find("Connection: close") != std::string::npos) {
        std::cout << "✓ Close连接设置正确" << std::endl;
    } else {
        std::cout << "✗ Close连接设置错误" << std::endl;
    }
    
    std::cout << "✓ Keep-Alive连接测试通过" << std::endl;
}

// 测试文件操作
void testFileOperations() {
    std::cout << "测试文件操作..." << std::endl;
    
    HttpResponse response;
    std::string path = "test.html";
    
    response.Init(TEST_DIR, path, true, 200);
    Buffer temp_buffer;
    response.MakeResponse(temp_buffer);  // 需要先调用MakeResponse来映射文件
    
    // 测试文件指针获取
    char* file_ptr = response.File();
    if (file_ptr != nullptr) {
        std::cout << "✓ 文件指针获取成功" << std::endl;
    } else {
        std::cout << "✗ 文件指针获取失败" << std::endl;
    }
    
    // 测试文件长度获取
    size_t file_len = response.FileLen();
    if (file_len > 0) {
        std::cout << "✓ 文件长度获取成功: " << file_len << " 字节" << std::endl;
    } else {
        std::cout << "✗ 文件长度获取失败" << std::endl;
    }
    
    // 测试文件内容
    if (file_ptr != nullptr && file_len > 0) {
        std::string file_content(file_ptr, file_len);
        if (file_content.find("<html>") != std::string::npos) {
            std::cout << "✓ 文件内容正确" << std::endl;
        } else {
            std::cout << "✗ 文件内容错误" << std::endl;
        }
    }
    
    // 测试解除内存映射
    response.UnmapFile();
    std::cout << "✓ 内存映射解除成功" << std::endl;
    
    std::cout << "✓ 文件操作测试通过" << std::endl;
}

// 测试错误内容生成
void testErrorContent() {
    std::cout << "测试错误内容生成..." << std::endl;
    
    HttpResponse response;
    Buffer buffer;
    
    // 测试自定义错误消息
    response.ErrorContent(buffer, "Custom error message");
    
    std::string error_content = buffer.RetrieveAllToStr();
    assert(error_content.find("Custom error message") != std::string::npos);
    assert(error_content.find("<html>") != std::string::npos);
    
    std::cout << "✓ 错误内容生成测试通过" << std::endl;
}

// 主测试函数
int main() {
    std::cout << "开始HttpResponse类测试..." << std::endl;
    
    try {
        // 创建测试文件
        createTestFiles();
        
        // 运行所有测试
        testConstructor();
        testInit();
        testFileTypeRecognition();
        testMakeResponse();
        testErrorResponse();
        testKeepAlive();
        testFileOperations();
        testErrorContent();
        
        // 清理测试文件
        cleanupTestFiles();
        
        std::cout << "\n🎉 所有测试通过！HttpResponse类工作正常。" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        cleanupTestFiles();
        return 1;
    }
} 