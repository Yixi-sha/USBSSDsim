#ifndef __EDEN_THREAD_H__
#define __EDEN_THREAD_H__

#include "noncopyable.h"
#include <functional>

extern "C"{
#include <pthread.h>
#include <unistd.h>
}

namespace EDEN{   
     
class Thread : public  Noncopyable{
private:
    std::function<void*(void*)> func_;
    pid_t tid_;
    pthread_t pthreadID_;
    void *argv_;
    bool start_;
public:
    Thread(const std::function<void*(void*)> &func, void *argv = nullptr)
    : func_(func), tid_(0), argv_(argv), start_(false){
        
    }
    Thread(void* (*func)(void*), void *argv = nullptr)
    : func_(func), tid_(0), argv_(argv){
        
    }
    int start();
    pthread_t tid(){
        return gettid();
    }
    int join(void **status = nullptr){
        if(status == NULL){
            return pthread_join(pthreadID_, nullptr);
        }else{
            return pthread_join(pthreadID_, status);
        }
        start_ = false;
    }
    bool is_start(){
        return start_;
    }
};
}

#endif