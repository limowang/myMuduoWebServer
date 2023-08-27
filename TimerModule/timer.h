#ifndef TINY_MUDUO_TIMER_H_
#define TINY_MUDUO_TIMER_H_

#include <functional>

#include "timestamp.h"
#include "noncopyable.h"

namespace tiny_muduo {

class Timer : public NonCopyAble {
 public:
  typedef std::function<void()> BasicFunc;

  Timer(Timestamp timestamp, BasicFunc&& cb, double interval);

  //重新设置下一次超时时间
  void Restart(Timestamp now) {
    expiration_ = Timestamp::AddTime(now, interval_);
  }

  //运行回调函数
  void Run() const {
    callback_();
  }

  Timestamp expiration() const { return expiration_; }
  bool repeat() const { return repeat_; }

 private:
  Timestamp expiration_;          //超时时间
  BasicFunc callback_;            //任务函数，唤醒之后的回调函数
  double interval_;               //唤醒间隔
  bool repeat_;                   //是否是重复定时事件
};

} // namespace tiny_muduo

#endif