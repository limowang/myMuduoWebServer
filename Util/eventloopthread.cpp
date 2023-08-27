#include "eventloopthread.h"

#include <pthread.h>
#include <functional>

#include "mutex.h"
#include "condition.h"
#include "eventloop.h"

using namespace tiny_muduo;

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      thread_(std::bind(&EventLoopThread::StartFunc, this)),
      mutex_(),
      cond_(mutex_) {
}

EventLoopThread::~EventLoopThread() {}        

//这个任务是在主线程中运行的
EventLoop* EventLoopThread::StartLoop() {
  thread_.StartThread();
  EventLoop* loop = nullptr;
  {   
    MutexLockGuard lock(mutex_);
    while (loop_ == nullptr) {
      cond_.Wait();
    }
    loop = loop_;
  }
  return loop;
}

//这个函数是在子线程环境中运行的
void EventLoopThread::StartFunc() {
  EventLoop loop;

  //初始化成功就唤醒
  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.Notify();
  }

  //启动Loop
  loop_->Loop();
  {
    MutexLockGuard lock(mutex_);
    loop_ = nullptr;
  }
}