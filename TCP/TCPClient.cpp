//使用tcpsock类实现tcp客户端程序
// 1，创建套接字
// 2，发起连接请求
// 3，接收或者发送数据
// 4，关闭套接字
//

#define CHECK_RET(q) if((q) == false) {return -1;}

#include "TCPSocket.hpp"

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    cout << "Usge : TCPClient  ip  port ";
    return -1;
  }

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  TCPSocketApi sp;
  CHECK_RET(sp.Socket());
  CHECK_RET(sp.Connect(ip, port));

  while(1)
  {
    string str;
    cout << "client say: ";
    fflush(stdout);
    getline(cin, str);
    sp.Send(str.c_str(), str.size());  
  
    char buff[1024] = {0};
    sp.Recv(buff, 1023);
    cout << "server say: " << buff << endl; 
  }
  sp.Close();
  return 0;
}
