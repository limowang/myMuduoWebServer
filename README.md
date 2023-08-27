# limoMuduoWebServer

本项目是基于C++11编写的Web服务器，整个服务器是以Reactor模式的事件驱动模式为核心。  
支持解析HTTP头部，响应HTTP GET请求，可支持服务器长连接。  
实现了服务器定时断开空闲连接（基于内部定时器实现），异步双缓冲日志库完成服务器日志功能。
用Webbench测试达到了万级别的qps.  


  
## System Requires:

  Linux kernel version 5.10.60-9.

    
  编写环境 Alibaba Cloud Linux release 3 

	
## Optimized Methods
  
  * 采用Reactor模式，工作模式采用LT水平触发模式，对长连接短连接处理性能友好  
  * 采用RAII机制，智能指针Shared/Unique Ptr来管理内存，包装了锁资源以及文件资源，避免了可能存在的内存泄漏问题  
  * 采用多线程编程模型
  * 采用Round-Robin算法，当新连接到来时调用  
  * 采用DOUBLE-BUFFERING双缓冲机制，自定义固定大小Buffer并重载运算符，优化异步日志库写入性能    
  * 定时器用了红黑树实现的set效率高  
  * 采用__thread gcc内置线程局部存储设施，存储日期/PID/时间，节省了多次系统调用，进一步优化日志库Logging效率  
  * 采用优雅断开服务器连接方式，避免直接Close而丢失双方仍未发出或接受的数据  
  * 采用C++11 std::move移动函数避免大量不必要的拷贝动作  
  * 采用提前准备一个空的文件描述符，避免因描述符耗尽而导致服务器Busy Loop  
  * 采用有限状态机分析TCP字节流是否为HTTP包，高效的解析HTTP请求中的URI以及HTTP头部参数，采用unordered_map来记录参数方便使用  
  
## Thanks For
  *《linux多线程服务端编程》 陈硕        
  *《Linux高性能服务器编程》 游双