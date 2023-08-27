#include "eventloopthreadpool.h"

#include <memory>

#include "eventloopthread.h"

using namespace tiny_muduo;

//loop 是由httpsever传进来的
EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop)
    : base_loop_(loop),
      thread_nums_(0),
      next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::StartLoop() {
  for (int i = 0; i < thread_nums_; ++i) {
    EventLoopThread* ptr = new EventLoopThread();
    threads_.emplace_back(std::unique_ptr<EventLoopThread>(ptr));
    loops_.emplace_back(ptr->StartLoop());
  }
}

//简单的负载均衡-----轮询算法，在新连接到来的时候调用
EventLoop* EventLoopThreadPool::NextLoop() {
  EventLoop* ret = base_loop_;
  if (!loops_.empty()) {
    ret = loops_[next_++];
    if (next_ == static_cast<int>(loops_.size())) next_ = 0;
  }

  return ret;
}