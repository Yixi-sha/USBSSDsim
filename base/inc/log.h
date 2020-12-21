#ifndef __EDEN_LOG_H__
#define __EDEN_LOG_H__

#include <memory>
#include <iostream>
#include <fstream>

#include "object.h"
#include "noncopyable.h"
#include "fixedBuffer.h"
#include "mutex.h"

namespace EDEN{

#define err(s) do{\
    std::cout << s << std::endl;\
    std::cout << __FILE__ <<": "<< __LINE__ << std::endl;\
    exit(1);\
}while(0)

#define err_msg(s) do{  \
    if(err_flag == 1){ \
        syslog(LOG_ERR,s); \
        syslog(LOG_ERR," %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    }else{ \
        printf(s); \
        printf(" %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    } \
}while(0)


class LogOut : public Noncopyable{
private:
    Mutex mutex_;
    std::unique_ptr<std::ofstream> upOut_;
public:
    LogOut(const char *outPath = NULL){
        if(outPath){
            upOut_.reset(new std::ofstream(outPath));
            if(!upOut_->is_open()){
                std::cout << "open fail!!!!" << endl;
                abort();
            }
        }
    }
    void write(std::string s){
        MutexLockGuard guard(mutex_);
        if(upOut_){
            *upOut_ << s;
            upOut_->flush();
        }else{
            std::cout << s;
            std::cout.flush();
        }
        
    }
};


class Log : public Noncopyable{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };
    static const char *LOG_PATH;
private:
    enum LogLevel logLevel_;

    const char* file_;
    int line_;
    LogLevel level_;
    const char* func_;
    FixedBuffer<kSmallBuffer> buf_;
private:

public:
    Log(const char* file, int line, const char* outPath = nullptr):
    file_(file), line_(line), func_(nullptr), level_(TRACE){
       
    }
    Log(const char* file, int line, LogLevel level, const char* outPath = nullptr):
    file_(file), line_(line), func_(nullptr), level_(level){
        
    }
    Log(const char* file, int line, const char* func, LogLevel level, const char* outPath = nullptr):
    file_(file), line_(line), func_(func), level_(level){
        
    }
    ~Log();

    FixedBuffer<kSmallBuffer>& stream();
    
};



#define LOG_TRACE Log(__FILE__, __LINE__, __func__, Log::TRACE, nullptr).stream()

#define LOG_DEBUG_MY Log(__FILE__, __LINE__, __func__, Log::DEBUG, nullptr).stream()

#define LOG_INFO_MY Log(__FILE__, __LINE__, __func__, Log::INFO, nullptr).stream()

#define LOG_WARN Log(__FILE__, __LINE__, __func__, Log::WARN, nullptr).stream()

#define LOG_ERROR Log(__FILE__, __LINE__, __func__, Log::ERROR, nullptr).stream()

#define LOG_FATAL Log(__FILE__, __LINE__, __func__, Log::FATAL, nullptr).stream()

}


#endif