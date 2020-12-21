#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

#include "timestamp.h"
#include "noncopyable.h"
#include "channel.h"

#include <functional>
#include <set>
#include <vector>
#include <memory>

namespace EDEN{

class Timer : public Noncopyable{
private:
    std::function<void()> timeCallback_;
    Timestamp expire_;
    const double interval_;
    bool repeat_;
public:
    Timer(const std::function<void()> &cb, Timestamp when, double interval);
    ~Timer(){

    }

    void run() const;
    Timestamp expire() const;
    bool repeat() const;
    void set_repeat(bool on);
    bool restart(Timestamp now);

};

class TimerId : Object{
private:
    Timer* value_;

public:
    explicit TimerId(Timer* timer = nullptr): value_(timer){
    }  

    Timer* get_value(){
        return value_;
    }

};

class EventLoop;

class TimerQueue : public Noncopyable{
private:
    EventLoop *eventLoop_;
    const int timerFd_;
    std::unique_ptr<Channel> channel_;
    std::set<std::pair<Timestamp, Timer*>> timers_;

    std::set<Timer*> willDelet_;

    bool callbacking_;

    void handle_read();
    std::vector<std::pair<Timestamp, Timer*>> get_expired(Timestamp now);
    bool insert(Timer* timer);
    void reset(std::vector<std::pair<Timestamp, Timer*>> &expireds, Timestamp now);
    bool add_timer_in_loop(Timer *timer);
    bool construct_two();
    TimerQueue(EventLoop *eventLoop);
    void cancel_in_loop(TimerId timerId);
public:
    static TimerQueue* construct_timerQueue(EventLoop *eventLoop);
    ~TimerQueue();

    TimerId add_timer(const std::function<void()> cb, Timestamp when, double interval);
    void cancel(TimerId timerId);
};
}

#endif