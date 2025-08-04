# MyWebServer - 高性能C++ Web服务器

一个基于C++11开发的高性能Web服务器，采用Reactor模式，支持HTTP/1.1协议，具备连接池、线程池、定时器等功能。

## 🚀 功能特性

- **高性能架构**: 基于Epoll的Reactor模式，支持高并发连接
- **HTTP/1.1协议**: 完整的HTTP请求解析和响应生成
- **连接池管理**: MySQL数据库连接池，提高数据库访问效率
- **线程池**: 异步处理HTTP请求，提高并发性能
- **定时器**: 基于小根堆的定时器，支持连接超时管理
- **日志系统**: 异步日志记录，支持多级别日志输出
- **配置管理**: 灵活的配置文件支持
- **缓冲区管理**: 高效的读写缓冲区实现

## 📁 项目结构

```
MyWebServer/
├── code/                    # 源代码目录
│   ├── main.cpp            # 主程序入口
│   ├── buffer/             # 缓冲区模块
│   │   ├── buffer.h        # 缓冲区头文件
│   │   ├── buffer.cpp      # 缓冲区实现
│   │   └── testbuffer.cpp  # 缓冲区测试
│   ├── config/             # 配置管理模块
│   ├── http/               # HTTP协议模块
│   │   ├── httpconn.h      # HTTP连接管理
│   │   ├── httprequest.h   # HTTP请求解析
│   │   ├── httpresponse.h  # HTTP响应生成
│   │   └── *.cpp           # 对应实现文件
│   ├── log/                # 日志系统模块
│   ├── pool/               # 连接池模块
│   │   ├── threadpool.h   # 线程池
│   │   └── sqlconnpool.cpp # MySQL连接池
│   ├── server/             # 服务器核心模块
│   │   ├── webserver.h     # Web服务器类
│   │   └── epoller.h       # Epoll封装
│   └── timer/              # 定时器模块
│       └── heaptimer.cpp   # 小根堆定时器
├── bin/                    # 编译输出目录
├── log/                    # 日志文件目录
├── resources/              # 静态资源目录
├── webbench-1.5/          # 压力测试工具
├── config.txt             # 配置文件
├── Makefile               # 编译脚本
└── README.md              # 项目说明
```

## 🛠️ 环境要求

- **操作系统**: Linux (推荐Ubuntu 18.04+)
- **编译器**: GCC 4.8+ 或 Clang 3.3+
- **C++标准**: C++11
- **依赖库**: 
  - MySQL Client Library (`libmysqlclient-dev`)
  - pthread


## 🔨 编译安装

### 快速编译
```bash
# 编译发布版本
make

# 编译调试版本
make debug

# 清理编译文件
make clean
```

### 详细编译选项
```bash
# 编译发布版本（优化）
make release

# 编译调试版本（包含调试信息）
make debug

# 运行服务器
make run

# 运行调试版本
make run-debug

# 安装到系统
make install

# 卸载
make uninstall
```

## ⚙️ 配置说明

编辑 `config.txt` 文件来配置服务器参数：

```ini
# 服务器配置
port:1316              # 监听端口
mode:3                 # 运行模式
timeout:60000          # 连接超时时间(ms)
optLinger:false        # 是否启用优雅关闭

# 数据库配置
sqlPort:3306           # MySQL端口
sqlUser:nieqishuai     # MySQL用户名
sqlPwd:1               # MySQL密码
dbName:tinyweb         # 数据库名

# 连接池和线程池配置
connPoolNum:12         # 数据库连接池大小
threadNum:12           # 线程池大小

# 日志配置
openLog:false          # 是否启用日志
logLevel:1             # 日志级别(0-4)
logQueSize:1024        # 日志队列大小
```

## 🚀 运行服务器

### 基本运行
```bash
# 编译并运行
make run

# 或者直接运行
./bin/webserver
```

### 后台运行
```bash
# 使用nohup后台运行
nohup ./bin/webserver > server.log 2>&1 &

# 查看进程
ps aux | grep webserver
```

## 🧪 测试

### 单元测试
```bash
# 测试缓冲区
g++ -std=c++11 -o testbuffer testbuffer.cpp buffer.cpp
./testbuffer

# 测试配置管理
g++ -std=c++11 -o testconfig testconfig.cpp
./testconfig

# 测试日志系统
g++ -std=c++11 -o testlog testlog.cpp log.cpp ../buffer/buffer.cpp
./testlog

# 测试数据库连接池
g++ -std=c++11 testsqlconnpool.cpp sqlconnpool.cpp ../log/log.cpp ../buffer/buffer.cpp -o testsqlconnpool -lmysqlclient -lpthread
./testsqlconnpool

# 测试定时器
g++ -std=c++11 testheaptimer.cpp heaptimer.cpp ../log/log.cpp ../buffer/buffer.cpp -o testheaptimer
./testheaptimer

# 测试线程池
g++ -std=c++11 testthreadpool.cpp -o testthreadpool
./testthreadpool

# 测试HTTP请求解析
g++ -std=c++11 testhttprequest.cpp httprequest.cpp ../buffer/buffer.cpp ../log/log.cpp ../pool/sqlconnpool.cpp -o testhttprequest -lmysqlclient
./testhttprequest

# 测试HTTP响应生成
g++ -std=c++11 testhttpresponse.cpp httpresponse.cpp ../buffer/buffer.cpp ../log/log.cpp -o testhttpresponse
./testhttpresponse

# 测试HTTP连接
g++ -std=c++11 testhttpconn.cpp httpconn.cpp httpresponse.cpp httprequest.cpp ../buffer/buffer.cpp ../log/log.cpp ../pool/sqlconnpool.cpp -o testhttpconn -lmysqlclient
./testhttpconn

# 测试Epoll
g++ -std=c++11 testepoller.cpp -o testepoller epoller.cpp
./testepoller
```

### 压力测试
使用webbench进行压力测试：

```bash
# 100并发，持续10秒
./webbench-1.5/webbench -c 100 -t 10 http://localhost:1316/

# 1000并发，持续10秒
./webbench-1.5/webbench -c 1000 -t 10 http://localhost:1316/

# 5000并发，持续10秒
./webbench-1.5/webbench -c 5000 -t 10 http://localhost:1316/

# 10000并发，持续10秒
./webbench-1.5/webbench -c 10000 -t 10 http://localhost:1316/
```

### 功能测试
```bash
# 使用curl测试HTTP请求
curl -v http://localhost:1316/

# 测试POST请求
curl -X POST -d "name=test&value=123" http://localhost:1316/api/test

# 测试文件上传
curl -F "file=@test.txt" http://localhost:1316/upload
```

## 📚 学习顺序

建议按以下顺序学习代码：

1. **buffer** - 缓冲区管理，理解读写缓冲的实现
2. **config** - 配置管理，学习配置文件的读取和解析
3. **log** - 日志系统，了解异步日志的实现
4. **sqlpool、timer、threadpool** - 连接池、定时器、线程池
5. **http** - HTTP协议解析和响应生成
6. **server** - 服务器核心，整合所有模块

## 🔧 开发说明

### 编译单个模块测试
```bash
# 编译主程序
g++ -std=c++11 code/main.cpp code/buffer/buffer.cpp code/http/httpconn.cpp code/http/httprequest.cpp code/http/httpresponse.cpp code/log/log.cpp code/pool/sqlconnpool.cpp code/server/epoller.cpp code/server/webserver.cpp code/timer/heaptimer.cpp -o bin/webserver -lmysqlclient
```

### 调试模式
```bash
# 启用调试模式编译
make debug

# 使用gdb调试
gdb bin/webserver_debug
```

## 📊 性能指标

- **并发连接**: 支持10,000+并发连接
- **请求处理**: 每秒可处理数万次HTTP请求
- **内存使用**: 低内存占用，每个连接约1KB内存
- **CPU使用**: 高效的事件驱动模型，CPU使用率低


## 📞 联系方式

- 项目维护者: nieqishuai
- 邮箱: [nieqishuai@163.com]
- 项目地址: [https://github.com/nieqishuai/MyWebServer]

## 🙏 致谢

感谢以下开源项目的启发：
- [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
- [WebServer](https://github.com/markparticle/WebServer)
- [WebServer](https://github.com/breeze1396/WebServer)
- [WebBench](http://home.tiscali.cz/~cz210552/webbench.html)


---

⭐ 如果这个项目对您有帮助，请给个星标支持一下！

