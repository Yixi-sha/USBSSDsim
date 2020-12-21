#ifndef __EDEN_FIXEDBUF_H__
#define __EDEN_FIXEDBUF_H__

#include <cstring>
#include <string>
#include <iostream>


namespace EDEN{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;
const char endl = '\n';

template <int SIZE>
class FixedBuffer : public Noncopyable{
private:
    char data_[SIZE];
    char *cur_;

    const char* end() const { 
        return data_ + sizeof(data_); 
    }
public:
    FixedBuffer() : cur_(data_){

    }

    size_t avail() const { 
        return static_cast<int>(end() - cur_); 
    }

    size_t append(const char * buf, size_t len){
        int nowavail = avail();
        len = nowavail < len ? nowavail : len; 
        memcpy(cur_, buf, len);
        cur_ += len;
        return len;
    }

    const char* data() const { 
        return data_; 
    }
    
    int length() const { 
        return static_cast<int>(cur_ - data_); 
    }

    void add(size_t len) { 
        cur_ += len; 
    }

    void reset() { 
        cur_ = data_; 
    }

    void bzero() { 
        bzero(data_, sizeof(data_)); 
    }

    std::string toString() const { 
        return std::string(data_, length()); 
    }

    FixedBuffer& operator<<(bool v){
        append(v ? "1" : "0", 1);
        return *this;
    }

    FixedBuffer& operator<<(short s){
        return operator<<(std::to_string(s));
    }

    FixedBuffer& operator<<(unsigned short s){
        return operator<<(std::to_string(s));
    }

    FixedBuffer& operator<<(int i){
        return operator<<(std::to_string(i));
    }

    FixedBuffer& operator<<(unsigned int i){
        return operator<<(std::to_string(i));
    }

    FixedBuffer& operator<<(long l){
        return operator<<(std::to_string(l));
    }

    FixedBuffer& operator<<(unsigned long l){
        return operator<<(std::to_string(l));
    }

    FixedBuffer& operator<<(long long ll){
        return operator<<(std::to_string(ll));
    }

    FixedBuffer& operator<<(unsigned long long ll){
        return operator<<(std::to_string(ll));
    }

    FixedBuffer& operator<<(double d){
        return operator<<(std::to_string(d));
    }

    FixedBuffer& operator<<(float v){
        return operator<<(std::to_string(v));
    }

    FixedBuffer& operator<<(char v){
        append(&v, 1);
        return *this;
    }
    
    FixedBuffer& operator<<(const char* str){
        if(str){
            append(str, strlen(str));
        }else{
            append("(null)", 6);
        }
        return *this;
    }

    FixedBuffer& operator<<(const unsigned char* str){
        return operator<<(reinterpret_cast<const char*>(str));
    }

    FixedBuffer& operator<<(const std::string& v){
        append(v.c_str(), v.size());
        return *this;
    }


};

}



#endif