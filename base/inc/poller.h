#ifndef __EDEN_POLLER_H__
#define __EDEN_POLLER_H__

#include "channel.h"
#include "timestamp.h"

#include <vector>
#include <map>

extern "C"{
#include <sys/epoll.h>
#include <poll.h>
}

namespace EDEN {

class Poller : public Noncopyable{
private:
    EventLoop *eventLoop_;
    int epollfd_;
    
    std::vector<struct epoll_event> pollEvents_;
    std::map<int, Channel*> channels_;
    epoll_event *pollReventsP_;
    int pollReventsNum_;

    Poller(EventLoop *eventLoop);
    bool construct_two();

    void fill_active_channels(int eventNum, std::vector<Channel*> *activeChannels);
   

public:
    static Poller *construct_poller(EventLoop *eventLoop);

    ~Poller();

    Timestamp poll(int timeOutMs, std::vector<Channel*> *activeChannels);
    bool update_channel(Channel *channel);
    bool is_in_loop_thread() const;
    bool remove_channel(Channel* channel);
};

}

#endif