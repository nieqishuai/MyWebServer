#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include <mysql/mysql.h>  //mysql

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"

class HttpRequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,    ///< 正在解析请求行
        HEADERS,         ///< 正在解析请求头
        BODY,            ///< 正在解析请求体
        FINISH,          ///< 解析完成       
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,          ///< 无请求
        GET_REQUEST,             ///< GET请求
        BAD_REQUEST,             ///< 错误请求
        NO_RESOURSE,             ///< 资源不存在
        FORBIDDENT_REQUEST,      ///< 禁止请求
        FILE_REQUEST,            ///< 文件请求
        INTERNAL_ERROR,          ///< 内部错误
        CLOSED_CONNECTION,       ///< 连接关闭
    };
    
    HttpRequest() { Init(); }    ///< 构造函数，自动初始化
    ~HttpRequest() = default;    ///< 析构函数

    void Init();                 ///< 初始化所有成员变量
    bool parse(Buffer& buff);    ///< 解析HTTP请求的主方法

    std::string path() const;    ///< 获取请求路径（只读）
    std::string& path();         ///< 获取请求路径（可修改）
    std::string method() const;  ///< 获取HTTP方法
    std::string version() const; ///< 获取HTTP版本
    std::string GetPost(const std::string& key) const;  ///< 获取POST参数值（字符串键）
    std::string GetPost(const char* key) const;         ///< 获取POST参数值（C字符串键）

    bool IsKeepAlive() const;    ///< 检查是否为长连接

    /* 
    todo 
    void HttpConn::ParseFormData() {}
    void HttpConn::ParseJson() {}
    */

private:
    bool ParseRequestLine_(const std::string& line);    ///< 解析HTTP请求行
    void ParseHeader_(const std::string& line);         ///< 解析HTTP请求头
    void ParseBody_(const std::string& line);           ///< 解析HTTP请求体

    void ParsePath_();           ///< 处理请求路径，添加.html后缀
    void ParsePost_();           ///< 处理POST请求数据
    void ParseFromUrlencoded_(); ///< 解析URL编码的表单数据

    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);  ///< 用户验证方法

    PARSE_STATE state_;                                    ///< 当前解析状态
    std::string method_, path_, version_, body_;          ///< HTTP请求的基本信息
    std::unordered_map<std::string, std::string> header_; ///< 请求头键值对
    std::unordered_map<std::string, std::string> post_;   ///< POST请求参数键值对

    static const std::unordered_set<std::string> DEFAULT_HTML;        ///< 默认HTML页面路径集合
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG; ///< 特殊页面标签映射
    static int ConverHex(char ch);                                     ///< 十六进制字符转整数
};


#endif //HTTP_REQUEST_H