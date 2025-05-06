#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

// 全局的watcher观察器   zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type,
                   int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)  // 回调的消息类型是和会话相关的消息类型
	{
		if (state == ZOO_CONNECTED_STATE)  // zkclient和zkserver连接成功
		{
			// 从ZooKeeper句柄获取上下文，这里存储的是信号量
			//sem_t 是 POSIX 信号量类型，通常用于进程间或线程间同步。在这里，它用于同步等待 ZooKeeper 连接的建立。
			//zoo_get_context 返回的是 void* 类型的指针，表示通用指针。为了能够正确地使用它作为信号量，需要将其转换为 sem_t* 类型，便于后续的信号量操作。
			sem_t *sem = (sem_t*)zoo_get_context(zh);
			// 释放信号量，表示连接成功
			//将信号量 sem 的值增加 1。如果有线程或进程在等待这个信号量（通过 sem_wait 或 sem_trywait 等方式），
			//sem_post 会让其中一个等待的线程或进程继续执行。信号量的计数越大，表示有更多的资源可供使用，等待的线程也就越少。
            sem_post(sem);
		}
	}
}

// 构造函数，初始化时，m_zhandle指针为空
ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

// 析构函数，关闭ZooKeeper句柄，释放资源
ZkClient::~ZkClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle); // 关闭句柄，释放资源  MySQL_Conn
    }
}

// 连接zkserver
void ZkClient::Start()
{
	// 获取配置文件中ZooKeeper服务器的IP地址和端口
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;// 构建连接字符串
    
	/*
	zookeeper_mt：多线程版本
	zookeeper的API客户端程序提供了三个线程
	API调用线程 
	网络I/O线程  pthread_create  poll
	watcher回调线程 pthread_create
	*/
	// 使用zookeeper_init建立与ZooKeeper服务器的连接，并传入回调函数
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
	 // 如果连接失败
    if (nullptr == m_zhandle) 
    {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

	// 创建一个信号量，用于等待连接完成
    sem_t sem;
	// 初始化信号量，初始值为0
    sem_init(&sem, 0, 0);//0：信号量仅在当前进程内的线程之间共享。非 0：信号量可在多个进程之间共享，通常需要使用进程间共享内存。
	// 将信号量存储在ZooKeeper句柄的上下文中
    zoo_set_context(m_zhandle, &sem);

	// 等待连接成功的信号
    sem_wait(&sem);
    std::cout << "zookeeper_init success!" << std::endl;
}

// 创建znode节点
void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
	// 用于存储创建的路径
    char path_buffer[128];
	// 缓冲区长度
    int bufferlen = sizeof(path_buffer);
    int flag;
	// 先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
	flag = zoo_exists(m_zhandle, path, 0, nullptr);
	if (ZNONODE == flag) // 表示path的znode节点不存在
	{
		// 创建指定path的znode节点了
		flag = zoo_create(m_zhandle, path, data, datalen,
			&ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
		if (flag == ZOK)
		{
			std::cout << "znode create success... path:" << path << std::endl;
		}
		else
		{
			std::cout << "flag:" << flag << std::endl;
			std::cout << "znode create error... path:" << path << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

// 根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char *path)
{
    char buffer[64];// 缓冲区用来存储znode节点数据
	int bufferlen = sizeof(buffer);// 缓冲区长度
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);// 获取znode节点数据
	if (flag != ZOK)
	{
		std::cout << "get znode error... path:" << path << std::endl;
		return "";
	}
	else
	{
		return buffer;// 返回获取到的znode数据
	}
}