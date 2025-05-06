#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

//MprpcChannel是自定义的RPC通道类，继承自 google::protobuf::RpcChannel。google::protobuf::RpcChannel 是 protobuf 提供的接口，用于定义 RPC 调用的通道。它为 RPC 请求和响应提供了基础的网络通信能力，
class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    // 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据数据序列化和网络发送 
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf:: Closure* done);
    //CallMethod 是一个虚函数，属于 google::protobuf::RpcChannel 类，用于实现方法调用的实际过程。每当客户端通过 stub 代理对象调用远程方法时，都会进入这个方法。
    //将其声明为虚函数的目的是允许不同的 RpcChannel 子类（例如 MprpcChannel）提供各自的实现方式
    /* method (const google::protobuf::MethodDescriptor*):

    MethodDescriptor 描述了被调用的 RPC 方法的信息。它包含方法的名称、输入输出类型等信息。你可以通过这个描述符来知道调用的是哪个远程方法，并且可以根据它进行序列化、反序列化操作。
    controller (google::protobuf::RpcController*):

    RpcController 是一个控制 RPC 执行过程的对象，通常用于处理请求的超时、取消、错误处理等。通过它，你可以管理 RPC 调用的生命周期，控制调用的过程，例如，检查调用是否超时或者是否需要取消。
    request (const google::protobuf::Message*):

    这是要发送的请求数据，它是一个 protobuf 消息。请求消息封装了调用远程方法时传递的参数数据。它是通过 google::protobuf::Message 类的派生类进行传递的。
    response (google::protobuf::Message*):

    这是 RPC 调用的响应消息，response 是一个空的消息对象，在调用结束时会被填充为远程方法的返回值。它是一个 protobuf 消息对象，可以根据 method 参数中描述的返回类型进行填充。
    done (google::protobuf::Closure*):

    Closure 是一个可调用对象（通常是一个回调），当 RPC 调用完成时，它会被执行。这个回调可以用来处理 RPC 调用完成后的后续操作，例如通知调用者 RPC 完成或处理错误。
    */
};
