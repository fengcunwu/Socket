#include "Select_Tcp.hpp"

using namespace std;

void Usage(string proc)
{
  cout << "Usage: " << proc << "  prot" << endl;
}
int main(int argc, char *argv[])
{
  //只传入端口号
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(0);
  }

  int port = atoi(argv[1]);//字符串转整形
  Server *sp = new Server(port);
  sp->InitServer();
  sp->Start();
  delete sp;
  return 0;
}
