#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication//单例设计模式：MprpcApplication 类是一个单例类，确保在整个应用程序中只有一个 MprpcApplication 实例。这通过将构造函数设为私有并提供静态方法 GetInstance() 实现。
{
public:
    static void Init(int argc, char **argv);//Init 是框架启动时的入口函数，确保系统在启动时进行必要的初始化工作。
    static MprpcApplication& GetInstance();//返回 MprpcApplication 类的单例对象。MprpcApplication 类使用了单例设计模式，确保全局只有一个 MprpcApplication 实例。通过 GetInstance() 方法，其他地方可以获取该实例并访问它的功能。
    static MprpcConfig& GetConfig();//返回 MprpcConfig 配置对象的引用。通过这个函数，其他部分可以访问框架的配置文件或相关设置。
private:
    static MprpcConfig m_config;//用于存储 MprpcConfig 配置对象

    MprpcApplication(){}//这是默认构造函数。它被声明为私有，防止外部直接创建 MprpcApplication 对象。这符合单例设计模式的要求，确保只能通过 GetInstance() 方法来获取实例，而不能直接创建对象。
    MprpcApplication(const MprpcApplication&) = delete;//删除了拷贝构造函数，防止 MprpcApplication 对象被拷贝
    MprpcApplication(MprpcApplication&&) = delete;//删除了移动构造函数，防止 MprpcApplication 对象被移动
};