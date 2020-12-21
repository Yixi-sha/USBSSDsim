#include "../inc/channel.h"
#include "../inc/log.h"
#include "../inc/eventloop.h"

extern "C"{
#include <sys/epoll.h>
}

namespace EDEN{

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent_ = EPOLLOUT;
const int Channel::kErrorEvent_ = EPOLLERR;


Channel::Channel(EventLoop *eventLoop, int fd)
:eventLoop_(eventLoop), fd_(fd), events_(0), revents_(0),index_(-1), handling_(false){

    
}

bool Channel::construct_two(){
    if(!eventLoop_ || fd_ < 0){
        return false;
    }
    return true;
}

Channel *Channel::construct_channel(EventLoop *eventLoop, int fd){
    Channel *ret = new Channel(eventLoop, fd);  
    if(!ret || !ret->construct_two()){
        if(ret){
            delete ret;
        }
        return nullptr;
    }
    return ret;
}
Channel::~Channel(){
    if(handling_)
        LOG_ERROR << "Channel::~Channel()" << endl;
}

void Channel::set_read_callback(const EventCallback& cb){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::set_read_callback, this, cb));
    }
    readCallback_ = cb;
}

void Channel::set_write_callback(const EventCallback& cb){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::set_write_callback, this, cb));
    }
    writeCallback_ = cb;
}

void Channel::set_error_callback(const EventCallback& cb){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::set_error_callback, this, cb));
    }
    errorCallback_ = cb;
}

int Channel::fd() const{
    return fd_;
}

void Channel::enable_read(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::enable_read, this));
    }
    events_ |= kReadEvent_;
    update();

}

void Channel::disable_read(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::disable_read, this));
    }
    events_ &= ~kReadEvent_;
    revents_ &= ~kReadEvent_;
    update();
}

void Channel::enable_write(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::enable_write, this));
    }
    events_ |= kWriteEvent_;
    update();

}
void Channel::disable_write(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::disable_write, this));
    }
    events_ &= ~kWriteEvent_;
    revents_ &= ~kWriteEvent_;
    update();

}

void Channel::enable_all(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::enable_all, this));
    }
    enable_read();
    enable_write();
}

void Channel::disable_all(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::disable_all, this));
    }
    disable_read();
    disable_write();
}

int Channel::index() const{
    return index_;
}

void Channel::set_index(int index){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::set_index, this, index));
    }
    index_ = index;
}

EventLoop* Channel::owner_loop() const{
    return eventLoop_;
}

void Channel::handle_event(Timestamp time){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::handle_event, this, time));
    }
    handling_ = true;
    if(revents_ & POLLNVAL){
        LOG_WARN << "channel::handle_event() POLLNVAL";
    }
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallback_)
            closeCallback_(time);
    }
    if(revents_ & EPOLLERR){
        if(errorCallback_)
            errorCallback_(time);
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(readCallback_)
            readCallback_(time);
    }
    if(revents_ & EPOLLOUT){
        if(writeCallback_)
            writeCallback_(time);
    }
    handling_ = false;
}

bool Channel::is_none_event() const{
    return events_ == Channel::kNoneEvent_;
}

void Channel::update(){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::update, this));
    }
    eventLoop_->update_channel(this);
}

void Channel::set_revents(int revent){
    if(!eventLoop_->is_in_loop_thread()){
        eventLoop_->run_in_loop(std::bind(&Channel::set_revents, this, revent));
    }
    revents_ = revent;
}

unsigned int Channel::events() const{
    return events_;
}

bool Channel::is_writing() const{
    return events_ & kWriteEvent_;
}

}
