#include <unistd.h>
#include <iostream>
#include "server/webserver.h"
#include "config/config.h"

int main() {
    try {
        /* 守护进程 后台运行 */
        //daemon(1, 0); 

        // 获取配置实例
        auto& config = bre::Config::getInstance();
        
        // 从配置文件读取参数，如果读取失败则使用默认值
        int port = 1316;
        int mode = 3;
        int timeout = 60000;
        bool optLinger = false;
        
        int sqlPort = 3306;
        std::string sqlUser = "nieqishuai";
        std::string sqlPwd = "1";
        std::string dbName = "tinyweb";
        
        int connPoolNum = 12;
        int threadNum = 6;
        bool openLog = true;
        int logLevel = 1;
        int logQueSize = 1024;

        // 尝试从配置文件读取值
        std::string portStr = config.Get("port");
        if (!portStr.empty()) {
            port = std::stoi(portStr);
        }
        
        std::string modeStr = config.Get("mode");
        if (!modeStr.empty()) {
            mode = std::stoi(modeStr);
        }
        
        std::string timeoutStr = config.Get("timeout");
        if (!timeoutStr.empty()) {
            timeout = std::stoi(timeoutStr);
        }
        
        std::string optLingerStr = config.Get("optLinger");
        if (!optLingerStr.empty()) {
            optLinger = (optLingerStr == "true" || optLingerStr == "1");
        }
        
        std::string sqlPortStr = config.Get("sqlPort");
        if (!sqlPortStr.empty()) {
            sqlPort = std::stoi(sqlPortStr);
        }
        
        std::string sqlUserStr = config.Get("sqlUser");
        if (!sqlUserStr.empty()) {
            sqlUser = sqlUserStr;
        }
        
        std::string sqlPwdStr = config.Get("sqlPwd");
        if (!sqlPwdStr.empty()) {
            sqlPwd = sqlPwdStr;
        }
        
        std::string dbNameStr = config.Get("dbName");
        if (!dbNameStr.empty()) {
            dbName = dbNameStr;
        }
        
        std::string connPoolNumStr = config.Get("connPoolNum");
        if (!connPoolNumStr.empty()) {
            connPoolNum = std::stoi(connPoolNumStr);
        }
        
        std::string threadNumStr = config.Get("threadNum");
        if (!threadNumStr.empty()) {
            threadNum = std::stoi(threadNumStr);
        }
        
        std::string openLogStr = config.Get("openLog");
        if (!openLogStr.empty()) {
            openLog = (openLogStr == "true" || openLogStr == "1");
        }
        
        std::string logLevelStr = config.Get("logLevel");
        if (!logLevelStr.empty()) {
            logLevel = std::stoi(logLevelStr);
        }
        
        std::string logQueSizeStr = config.Get("logQueSize");
        if (!logQueSizeStr.empty()) {
            logQueSize = std::stoi(logQueSizeStr);
        }

        // 输出配置信息
        std::cout << "=== Web服务器配置 ===" << std::endl;
        std::cout << "端口: " << port << std::endl;
        std::cout << "模式: " << mode << std::endl;
        std::cout << "超时: " << timeout << "ms" << std::endl;
        std::cout << "优雅退出: " << (optLinger ? "是" : "否") << std::endl;
        std::cout << "数据库端口: " << sqlPort << std::endl;
        std::cout << "数据库用户: " << sqlUser << std::endl;
        std::cout << "数据库名: " << dbName << std::endl;
        std::cout << "连接池数量: " << connPoolNum << std::endl;
        std::cout << "线程池数量: " << threadNum << std::endl;
        std::cout << "日志开关: " << (openLog ? "开启" : "关闭") << std::endl;
        std::cout << "日志等级: " << logLevel << std::endl;
        std::cout << "日志队列容量: " << logQueSize << std::endl;
        std::cout << "====================" << std::endl;

        WebServer server(
            port, mode, timeout, optLinger,              /* 端口 ET模式 timeoutMs 优雅退出  */
            sqlPort, sqlUser.c_str(), sqlPwd.c_str(), dbName.c_str(),     /* Mysql配置 */
            connPoolNum, threadNum, openLog, logLevel,                /* 连接池数量 线程池数量 日志开关 日志等级  */
            logQueSize);             /* 日志异步队列容量 */
        server.Start();
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 
