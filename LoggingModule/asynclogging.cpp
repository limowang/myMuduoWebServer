#include "asynclogging.h"
#include <functional>
#include <utility>
#include <memory>

using namespace tiny_muduo;

//往缓冲区里面写，当写满了唤醒后台工作线程开始处理
void AsyncLogging::Append(const char* data, int len) {
  MutexLockGuard guard(mutex_);
  if (current_->writablebytes() >= len) {
    current_->Append(data, len);
  } else {
    buffers_to_write_.emplace_back(std::move(current_)); 
    if (next_) {
      current_ = std::move(next_);
    } else {
      current_.reset(new Buffer());
    }
    //后台线程被唤醒开始工作
    cond_.Notify();
  }
}

void AsyncLogging::Flush() {
  fflush(stdout);
}

void AsyncLogging::ThreadFunc() {
  latch_.CountDown();
  BufferPtr newbuffer_current(new Buffer());
  BufferPtr newbuffer_next(new Buffer());
  LogFilePtr log(new LogFile(filepath_));
  newbuffer_current->SetBufferZero();
  newbuffer_next->SetBufferZero();
  BufferVector buffers;

  while (running_) {
    {
      MutexLockGuard guard(mutex_);
      if (buffers_to_write_.empty()) {
        cond_.WaitForFewSeconds(kBufferWriteTimeout);
      }

      //直接交换两个缓冲区，避免赋值操作
      buffers_to_write_.emplace_back(std::move(current_));
      buffers.swap(buffers_to_write_);
      current_ = std::move(newbuffer_current);
      if (!next_) {
        next_ = std::move(newbuffer_next);
      }
    }
    
    //开始写日志
    for (const auto& buffer : buffers) {
      log->Write(buffer->data(), buffer->len());
    }

    if (log->writtenbytes() >= kSingleFileMaximumSize) {
      log.reset(new LogFile(filepath_));
    } 
    
    if (buffers.size() > 2) {
      buffers.resize(2);
    }

    if (!newbuffer_current) {
      newbuffer_current = std::move(buffers.back());
      buffers.pop_back();
      newbuffer_current->SetBufferZero();
    }

    if (!newbuffer_next) {
      newbuffer_current = std::move(buffers.back());
      buffers.pop_back();
      newbuffer_current->SetBufferZero();
    }
    buffers.clear();
  }
}