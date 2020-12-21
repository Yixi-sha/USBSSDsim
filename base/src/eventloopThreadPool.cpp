#include "../inc/eventloopThreadPool.h"
#include "../inc/eventloop.h"
#include "../inc/channel.h"

#include <functional>

namespace EDEN{

EventLoopThreadPool::EventLoopThreadPool(/*EventLoop *baseloop,*/ int num)
:/*baseloop_(baseloop),*/ threadsNum_(num), next_(0), started_(false){

}
EventLoopThreadPool::~EventLoopThreadPool(){

}

int EventLoopThreadPool::start(){
    int nowThreadsNum = loops_.size();
    started_ = true;
    for(int i = nowThreadsNum; i < threadsNum_; ++i){
        std::shared_ptr<EventLoopThread>  eventLoopThread(new EventLoopThread());
        if(!eventLoopThread.get()){
            threadsNum_ = i;
            return threadsNum_;
        }
        EventLoop *loop = eventLoopThread->start_loop().get();
        if(!loop){
            threadsNum_ = i;
            return threadsNum_;
        }
        threads.push_back(eventLoopThread);
        loops_.push_back(loop);
    }
    return threadsNum_;
    
}

void EventLoopThreadPool::set_thread_num(int num){
    threadsNum_ = num;
}

void EventLoopThreadPool::loop_wait(){
    for(auto &loop : loops_){
        while(loop->is_looping());
    }
}

void EventLoopThreadPool::end_all_looping(){
    for(auto &loop : loops_){
        if(loop->is_looping()){
            loop->quit();
        }
    }
}

EventLoop* EventLoopThreadPool::get_next(){
    /*if(!baseloop_->is_in_loop_thread())
        return nullptr;*/
    if(!started_ || loops_.size() == 0)
        return nullptr;
    if(next_ == loops_.size())
        next_ = 0;
    return loops_[next_++];
}

}