#include "config.h"
#include <iostream>
#include <optional>
using namespace std;
using namespace bre;
// 测试函数
void testConfig() {
    std::cout << "name: " << Config::getInstance().Get("name") << std::endl;
    std::cout << "passwd: " << Config::getInstance().Get("passwd") << std::endl;
    std::string missing = Config::getInstance().Get("missing_key");
    if (missing.empty()) {
        std::cout << "missing_key not found." << std::endl;
    } else {
        std::cout << "missing_key: " << missing << std::endl;
    }
}


int main() {
    testConfig();
    return 0;
}
