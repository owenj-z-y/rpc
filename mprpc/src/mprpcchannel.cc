#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "zookeeperutil.h"

/*
header_size + service_name method_name args_size + args
*/
// 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据数据序列化和网络发送 
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                google::protobuf::RpcController* controller, 
                                const google::protobuf::Message* request,
                                google::protobuf::Message* response,
                                google::protobuf:: Closure* done)
{
    //// 获取方法所属的服务（ServiceDescriptor）、service_name（服务名称）和method_name（方法名称）
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name(); // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    // 将请求消息序列化为字符串，若成功则获取参数大小
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }
    
    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    // 将请求头序列化为字符串，若成功则获取请求头大小
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    // 组织待发送的rpc请求的字符串,请求头大小 + 请求头 + 请求参数
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4)); // header_size,添加请求头的大小（4字节）
    send_rpc_str += rpc_header_str; // rpcheader,添加请求头内容
    send_rpc_str += args_str; // args,添加请求参数内容

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    // 创建一个TCP socket，用于与远程服务器进行连接
    //套接字是计算机网络通信的基本操作单元，允许应用程序通过它发送和接收数据
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);//F_INET 表示我们将使用 IPv4 协议进行网络通信。对于 IPv6，通常会使用 AF_INET6
                                                  //SOCK_STREAM：这个参数指定套接字类型为 TCP（传输控制协议）。
                                                  //如果使用 SOCK_DGRAM，则表示是 UDP（用户数据报协议）套接字。
                                                  //0:指定使用默认协议。对于 AF_INET 和 SOCK_STREAM，它通常会自动选择 IPPROTO_TCP，即使用 TCP 协议
    if (-1 == clientfd)
    {
        // 如果创建socket失败，设置错误信息并返回
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 读取配置文件rpcserver的信息
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    // rpc调用方想调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    // 创建Zookeeper客户端，查询服务节点的地址
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    // 构造请求路径（如：/UserServiceRpc/Login）
    std::string method_path = "/" + service_name + "/" + method_name;
    // 获取Zookeeper中该服务方法的地址（如：127.0.0.1:8000）127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        // 如果无法获取到服务地址，设置失败信息并返回
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    // 提取IP和端口信息
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);// 提取IP地址
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); // 提取端口号

    // 配置目标服务器的地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;// IPv4协议
    server_addr.sin_port = htons(port);// 设置目标端口号
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());// 设置目标IP地址

    // 连接rpc服务节点
    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        // 如果连接失败，关闭socket并设置错误信息
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 发送RPC请求数据到服务器
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 接收rpc请求的响应值,最大接收1024字节
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0)))
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))
    // 将接收到的响应数据反序列化到response对象中
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    // 关闭连接
    close(clientfd);
}