#pragma once
#include <google/protobuf/service.h>
#include <string>
//google::protobuf::RpcController 是 Google Protocol Buffers 库中的一个接口，通常用于处理 RPC 调用时的控制和状态管理
class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();//构造函数，通常用于初始化成员变量。可以在构造函数中初始化状态。
    void Reset();//重置控制器的状态。通常用于在执行新一轮 RPC 调用之前清理状态，例如重置错误信息和失败状态。
    bool Failed() const;//返回当前 RPC 调用是否失败。通常这个方法用于判断 RPC 调用过程中是否出现错误（例如通信失败、服务器错误等）。
                        //如果 Failed() 返回 true，可以通过 ErrorText() 获取错误信息。
    std::string ErrorText() const;//返回 RPC 调用过程中发生的错误信息。如果 RPC 调用失败，可以通过该方法获取具体的错误描述。
    void SetFailed(const std::string& reason);//设置 RPC 调用失败的原因。通常用于在调用过程中发现错误时，通过该方法设定失败状态和错误信息。

    // 目前未实现具体的功能
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool m_failed; // RPC方法执行过程中的状态
    std::string m_errText; // RPC方法执行过程中的错误信息
};