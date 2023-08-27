#ifndef TINY_MUDUO_ACCEPTOR_H_
#define TINY_MUDUO_ACCEPTOR_H_

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <memory>
#include <functional>

#include "noncopyable.h"
#include "address.h"

namespace tiny_muduo {

class EventLoop;
class Address;
class Channel;

//本质上也是channel一个事件分发器，但是这个是专门为lisfd实现的，即新建连接，即只有可读回调函数，会设置套接字的一些属性，端口复用，阻塞非阻塞处理等
class Acceptor : public NonCopyAble {
 public:
  typedef std::function<void (int, const Address&)> NewConnectionCallback;
  
  Acceptor(EventLoop* loop, const Address& address);
  ~Acceptor();

  void BindListenFd(const Address& address); 
  void Listen();
  void NewConnection();

  int SetSockoptKeepAlive(int fd) {
    int option_val = 1;
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                      &option_val, static_cast<socklen_t>(sizeof(option_val)));
  }

  int SetSockoptReuseAddr(int fd) {
    int option_val = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                      &option_val, static_cast<socklen_t>(sizeof(option_val)));
  }

  int SetSockoptTcpNoDelay(int fd) {
    int option_val = 1;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                      &option_val, static_cast<socklen_t>(sizeof(option_val)));
    
  }
 
  void SetNewConnectionCallback(const NewConnectionCallback& callback) {
    new_connection_callback_ = callback;
  }

  void SetNewConnectionCallback(NewConnectionCallback&& callback) {
    new_connection_callback_ = std::move(callback);
  }

 private:
   EventLoop* loop_;
   int listenfd_;
   int idlefd_;
   std::unique_ptr<Channel> channel_;
   NewConnectionCallback new_connection_callback_; 
};

}
#endif