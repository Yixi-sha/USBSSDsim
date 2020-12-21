#ifndef __EDEN_EVENTLOOP_H__
#define __EDEN_EVENTLOOP_H__

#include <iostream>
#include <memory>
#include <vector>

#include "noncopyable.h"
#include "log.h"
#include "poller.h"
#include "channel.h"
#include "timer.h"
#include "mutex.h"


extern "C"{
#include <pthread.h>
#include <poll.h>
}



namespace EDEN{

class EventLoop : public  Noncopyable{
private:
    const pid_t tid_;
    bool looping_;
    bool quit_;
    std::unique_ptr<Poller> poller_;
    std::vector<Channel*> activeChannels_;
    std::unique_ptr<TimerQueue> timerQueue_;

    bool callingPendingFuncBool_;
    int wakeFd_;
    std::unique_ptr<Channel> wakeChannel_;
    Mutex pendingFuncMutex_;
    std::vector<std::function<void()>> pendingFunc_;

    
    void wakeup();
    void do_pending_func();
    void handle_read_wake();

    bool construct_two();
    EventLoop();

    void queue_in_loop(const std::function<void()> &cb);
public:
   
    ~EventLoop();

    static EventLoop *construct_eventLoop();

    bool loop();
    void quit();
    void update_channel(Channel *channel);
    bool is_in_loop_thread() const{
        return tid_ == gettid();
    }
    TimerId run_at(const Timestamp &time, const std::function<void()> &cb);
    TimerId run_after(double delay, const std::function<void()> &cb);
    TimerId run_every(double interval, const std::function<void()> &cb);

    static EventLoop* get_event_loop_of_current_thread();
    void run_in_loop(const std::function<void()> &cb);
    bool remove_channel(Channel* channel);

    void cancel_timer(TimerId timerid){
        timerQueue_->cancel(timerid);
    }

    bool is_looping()const {
        return looping_;
    }
};

}



#endif