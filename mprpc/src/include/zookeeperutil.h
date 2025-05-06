#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

// 封装的zk客户端类
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    // zkclient启动连接zkserver
    void Start();
    // 在zkserver上根据指定的path创建znode节点
    void Create(const char *path, const char *data, int datalen, int state=0);
    // 根据参数指定的znode节点路径，或者znode节点的值
    std::string GetData(const char *path);
private:
    // zk的客户端句柄
    zhandle_t *m_zhandle;//m_zhandle 是 ZooKeeper 客户端的句柄，用来管理与 ZooKeeper 服务器的连接。在使用 ZooKeeper C API 时，这个句柄是与服务器交互的核心，所有的操作（如创建节点、获取节点数据等）都依赖这个句柄。
};