// 一个基于udp协议的网络通信服务端程序
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

int main()
{
  //创建套接字
  //为套接字绑定地址信息
  //接/收发数据
  //关闭套接字
 // 1, 创建套接字 int socket(int domain, int type, int protocol);
 //    domain 协议家族地址域 IPv4(AF_INET)
 //    type 类型(SOCK_STREAM 和 SOCK_DGRAM)  流式套接字和数据报套接字
 //    protocol  协议(tcp和udp)  UDP---17----IPPROTO_UDP（宏）tcp---6-----IPPROTO_TCP
 //  返回值，成功就是一个新的文件描述符(非负整数)，失败返回-1
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock < 0)
  {
    perror("socket error\n");
    return -1;
  }
  cout << "socket success" << endl;
  // 2, 绑定地址信息
  // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
  //    sockfd : 套接字 描述符
  //    *addr : 地址信息 使用sockaddr_in定义，类型强转
  //    addlen: 地址信息长度
  //    返回值：0成功，-1失败
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  //uint32_t htonl(uint32_t hostlong);
  // 将uint32_t的主机字节序转换成网络字节序
  //uint16_t htons(uint16_t hostshort);
  // 将uint16_t的主机字节序转换成网络字节序
  addr.sin_port = htons(9000);
  //in_addr_t inet_addr(const char *cp);
  //将点分十进制的ip地址转换成网络字节序的IP地址
  addr.sin_addr.s_addr = inet_addr("192.168.226.129");
  socklen_t len = sizeof(struct sockaddr_in);
  int ret = bind(sock, (struct sockaddr*)&addr, len);
  if(ret < 0)
  {
    perror("bind error\n");
    return -1;
  }
  cout << "bind success" << endl;
 while(1)
 {
  // 发送数据 
  // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
  // sockfd 套接字描述符---确定从哪获取数据
  // buf   用于接收数据
  // len   接收数据长度
  // flags  标志位，默认为0，表示阻塞接收
  // src_addr   数据从哪里来   对端的地址信息
  // addrlen(输入输出复合型参数)    地址信息长度
  // 返回值  实际接收的数据长度  失败为-1
  char buff[1024] = {0};
  struct sockaddr_in cli_addr;
  recvfrom(sock, buff, 1023, 0, (struct sockaddr*)&cli_addr, &len);
  printf("client sys : %s\n", buff);
  memset(buff, 0x00, 1024);
  //scanf("%s", buff);
  // 接收数据
  //ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
  // sock   套接字描述符，发送到哪
  // buf    要发送的数据
  // len     要发送数据的长度
  // flags   标志位，0表示阻塞发送
  // dest_addr   对端的地址信息，数据接收方的地址信息，目的地址信息
  // addrlen  地址信息长度
  // 返回值  实际发送的数据长度
  string str;
  getline(cin, str);
  sendto(sock, str.c_str(), str.size(), 0, (struct sockaddr*)&cli_addr, len);  
 }
  // 关闭套接字
  close(sock);
  return 0;
}
