// 使用封装好的TCPSocketApi类来实现tcp服务端程序
// 1，创建套接字
// 2，绑定地址信息
// 3，开始监听
// 4，获取新连接
// 5，接收或者发送数据
// 6，关闭套接字
#include "TCPSocket.hpp"

#define CHECK_RET(q) if((q) == false) {return -1;}

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    cout << "Usge : TCPServer:  ip  port " <<  endl;
    return -1;
  }
  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  TCPSocketApi sp;

  CHECK_RET(sp.Socket());
  CHECK_RET(sp.Bind(ip, port));
  CHECK_RET(sp.Listen());

  while(1)
  {
    TCPSocketApi clisock;
    //获取新连接，没有新连接就是阻塞的，阻塞等待
    if(sp.Accept(&clisock) == false)
      continue;
    if(fork() == 0)
    {
      if(fork() == 0)
      {
        while(1)
        {
           char buff[1024];
           clisock.Recv(buff, 1023); 
           cout << "client say : " << buff << endl;
           string str;
           cout << "server say: ";
           fflush(stdout);
           getline(cin, str);
           clisock.Send(str.c_str(), str.size());
        }
        clisock.Close();
      }
      else 
      {
        exit(0);
      }
    }
  }
  sp.Close();
  return 0;
}
