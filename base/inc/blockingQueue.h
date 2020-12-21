#ifndef __MY_BLOCKINGQUEUE_H__
#define __MY_BLOCKINGQUEUE_H__

#include "mutex.h"
#include "condition.h"
#include "noncopyable.h"
#include <queue>


namespace EDEN{

template <typename T>
class BlockingQueue : public Noncopyable{
private:
    int limit_;
    int now_;
    Mutex mutex_;
    Condition conditionD_;
    Condition conditionE_;
    queue<T> q_;
public:
    BlockingQueue(int limit = 10) 
    : limit_(limit), now_(0), conditionD_(mutex_), conditionE_(mutex_){

    }

    void enqueue(T item){
        MutexLockGuard guard(mutex_);
        while(q_.size() == limit_){
            conditionE_.wait();
        }
        q_.push(item);
        conditionD_.notify();
    }

    T dequeue(){
        MutexLockGuard guard(mutex_);
        while(q_.size() == 0){
            conditionD_.wait();
        }
        T ret = q_.front();
        q_.pop();

        conditionE_.notify();
        return ret;
    }

};

}

#endif