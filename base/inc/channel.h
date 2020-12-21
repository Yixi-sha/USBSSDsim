#ifndef __EDEN_CHANNEL_H__
#define __EDEN_CHANNEL_H__

#include <functional>

#include "noncopyable.h"
#include "timestamp.h"


namespace EDEN{

class EventLoop;

class Channel : public Noncopyable{
public:
    typedef std::function<void(Timestamp)> EventCallback;
private:
    const int fd_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_; 

    static const int kNoneEvent_;
    static const int kReadEvent_;
    static const int kWriteEvent_;
    static const int kErrorEvent_;

    bool handling_;
    unsigned int events_;
    unsigned int revents_;
    int index_;
    EventLoop *eventLoop_;
    
    Channel(EventLoop *eventLoop, int fd);
    bool construct_two();

    void update();
public:
    static Channel *construct_channel(EventLoop *eventLoop, int fd);
    ~Channel();

    void handle_event(Timestamp time);
    void set_read_callback(const EventCallback& cb);
    void set_write_callback(const EventCallback& cb);
    void set_error_callback(const EventCallback& cb);

    int fd() const;

    unsigned int events() const;
    void set_revents(int revent);
    bool is_none_event() const;
    
    void enable_read();
    void disable_read();
    void enable_write();
    void disable_write();
    void enable_all();
    void disable_all();
    
    bool is_writing() const;
    
    int index() const;
    void set_index(int index);

    EventLoop* owner_loop() const;

};

}


#endif