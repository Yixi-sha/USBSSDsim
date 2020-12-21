#include "../inc/eventloopThread.h"
#include "../inc/eventloop.h"

#include <functional>
#include <iostream>
namespace EDEN{


EventLoopThread::EventLoopThread()
:eventLoop_(nullptr), exiting_(false), 
thread_(std::bind(&EventLoopThread::thread_func, this, std::placeholders::_1)),
mutex_(), cond_(mutex_){

}
EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    eventLoop_->quit();
    thread_.join();
}

std::shared_ptr<EventLoop>  EventLoopThread::start_loop(){
    if(thread_.is_start()){
        LOG_ERROR << "EventLoopThread::start_loop() thread_.is_start()" << endl;
        return eventLoop_;
    }
    if(thread_.start()){
        return eventLoop_;
    }
    
    {
        MutexLockGuard guard(mutex_);
        while(eventLoop_.get() == nullptr){
            cond_.wait();
        }
    }
    return eventLoop_;
}

void* EventLoopThread::thread_func(void *argv){
    std::shared_ptr<EventLoop> eventLoop(EventLoop::construct_eventLoop());
    if(eventLoop.get() == nullptr){
        LOG_ERROR << "EventLoopThread::thread_func " << endl;
        return nullptr;
    }
    {
        MutexLockGuard guard(mutex_);
        eventLoop_ = eventLoop;
        cond_.notify();
    }
    std::cout << gettid() << std::endl;
    eventLoop->loop();

    return nullptr;
}


}