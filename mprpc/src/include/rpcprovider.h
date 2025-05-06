#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// 框架提供的专门发布rpc服务的网络对象类
class RpcProvider
{
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);
    //接收一个 google::protobuf::Service 类型的指针，并将其注册到 RpcProvider 中，以便框架可以识别和调用该服务。每个 RPC 服务对象包含多个 RPC 方法，框架会利用这些方法描述符在网络请求中进行调用。

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    // 组合EventLoop
    muduo::net::EventLoop m_eventLoop;//用于处理网络事件（如客户端连接、消息收发等）。muduo::net::EventLoop 是事件驱动的核心，可以启动并处理 I/O 操作。

    // service服务类型信息
    //结构体存储了服务对象以及该服务的方法描述符。每个服务对象（m_service）可以有多个方法，m_methodMap 保存了方法名称与对应的 MethodDescriptor 的映射。
    struct ServiceInfo
    {
        google::protobuf::Service *m_service; // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; // 保存服务方法
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    // 新的socket连接回调,该回调函数处理新的 TCP 连接。每当一个新的客户端连接到服务器时，该函数会被触发。这里可以初始化连接、身份验证、连接设置等。
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    // 已建立连接用户的读写事件回调,该回调函数用于处理客户端发送的消息。在这个方法中，可以解析客户端请求的 RPC 方法，查找相应的服务和方法，并调用它们。
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    // Closure的回调操作，用于序列化rpc的响应和网络发送,该方法用于发送 RPC 响应给客户端。通过 google::protobuf::Message 对象序列化响应数据并发送回客户端。
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};