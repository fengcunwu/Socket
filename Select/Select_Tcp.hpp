//这是一个运用select技术编写的服务器，select监听多个文件描述符，在本机上启动服务器，百度本机IP地址加端口号，会显示一段固定的字符串

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
#include <sys/select.h>
#include <vector>

using namespace std;

#define DEFAULT_FD -1

class Server
{
  private:
    int listen_sock; 
    int _port;
    int *fd_array;//用于存放文件描述符，在调整之前存放文件描述符
    int _capacity;
    int _size;//合法文件描述符的个数,数组中有效文件描述符的个数
  public:
    Server(const int& port)
      : _port(port)
    {}

    ~Server()
    {}

    void  InitServer()
    {
      _capacity = sizeof(fd_set) * 8;
      fd_array = new int[_capacity];
      for(auto i = 0; i < _capacity; i++)
      {
        fd_array[i] = DEFAULT_FD;
      }
      //1 创建套接字
      listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(listen_sock < 0)
      {
        perror("socket error");
        cerr << "socket error" << endl;
        exit(2);
      }

      int opt = 1;
      setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//让异常断开的服务器立即重启，
      //原理就是,在服务器异常断开的时候，会进入time_wait状态，从而占用端口，这个函数的作用就是，让服务器不进入time_wait状态，
      //listen_sock:设置的套接字描述符
      //SOL_SOCKET : 在套接字级别上设置
      //SO_REUSEADDR : 允许端口复用

      //绑定地址信息
      struct sockaddr_in listen_sock_addr;
      listen_sock_addr.sin_family = AF_INET;
      listen_sock_addr.sin_port = htons(_port);//端口号
      listen_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址 INADDR_ANY表示0.0.0.0或者是所有地址，任意地址
      socklen_t listen_sock_len = sizeof(listen_sock_addr);
      if(bind(listen_sock, (struct sockaddr*)&listen_sock_addr, listen_sock_len) < 0)
      {
        cerr << "bind error" << endl;
        exit(3);
      }
      
      // 监听套接字
      if(listen(listen_sock, 5) < 0)//返回一个新的文件描述符
      {
        cerr << "listen error" << endl;
        exit(4);
      }
    }

    int AddAllfd(int fds[], fd_set &rfds)
    {
      int max = fds[0];
      for(auto i = 0; i < _size; i++)
      {
        if(fds[i] == DEFAULT_FD)
        {
          continue;
        }
         FD_SET(fds[i], &rfds); 
         if(max < fds[i])
           max = fds[i];
      }
      return max;
    }

    bool Addfd(int fd, int fds[])
    {
      cout << "add fd is: " << fd << endl;
      if(_size < _capacity)
      {
        //是合法的文件描述符
        fds[_size++] = fd;
        cout << "size : " << _size << " capacity : " << _capacity  << endl;
        return true;
      }
      return false;
    }

    void Delfd(int index, int fds[])
    {
      cout << "delete fd is : " << fds[index] << endl; 
      fds[index] = fds[_size - 1];
      fds[--_size] = DEFAULT_FD;
      cout << "size : " << _size << " capacity " << _capacity << endl; 
    }

    void Start()
    {
      fd_array[0] = listen_sock; //把监听套接字保存
      _size = 1;//有效的文件描述符有一个是listen_sock
      while(1)
      {  
         fd_set rfds;//创建一个fd_set的类型
         FD_ZERO(&rfds);//初始化位图，
          
        //将所有合法的文件描述符添加到位图中去
        int maxfd =  AddAllfd(fd_array, rfds);

         //把监听套接字放入位图
       struct timeval timeout = {20, 0};//设置超时时间位5秒
       //如果timeout时间是0，就是非阻塞的，select没有等待到任何文件描述符就立即返回，
       // 如果timeout时间设为NULL就是阻塞的

       switch(select(maxfd + 1, &rfds, NULL, NULL, &timeout))
       {
          case -1 :
            {
              cerr << "select error" << endl;
              break;
            }
          case 0 :
           {
             cout << "select timeout..." << endl;
             break;
           }
          default:
           {
             for(auto i = 0; i < _size; i++)
             {
               //有连接来了---只关心了listen_sock，如果成功了肯定是listen_sock成功就绪
                  if(i == 0 && FD_ISSET(fd_array[i], &rfds))
                  {
                     cout << "get a net connect...." << endl;
                     struct sockaddr_in peer;//创建一个结构体用于保存获取新连接的地址
                     socklen_t peer_len;
                     int new_sock = accept(listen_sock, (struct sockaddr*)&peer, &peer_len);
               //这次accept不可能阻塞，因为select等过了，等的任务select已经解决了
                    if(new_sock < 0)//获取新连接失败
                    {
                       cerr << "accept error" << endl;
                       continue;
                    }
                    cout << "get a new client, fd is: " << new_sock << endl;
               //走到这一步说明获取新连接成功，但是不能保证立马有数据，也不能直接读，一次需要把新的文件描述符放入位图中
               //先间接的把新的套接字描述符放入数组中，
                   if(!Addfd(new_sock, fd_array))
                   {
                    //数组中文件描述符满了，装入失败
                     cerr << "socket full" << endl;
                     close(new_sock);
                   }
                 }
                 else if(FD_ISSET(fd_array[i], &rfds))
                 {
                   //走到这一步，说明来了一个新连接，但不是listen_sock
                   //说明数组中的i号文件描述符就绪，可以开始读了；
                   char buff[10240];
                   ssize_t s = recv(fd_array[i], buff, sizeof(buff) - 1, 0);//从哪读，读的数据存到哪，读的大小，以什么方式读
                   if(s > 0)
                   {
                     //读取成功
                     buff[s] = 0;
                     cout << buff << endl;
                     const string echo_http = "HTTP/1.0 200 OK \r\n\r\n<html>hello select </html>\r\n";
                     send(fd_array[i], echo_http.c_str(), echo_http.size(), 0);
                     close(fd_array[i]);
                     Delfd(i, fd_array);
                     cout << "echo_HTTP response Done!" << endl;
                   }
                   else if(s == 0)
                   {
                     //对端关闭
                     cout << "client shutdown" << endl;
                     close(fd_array[i]);
                     //说明这个文件描述已经是无效的文件描述符，因为对端已经关闭，要将这个文件描述符从数组中拿出来
                     //但是要保证这个数组的前半部分存放的是有效的文件描述符，因此需要进行操作
                     Delfd(i, fd_array);
                   }
                   else 
                   {
                     cerr << "recv error" << endl;
                   }
                 }
             }
             break;
           }
       }
     }
   } 
};




















