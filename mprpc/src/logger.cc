#include "logger.h"
#include <time.h>
#include <iostream>

// 获取日志的单例,单例模式的实现
Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()//构造函数启动了一个新的 后台线程，该线程不断地从 m_lckQue 队列中取出日志消息，并将其写入当天的日志文件。
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for (;;)
        {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件当中 a+
            time_t now = time(nullptr);//获取当前时间
            tm *nowtm = localtime(&now);//将时间转换为本地时间

            //根据当前日期生成日志文件名
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);//日志文件名是按 当前日期 动态生成的

            //打开日志文件，追加模式
            FILE *pf = fopen(file_name, "a+");//"a+" 是文件打开模式，它指定了如何打开文件。a: 追加模式（Append），这意味着所有写入文件的操作将会追加到文件的末尾。如果文件不存在，会创建一个新的文件。
                                            //+: 读写模式，表示文件既可以读也可以写。
            if (pf == nullptr)
            {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);//EXIT_FAILURE 宏：表示程序由于某种错误或异常情况终止，因此是一个指示程序异常退出的状态码。
            }

            //从锁队列中取出一条日志消息
            std::string msg = m_lckQue.Pop();//模版T pop()

            // 获取当前时间并格式化为字符串，作为日志的时间戳
            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", //sprintf 是 C 标准库中的一个函数，用于将格式化的字符串输出到指定的字符数组中
                    nowtm->tm_hour, 
                    nowtm->tm_min, 
                    nowtm->tm_sec,
                    (m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, time_buf);// 将时间戳添加到日志消息前
            msg.append("\n"); // 在日志消息后加上换行符
            
            // 将格式化后的日志消息写入文件
            fputs(msg.c_str(), pf);
            fclose(pf);// 写入完毕后关闭文件
        }
    });
    // 设置分离线程，守护线程,意味着该线程在主线程退出后会继续执行
    writeLogTask.detach();
}

// 设置日志级别 
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

// 写日志， 把日志信息写入lockqueue缓冲区当中,该方法将传入的日志消息 msg 添加到 锁队列（m_lckQue）中，以便后台线程能够处理并写入日志文件。
void Logger::Log(std::string msg)
{
    m_lckQue.Push(msg);
}