#include "../inc/poller.h"

#include "eventloop.h"
#include "channel.h"

extern "C"{
#include <strings.h>
}


namespace EDEN{

Poller::Poller(EventLoop *eventLoop)
:eventLoop_(eventLoop), pollReventsP_(nullptr), pollReventsNum_(0){

}

bool Poller::construct_two(){
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
    if(epollfd_ < 0){
        return false;
    }
    return true;
    
}

Poller* Poller::construct_poller(EventLoop *eventLoop){
    Poller* ret = new Poller(eventLoop);
    if(!ret || !ret->construct_two()){
        if(ret)
            delete ret;
        return nullptr;
    }
    return ret;
}

Poller::~Poller(){
    if(pollReventsP_){
        delete[] pollReventsP_;
    }
}

bool Poller::is_in_loop_thread() const{
    return eventLoop_->is_in_loop_thread();
}

Timestamp Poller::poll(int timeOutMs, std::vector<Channel*> *activeChannels){
    if(activeChannels == nullptr){
        LOG_ERROR << "activeChannels == nullptr" << endl;
        return Timestamp::now();
    }
    if(pollReventsP_ == nullptr || pollReventsNum_ < pollEvents_.size()){
        if(pollReventsP_){
            delete[] pollReventsP_;
        }
        pollReventsP_ = new epoll_event[pollEvents_.size()];
        if(pollReventsP_ == nullptr){
            LOG_ERROR << "new epoll_event[pollEvents_.size()]" << endl;
            pollReventsNum_ = 0;
            return Timestamp::now();
        }
        pollReventsNum_ = pollEvents_.size();
    }

    int eventsNum = ::epoll_wait(epollfd_, pollReventsP_, pollReventsNum_, timeOutMs);
    Timestamp now(Timestamp::now());
    
    if(eventsNum > 0){
        //LOG_TRACE << eventsNum << " events happend" << endl;
        fill_active_channels(eventsNum, activeChannels);
    }else if(eventsNum == 0){
        //LOG_TRACE << " nothing happend " << gettid()<< endl;
    }else {
        //LOG_ERROR << "Poller::poll" << endl;
    }
    return now;
}

void Poller::fill_active_channels(int eventNum, std::vector<Channel*> *activeChannels){
    for(int i = 0; i < eventNum; ++i){
        std::map<int ,Channel*>::const_iterator ch = channels_.find(pollReventsP_[i].data.fd);
        Channel *channel = ch->second;
        if(pollReventsP_[i].data.fd != channel->fd()){
            err("pollReventsP_[i].data.fd != channel->fd()");
        }
        channel->set_revents(pollReventsP_[i].events);
        activeChannels->push_back(channel);
    }
}

bool Poller::update_channel(Channel* channel){
    std::cout << "update_channel " << gettid()<< endl;
    if(!channel){
        return false;
    }
    if(!is_in_loop_thread()){
        LOG_ERROR << "Poller::update_channel is_in_loop_thread()" << endl;
        return false;
    }
        
    if(channel->index() == -1){
        if(channels_.find(channel->fd()) != channels_.end()){
            LOG_ERROR << "Poller::update_channel  channels_.find(channel->fd) != channels_.end()" << endl;
            LOG_ERROR << channel->index() << endl;
            return false;
        }
        struct epoll_event EPEvent;
        bzero(&EPEvent, sizeof(EPEvent));
        EPEvent.data.fd = channel->fd();
        EPEvent.events = channel->events();
        pollEvents_.push_back(EPEvent);
        channel->set_index(static_cast<int>(pollEvents_.size() - 1));
        channels_[EPEvent.data.fd] = channel;

        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->fd(), &EPEvent) == -1){
            LOG_ERROR << "Poller::update_channel  epoll_ctl(ADD) == -1" << endl;
            return false;
        }

    }else{
        int idx = channel->index();
        int preFd, nowFd;
        if(channels_.find(channel->fd()) == channels_.end()){
            LOG_ERROR <<  "Poller::update_channel  channels_.find(channel->fd) == channels_.end()" << endl;
            return false;
        }
        if(idx < 0 || idx >= pollEvents_.size()){
            LOG_ERROR << "Poller::update_channel  idx < 0 || idx >= pollfds_.size()" << endl;
            return false;
        }
            
        if(channels_[channel->fd()] != channel){
            LOG_ERROR <<"Poller::update_channel  channels_[channel->fd()] != channel" << endl;
            return false;
        }
        struct epoll_event &EPEvent = pollEvents_[idx];
        if(EPEvent.data.fd != (-channel->fd() - 1) && EPEvent.data.fd != channel->fd()){
            LOG_ERROR << "Poller::update_channel EPEvent.fd != -1 || EPEvent.fd != channel->fd()" << endl;
            return false;
        }
        
        preFd = EPEvent.data.fd;

        EPEvent.events = channel->events();
        EPEvent.data.fd = channel->fd();
        if(channel->is_none_event()){
            EPEvent.data.fd = -channel->fd() - 1;
        }
        nowFd = EPEvent.data.fd;
        if(preFd >= 0 && nowFd >= 0){
            if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, channel->fd(), &EPEvent) == -1){
                LOG_ERROR << "Poller::update_channel  epoll_ctl(MOD) == -1" << endl;
                return false;
            }
        }else if(preFd < 0 && nowFd >= 0){
            if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->fd(), &EPEvent) == -1){
                LOG_ERROR << "Poller::update_channel  epoll_ctl(ADD) == -1" << endl;
                return false;
            }
        }else if(preFd >= 0 && nowFd < 0){
            EPEvent.data.fd = channel->fd();
            if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->fd(), &EPEvent) == -1){
                EPEvent.data.fd = nowFd;
                LOG_ERROR << "Poller::update_channel  epoll_ctl(DEL) == -1" << endl;
                return false;
            }
            EPEvent.data.fd = nowFd;
        }
    }
    return true;
}

bool Poller::remove_channel(Channel* channel){
    if(!eventLoop_->is_in_loop_thread()){
        LOG_ERROR << "Poller::remove_channel !eventLoop_->is_in_loop_thread())" << endl;
        return false;
    }
    if(channels_.find(channel->fd()) == channels_.end()){
        LOG_ERROR << "Poller::remove_channel channels_.find(channel->fd()) == channels_.end()" << endl;
        return false;
    }
    if(!channel->is_none_event()){
        LOG_ERROR << "Poller::remove_channel !channel->is_none_event()" << endl;
        return false;
    }
    int idx = channel->index();
    if(idx < 0 || idx >= pollEvents_.size()){
        LOG_ERROR << "Poller::remove_channel idx < 0 || idx >= pollfds_.size()" << endl;
        return false;
    }
    struct epoll_event &EPEvent = pollEvents_[idx];
    if(EPEvent.data.fd != channel->fd() && EPEvent.data.fd != (-channel->fd() - 1)){
        LOG_ERROR << "Poller::remove_channel pfd.fd  != channel->fd()" << endl; 
        return false;
    }
    int n = channels_.erase(channel->fd());
    if(idx != pollEvents_.size() - 1){
        int endfd = pollEvents_.back().data.fd;
        iter_swap(pollEvents_.begin() + idx, pollEvents_.end() - 1);
        if(endfd < 0){
            endfd = -endfd - 1;
        }
        channels_[endfd]->set_index(idx);
    }
    EPEvent = pollEvents_[idx];
    if(EPEvent.data.fd < 0){
        EPEvent.data.fd = channel->fd();
    }
    if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->fd(), &EPEvent) == -1){
        LOG_ERROR << "Poller::update_channel  epoll_ctl(DEL) == -1" << endl;
        return false;
    }
    pollEvents_.pop_back();
    channels_[channel->fd()] = nullptr;
    return true;
}

}