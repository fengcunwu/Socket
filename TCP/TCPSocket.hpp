//  编写一个tcpsocket的类，实现tcpsocket的接口封装使用
//  1，创建套接字
//  2，绑定地址信息
//  3，监听
//  4，获取新链接
//  5，发起连接请求
//  6，发送数据
//  7，接收数据
//  8，关闭套接字
#pragma once 
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define CHECK_RET(q) if((q) == false) {return -1;} 

using namespace std;

class TCPSocketApi
{
  public:
    TCPSocketApi()
      :_sock(-1)
    {}
    // 1，创建套接字
    bool Socket()
    {
      _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(_sock < 0)
      {
        perror("socket error\n");
        return false;
      }
      return true;
    }
    // 2， 绑定地址信息
    bool Bind(string &ip, uint16_t port)
    {
      int ret = 0;
      sockaddr_in tcp_addr;
      tcp_addr.sin_family = AF_INET;
      tcp_addr.sin_port = htons(port);
      tcp_addr.sin_addr.s_addr = inet_addr(ip.c_str());
      socklen_t tcp_addr_len = sizeof(tcp_addr);
      ret = bind(_sock, (struct sockaddr*)&tcp_addr, tcp_addr_len);
      if(ret < 0)
      {
        perror("bind error\n");
        return false;
      }
      return true;
    }
   // 3, 监听 
   bool Listen(int backlog = 5)
   {
     //  int listen(int sockfd, int backlog);
     //  sockfd 要监听的哪一个套接字
     //  backlog tcp同一时间最大并发连接数，在调用listen的时候，操作系统会在内部创建一个队列，叫最大并发连接数队
     //  这个第二个参数就是用于定义队列最大节点个数的，每一个新的连接到来时，都会创建一个新的socket，然后放在
     //  这个队列中，一旦队列满了，后续来的链接全部丢弃
     //  返回值  非负整数表示一个套接字描述符，-1表示返回失败
      int ret = listen(_sock, backlog);
      if(ret < 0)
      {
        perror("listen error\n");
        return false;
      }
      return true;
   }

   // 4, 获取新连接----获取服务端为新客户所见的socket连接
   bool Accept(TCPSocketApi *sp, sockaddr* cliaddr = NULL)
   {
     // 从已完成连接的队列获取一个已连接的socket
     // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
     // sockfd  套接字描述符，从哪获取
     // addr  地址信息，新建连接(客户端)的地址信息，
     // addrlen 地址信息长度
     // 返回值： 新建的套接字描述符， 失败-1
      socklen_t len = sizeof(sockaddr_in);
      sockaddr_in addr;
      int newcli = accept(_sock, (sockaddr*)&addr, &len);
      if(newcli < 0)
      {
        perror("accept error\n");
        return false;
      }
      // 
      sp->_sock = newcli;
      if(cliaddr != NULL)
      {
        memcpy(cliaddr, &addr, len);
      }
      return true;
   }

   // 5,发起连接请求
   bool Connect(string &ip, uint16_t port)
   {
     // 向服务器发起连接请求，
      // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
      // sockfd  当前sock描述符，为这个socket发起连接请求
      // addr  服务器的地址信息
      // addrlen   地址信息长度
      // 返回值 -1 表示失败， 0 表示成功
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(ip.c_str());
      socklen_t len = sizeof(addr);
      int ret = connect(_sock, (struct sockaddr*)&addr, len);
      if(ret < 0)
      {
        perror("connect error\n");
        return false;
      }
      return true;
   }
   // 6, 发送数据
   ssize_t Recv(char* buff, size_t len)
   {
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    // MSG_PEEK  从接收队列缓冲区的起始位置返回数据，但是不移除数据，通常用作探测性接收数据
    // sockfd 从哪接收数据，套接字描述符(从缓冲区取数据)
    // buf  接收后存在buf里
    // len 接收的长度
    // flags  标志位，0 表示阻塞
    // 返回实际接收的长度
    // 返回值  接收实际长度  失败返回-1   0 连接断开
    ssize_t ret = recv(_sock, buff, len, 0);
    if(ret < 0)
    {
      perror("recv error\n");
      return -1;
    }else if(ret == 0)
    {
      printf("peer shutdown\n");
      return 0;
    }
    return ret;
   }
   // 7, 发送数据
   ssize_t Send(const char* buff, size_t len)
   {
     // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
     // 返回实际发送的长度
     // 因为tcp服务会为客户端新建socket进行专项通信，因此发送的时候，不需要指定地址信息，
     ssize_t ret = send(_sock, buff, len, 0);
     if(ret < 0)
     {
       perror("send error\n");
       return -1;
     }
     return ret;
   }

   // 8, 关闭套接字
   bool Close()
   {
     if(_sock != -1)
     {
       close(_sock);
       _sock = -1;
     }
     return true;
   }
  private:
    int _sock;
};
