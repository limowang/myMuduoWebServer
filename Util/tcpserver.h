#ifndef TINY_MUDUO_TCPSERVER_H_
#define TINY_MUDUO_TCPSERVER_H_

#include <memory>
#include <map>
#include <utility>
#include <memory>
#include <string>

#include "callback.h"
#include "eventloop.h"
#include "acceptor.h"
#include "eventloopthreadpool.h"
#include "tcpconnection.h"
#include "logging.h"
#include "noncopyable.h"
#include "address.h"

namespace tiny_muduo {

class Address;

class TcpServer : public NonCopyAble {
 public:
  TcpServer(EventLoop* loop, const Address& address);
  ~TcpServer();


  //tcpsever启动之后就将监听描述符挂在主线程的epoll上面，等待客户端发起的连接
  void Start() {
    threads_->StartLoop();

    loop_->RunOneFunc(std::bind(&Acceptor::Listen, acceptor_.get()));
  }

  void SetConnectionCallback(ConnectionCallback&& callback) { 
    connection_callback_ = std::move(callback);
  }

  void SetConnectionCallback(const ConnectionCallback& callback) { 
    connection_callback_ = callback;
  }

  void SetMessageCallback(MessageCallback&& callback) {
    message_callback_ = std::move(callback);
  }

  void SetMessageCallback(const MessageCallback& callback) {
    message_callback_ = callback;
  }

  //设置线程池数量，我设置为4与CPU的核数一致
  void SetThreadNums(int thread_nums) {
    threads_->SetThreadNums(thread_nums);    
  }

  inline void HandleClose(const TcpConnectionPtr& conn);
  inline void HandleCloseInLoop(const TcpConnectionPtr& ptr);
  inline void HandleNewConnection(int connfd, const Address& address);

 private:
  typedef std::map<int, TcpconnectionPtr> ConnectionMap;

  EventLoop* loop_;                           //事件循环，每个线程均有一个，封装了IO多路复用
  int next_connection_id_;                    
  std::unique_ptr<EventLoopThreadPool> threads_;//线程池，封装了各个子Reactor，每一个线程都有eventloop,监听主线程传过来的连接的可读可写事件
  std::unique_ptr<Acceptor> acceptor_;        //监听描述符对应的channel
  const std::string ipport_;

  ConnectionCallback connection_callback_;    //上层httpServer传递的回调函数 -------新连接建立  注册步骤 TCP Server ----> Tcpconnection ---> channel
  MessageCallback message_callback_;
  ConnectionMap connections_;                 //保存所有的已建立的tcpconnection,后面定时器的时候会用到
};

}// namespace tiny_muduo
#endif