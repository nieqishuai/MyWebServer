#include "buffer.h"
#include <iostream>

int main() {
    Buffer buffer;

    // 测试 Append 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");

    // 测试 RetrieveAllToStr 函数
    std::string str = buffer.RetrieveAllToStr();
    std::cout << "RetrieveAllToStr: " << str << std::endl;

    // 测试 RetrieveAll 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    buffer.RetrieveAll();
    std::cout << "RetrieveAll: " << buffer.RetrieveAllToStr() << std::endl;

    // 测试 ReadableBytes 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    std::cout << "ReadableBytes: " << buffer.ReadableBytes() << std::endl;

    // 测试 WritableBytes 函数
    std::cout << "WritableBytes: " << buffer.WritableBytes() << std::endl;

    // 测试 PrependableBytes 函数
    std::cout << "PrependableBytes: " << buffer.PrependableBytes() << std::endl;

    // 测试 Retrieve 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    buffer.Retrieve(5);
    std::cout << "Retrieve: " << buffer.RetrieveAllToStr() << std::endl;

    // 测试 Peek 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    std::cout << "Peek: " << buffer.Peek() << std::endl;

    // 测试 BeginWrite 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    std::cout << "BeginWrite: " << buffer.BeginWrite() << std::endl;

    // 测试 BeginWriteConst 函数
    buffer.Append("Hello, ");
    buffer.Append("World!");
    std::cout << "BeginWriteConst: " << buffer.BeginWriteConst() << std::endl;

    return 0;
}