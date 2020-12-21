#ifndef __EDEN_CONDITION_H__
#define __EDEN_CONDITION_H__

#include "noncopyable.h"
#include "mutex.h"

namespace EDEN{
class Condition : public Noncopyable{
private:
    Mutex &mutex_;
    pthread_cond_t cond_;

public:
    Condition(Mutex &mutex):mutex_(mutex){
        pthread_cond_init(&cond_, nullptr);
    }
    ~Condition(){
        pthread_cond_destroy(&cond_);
    }

    int wait(){
        return pthread_cond_wait(&cond_, mutex_.get_mutex());
    }
    int wait_for_time(int seconds){
        struct timespec time;
         clock_gettime(CLOCK_REALTIME, &time);
        time.tv_sec += seconds;
        return pthread_cond_timedwait(&cond_, mutex_.get_mutex(), &time);
    }
    int notify(){
        return pthread_cond_signal(&cond_);
    }

    int notify_all(){
        return pthread_cond_broadcast(&cond_);
    }
};

class CountDownLatch : Noncopyable{
private:
    Mutex mutex_;
    Condition cond_;
    int count_;
public:
    CountDownLatch(int count): cond_(mutex_), count_(count){

    }
    bool wait(){
        MutexLockGuard gurad(mutex_);
        if(count_ <= 0)
            return false;
        while(count_ > 0){
            cond_.wait();
        }
        return true;
    }

    bool countDown(){
        MutexLockGuard gurad(mutex_);
        if(count_ <= 0)
            return false;
        if(--count_ == 0){
            if(cond_.notify_all() != 0)
                return false;
        }
        return true;
    }
};

}

#endif