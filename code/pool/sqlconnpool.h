#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h"

class SqlConnPool {
public:
    static SqlConnPool *Instance();     //单例模式核心，返回连接池唯一实例（确保全局只有一个连接池）

    MYSQL *GetConn();                   //从连接池获取一个数据库连接
    void FreeConn(MYSQL * conn);        //将用完的连接归还给连接池（而非关闭）
    int GetFreeConnCount();             //获取当前空闲连接的数量

    void Init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int connSize);    //初始化连接池：指定数据库地址、端口、账号等信息，并创建初始连接
     void ClosePool();                   //关闭连接池：释放所有连接资源，销毁队列

private:
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;       // 最大连接数（连接池容量上限）
    int useCount_;       // 当前正在使用的连接数
    int freeCount_;      // 当前空闲的连接数

    std::queue<MYSQL *> connQue_;  // 存储数据库连接的队列（核心容器）
    std::mutex mtx_;               // 互斥锁：保证对队列的操作（获取/归还连接）线程安全
    sem_t semId_;                  // 信号量：用于控制空闲连接的数量（获取连接前检查是否有空闲）
};


#endif // SQLCONNPOOL_H