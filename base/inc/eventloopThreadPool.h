#ifndef __EDEN_EVENTLOOPTHREADPOOL_H__
#define __EDEN_EVENTLOOPTHREADPOOL_H__

#include "noncopyable.h"
#include "eventloopThread.h"
#include <vector>
#include <memory>

namespace EDEN{
class Channel;

class EventLoopThreadPool : public Noncopyable{
private:
    //EventLoop *baseloop_;
    bool started_;
    int threadsNum_;
    int next_;

    std::vector<std::shared_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops_;

public:
    EventLoopThreadPool(/*EventLoop *baseloop, */int num = 5);
    ~EventLoopThreadPool();

    int start();
    void set_thread_num(int num);
    void end_all_looping();
    void loop_wait();
    EventLoop* get_next();
};

}



#endif