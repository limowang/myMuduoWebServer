#ifndef TINY_MUDUO_TCPCONNECTION_H_
#define TINY_MUDUO_TCPCONNECTION_H_

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string> 
#include <memory>
#include <utility>

#include "callback.h"
#include "channel.h"
#include "buffer.h"
#include "httpcontent.h"
#include "noncopyable.h"
#include "timestamp.h"

using std::string;

namespace tiny_muduo {

class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>, NonCopyAble {
 public:
  enum ConnectionState {
    kConnecting,
    kConnected,
    kDisconnecting,
    kDisconnected
  };

  TcpConnection(EventLoop* loop, int connfd, int id);
  ~TcpConnection();

  void SetConnectionCallback(const ConnectionCallback& callback) { 
    connection_callback_ = callback;
  }

  void SetConnectionCallback(ConnectionCallback&& callback) { 
    connection_callback_ = std::move(callback);
  }

  void SetMessageCallback(const MessageCallback& callback) {
    message_callback_ = callback;
  }

  void SetMessageCallback(MessageCallback&& callback) {
    message_callback_ = std::move(callback);
  }

  void SetCloseCallback(const CloseCallback& callback) {
    close_callback_ = callback;
  }

  void SetCloseCallback(CloseCallback&& callback) {
    close_callback_ = std::move(callback);
  }

  //由主线程在新建连接时调用
  void ConnectionEstablished() {
    state_ = kConnected;
    channel_->Tie(shared_from_this());
    channel_->EnableReading();
    connection_callback_(shared_from_this(), &input_buffer_);
  }

  void Shutdown();  
  bool IsShutdown() const { return state_ == kDisconnecting; }
  int GetErrno() const;
  void ConnectionDestructor();
  void HandleClose();             //回调函数，注册给每个连接到分发器，即channel
  void HandleError();
  void HandleMessage();
  void HandleWrite();
  void Send(Buffer* buffer);
  void Send(const string& str);
  void Send(const char* message, int len);
  void Send(const char* message) { Send(message, static_cast<int>(strlen(message))); }
  void UpdateTimestamp(Timestamp now) { timestamp_ = now; } //更新事件戳

  Timestamp timestamp() const { return timestamp_; }
  int fd() const { return fd_; }
  int id() const { return connection_id_; }
  EventLoop* loop() const { return loop_; }
  HttpContent* GetHttpContent() { return &content_; }

 private:
  EventLoop* loop_;
  int fd_;
  int connection_id_;
  ConnectionState state_;
  std::unique_ptr<Channel> channel_;
  Buffer input_buffer_;
  Buffer output_buffer_;
  HttpContent content_;             //对收到的消息进行处理，自动状态机进行解析
  bool shutdown_state_;
  Timestamp timestamp_;

  ConnectionCallback connection_callback_;
  MessageCallback message_callback_;
  CloseCallback close_callback_;
};

typedef std::shared_ptr<TcpConnection> TcpconnectionPtr;

}  // namespace tiny_muduo
#endif