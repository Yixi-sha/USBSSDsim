#include "../inc/log.h"
#include "../inc/timestamp.h"

namespace EDEN{
    
static LogOut TRACE_l(nullptr);
static LogOut DEBUG_l(nullptr);
static LogOut INFO_l(nullptr);
static LogOut WARN_l(nullptr);
static LogOut ERROR_l(nullptr); 
static LogOut FATAL_l(nullptr);


FixedBuffer<kSmallBuffer>& Log::stream(){
    buf_ << Timestamp::now().to_formatted_string() << " ";
    switch (level_){
    case TRACE:
            buf_ << "TRACE";
            break;
        case DEBUG:
            buf_ << "DEBUG";
            break;
        case INFO:
            buf_ << "INFO";
            break;
        case WARN:
            buf_ << "WARN";
            break;
        case ERROR:
            buf_ << "ERROR";
            break;
        case FATAL:
            buf_ << "FATAL";
            break;    
        default:
            break;
    }
    buf_ << ": "<<file_ << ": " << line_ << " ";
    if(func_)
        buf_ << func_<<": ";
    buf_ <<"\t";
    return buf_;
}



Log::~Log(){
    switch (level_){
    case TRACE:
            TRACE_l.write(buf_.toString());
            break;
        case DEBUG:
            DEBUG_l.write(buf_.toString());
            break;
        case INFO:
            INFO_l.write(buf_.toString());
            break;
        case WARN:
            WARN_l.write(buf_.toString());
            break;
        case ERROR:
            ERROR_l.write(buf_.toString());
            break;
        case FATAL:
            FATAL_l.write(buf_.toString());
            break;    
        default:
            break;
    }
}

}