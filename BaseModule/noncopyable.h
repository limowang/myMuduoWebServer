#ifndef TINY_MUDUO_NONCOPYABLE_H_
#define TINY_MUDUO_NONCOPYABLE_H_

namespace tiny_muduo {

//取消类的复制属性，具有相同类直接继承此类即可
class NonCopyAble {
 protected:
  NonCopyAble() {}
  ~NonCopyAble() {}
 
 private:
  NonCopyAble(const NonCopyAble&) = delete;
  NonCopyAble& operator=(const NonCopyAble&) = delete;
};

} // namespace tiny_muduo

#endif