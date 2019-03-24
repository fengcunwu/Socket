// 用c语言实现http服务器，实现一个最简单的http服务器
// <html><body><h1>Hello Bit!</h1></body></html>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "Usage :./httpServer ip port" << endl;
  }
  //首先实现tcp服务端程序
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0)
  {
    perror("socket error\n");
    exit(0);
  }
  cout << "socket success" << endl;
  struct sockaddr_in http_addr;
  http_addr.sin_family = AF_INET;
  http_addr.sin_port = htons(atoi(argv[2]));//端口号
  http_addr.sin_addr.s_addr = inet_addr(argv[1]);//ip
  socklen_t http_len = sizeof(http_addr);
  int ret = bind(sock, (struct sockaddr*)&http_addr, http_len);
  if(ret < 0 )
  {
    perror("bind error\n");
    exit(1);
  }
  cout << "bind success" << endl;

  ret = listen(sock, 5);//返回0 表示成功，返回-1失败
  if(ret < 0)
  {
    perror("listen error\n");
    exit(2);
  }
  cout << "listen success" << endl;

  while(1)
  {
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int new_sock = accept(sock, (struct sockaddr*)&cli_addr, &cli_len);
    if(new_sock < 0 )
    {
      perror("accept error\n");
      continue;
    }
    char header[1024] = {0};
    ret = recv(new_sock, header, 1023, 0);
    if(ret <= 0)
    {
      // 返回值如果是0，对端关闭链接, 管道没有写端的时候，也返回0
      perror("recv error\n");
      close(new_sock);
      continue;
    }
    printf("header:[%s]\n", header);

    char* text = "<html><body><h1>Hello Bit!</h1></body></html>";
    char* first_line = "HTTP/1.1 404 NO Found";// 200 表示正常处理， 302 表示重定向
    char head[1024] = {0}; 
    sprintf(head, "Content-Lengt: %d\r\n", strlen(text));
    strcat(head, "Location: http://www.baidu.com/\r\n\r\n");
    
    //组织首行
    send(new_sock, first_line, strlen(first_line), 0);
    //组织头部，里面只有Contect-Length
    send(new_sock, head, strlen(head), 0); 
    //没有正文
    send(new_sock, text, strlen(text), 0); 
    close(new_sock);    
  }
  
  return 0;
}
