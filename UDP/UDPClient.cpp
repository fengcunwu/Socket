// 使用c++封装一个类，udpsocket
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

class UdpSocket
{
  private:
    int _sock;
  public:
    UdpSocket()
      : _sock(-1)
    {}
    //1，创建套接字
    bool Socket()
    {
      _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if(_sock < 0)
      {
        perror("error socket\n");
        return false;
      }
      return true;
    }
    //2, 绑定地址信息
    bool Bind(uint16_t port, string ip)
    {
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(ip.c_str());
      socklen_t len = sizeof(addr);
      int ret = bind(_sock, (struct sockaddr*)&addr, len);
      if(ret < 0)
      {
        perror("bind error\n");
        return false;
      }
      return true;
    }
    //3, 发送数据
    ssize_t Send(const char* buff, size_t len, struct sockaddr_in *addr)
    {
      ssize_t ret = 0;
      socklen_t addr_len = sizeof(struct sockaddr_in);
      ret = sendto(_sock, buff, len, 0, (struct sockaddr*)addr, addr_len);
      return ret;
    }


    //4，接收数据
    ssize_t Recv(char* buff, size_t len, struct sockaddr_in *addr = NULL)
    {
      ssize_t ret = 0;
      sockaddr_in _addr;
      socklen_t addr_len = sizeof(_addr);
      ret = recvfrom(_sock, buff, len, 0, (struct sockaddr*)&_addr, &addr_len);
      if(addr)
      {
        memcpy((void *)addr, (void *)&_addr, len);
      }
      return ret;
    }
    //5，关闭套接字
    bool Close()
    {
      if(_sock != -1)
      {
        close(_sock);
        _sock = -1;
      }
      return true;
    }
};

#define CHECK_RET(q) if((q) == false) {return -1;}

int main()
{
  UdpSocket sp;

  CHECK_RET(sp.Socket());

  sockaddr_in ser_addr;
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_port = htons(9000);
  ser_addr.sin_addr.s_addr = inet_addr("192.168.226.129");
  //客户端不推荐手动绑定地址信息
  while(1)
  {
    char buff[1024] = {0};
    string str;
    getline(cin,str);
    sp.Send(str.c_str(), str.size(), &ser_addr);
  
    sp.Recv(buff, 1024);

    cout << "server say : " << buff  << endl;
  }
  
  return 0;
}
