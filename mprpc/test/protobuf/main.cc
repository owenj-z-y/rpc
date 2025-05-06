#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败了");
    
    GetFriendListsResponse rsp;  // 创建一个 GetFriendListsResponse 对象
    ResultCode *rc = rsp.mutable_result();  // 获取可变的 ResultCode 指针
    rc->set_errcode(0);  // 设置错误代码为0（表示没有错误）

    // 创建并设置第一个朋友的属性
    User *user1 = rsp.add_friend_list();  
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    // 创建并设置第二个朋友的属性
    User *user2 = rsp.add_friend_list();  
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;  // 输出朋友列表的大小


    return 0;
}

int main1()
{
    // 封装了login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    // 对象数据序列化 =》 char*
    std::string send_str;
    if (req.SerializeToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }

    // 从send_str反序列化一个login请求对象
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    return 0;
}