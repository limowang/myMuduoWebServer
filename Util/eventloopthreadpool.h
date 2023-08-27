#ifndef TINY_MUDUO_EVENTLOOPTHREADPOOL_H_
#define TINY_MUDUO_EVENTLOOPTHREADPOOL_H_

#include <vector>
#include <memory>

#include "noncopyable.h"

namespace tiny_muduo {

class EventLoopThread;
class EventLoop;

//线程池的封装
class EventLoopThreadPool : public NonCopyAble {
  public:
   typedef std::vector<std::unique_ptr<EventLoopThread>> Thread;
   typedef std::vector<EventLoop*> Loop;

   EventLoopThreadPool(EventLoop* loop);
   ~EventLoopThreadPool();
   
   void SetThreadNums(int thread_nums) {
     thread_nums_ = thread_nums;
   }

   void StartLoop();
   EventLoop* NextLoop();

  private:
   EventLoop* base_loop_;   
   Thread threads_;                 //各个工作线程，先初始化loop再调用对应的工作函数
   Loop loops_;                     //拥有各个子线程的Loop方便进行通信

   int thread_nums_;
   int next_;
};

}
#endif