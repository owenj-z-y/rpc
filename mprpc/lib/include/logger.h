#pragma once
#include "lockqueue.h"
#include <string>

// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
//##__VA_ARGS__ 是 GCC 扩展，表示将可变参数列表传递给 snprintf 函数。如果没有传递任何可变参数，它会消除前面的逗号，防止语法错误。
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \ 
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

// 定义日志级别
enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供的日志系统
class Logger
{
public:
    // 获取日志的单例
    //Logger& 表示 GetInstance 方法返回的是 Logger 类的引用。
    //由于 Logger 是单例模式，GetInstance 返回的是该类的唯一实例的引用。这就确保了无论你在程序中多少次调用 GetInstance，你始终得到同一个 Logger 实例。
    static Logger& GetInstance();//静态方法可以在没有创建对象的情况下直接调用，因此它允许你通过 Logger::GetInstance() 来访问 Logger 的唯一实例，而不需要先实例化一个 Logger 对象。
    // 设置日志级别 
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loglevel; // 记录日志级别
    LockQueue<std::string>  m_lckQue; // 日志缓冲队列

    Logger();//Logger 类的 默认构造函数
    Logger(const Logger&) = delete;//禁止拷贝构造函数
    Logger(Logger&&) = delete;// Logger&& 表示 Logger 类型的 右值引用。
                             //它通常出现在 移动构造函数 或 移动赋值运算符 中，用于接管临时 Logger 对象的资源，而不是进行深拷贝。禁止转移构造函数
};
/*
特性	          Logger&（左值引用）	          Logger&&（右值引用）
绑定对象类型	只能绑定到 左值（如命名对象）	只能绑定到 右值（如临时对象）
资源管理	  对原对象进行引用，不会转移资源	    可通过移动语义转移资源
用途	       访问已有对象，可以修改对象内容	    用于移动构造函数、移动赋值运算符
是否会发生拷贝	通常不会触发拷贝，只是引用	     可以避免拷贝，使用移动构造
*/