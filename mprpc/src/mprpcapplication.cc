#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;

//用于显示帮助信息，告知用户命令行格式应该是 command -i <configfile>，也就是必须提供一个配置文件。
void ShowArgsHelp()
{
    std::cout<<"format: command -i <configfile>" << std::endl;
}

//首先检查命令行参数 argc 是否小于 2，如果小于 2，就说明没有传入配置文件路径，调用 ShowArgsHelp() 提示用户并退出程序。
void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)//getopt() 是 POSIX 标准提供的一个函数，用来从命令行参数中解析选项.后面可以加一个冒号 :，表示该选项需要一个参数
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件了 rpcserver_ip=  rpcserver_port   zookeeper_ip=  zookepper_port=
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}