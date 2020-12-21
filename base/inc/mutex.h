#ifndef __EDEN_MUTEX_H__
#define __EDEN_MUTEX_H__

#include <iostream>

#include "noncopyable.h"

extern "C"{
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
}

#include <atomic>

namespace EDEN{
class Mutex : public Noncopyable{
private:
    pthread_mutex_t mutex_;
    pid_t holder_;
    std::atomic_int  locked_;
public:
    Mutex() : mutex_(PTHREAD_MUTEX_INITIALIZER), holder_(0), locked_(0){

    }
    int lock(){
        if(locked_ == 1 && gettid() == holder_)
            abort();
        int ret = pthread_mutex_lock(&mutex_);
        if(ret == 0){
            locked_ = 1;
            holder_ = gettid();
        }
        return ret;
    }
    int unlock(){
        holder_ = 0;
        locked_ = 0;
        return pthread_mutex_unlock(&mutex_);
    }
    bool is_hold_by_self(){
        return holder_ == gettid();
    }
    pthread_mutex_t *get_mutex(){
        return &mutex_;
    }
    bool if_locked(){
        return locked_;
    }
};

class MutexLockGuard : Noncopyable{
private:
    Mutex &mutex_;
public:
    MutexLockGuard(Mutex &mutex): mutex_(mutex){
        mutex_.lock();
    }
    ~MutexLockGuard(){
        mutex_.unlock();
    }
};


}


#endif