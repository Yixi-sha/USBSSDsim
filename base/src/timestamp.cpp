#include "../inc/timestamp.h"

#include <utility>

#include <cstdio>
extern "C"{
#include <sys/time.h>
#include <inttypes.h>
}

namespace EDEN{

int64_t Timestamp::sToUs = 1000000;

Timestamp::Timestamp():us_(0){

}

Timestamp::Timestamp(int64_t us):us_(us){

}

std::string Timestamp::to_string() const{
    char buf[32];
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", us_ / sToUs, us_ % sToUs);
    return buf;
}

std::string Timestamp::to_formatted_string() const{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(us_ / sToUs);
    int microseconds = static_cast<int>(us_ % sToUs);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
        microseconds);
    return buf;
}

void Timestamp::swap(Timestamp& ts){
    std::swap(us_, ts.us_);
}

bool Timestamp::is_valid() const{
    return us_ > 0;
}

int64_t Timestamp::us() const{
    return us_;
}

Timestamp Timestamp::now(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return Timestamp(tv.tv_sec * sToUs + tv.tv_usec);
}

Timestamp Timestamp::invaild(){
    return Timestamp();
}

bool Timestamp::operator > (const Timestamp& ts) const{
    return us_ > ts.us_;
}

bool Timestamp::operator >= (const Timestamp& ts) const{
    return us_ >= ts.us_;
}

bool Timestamp::operator < (const Timestamp& ts) const{
    return !operator>=(ts);
}

bool Timestamp::operator <= (const Timestamp& ts) const{
    return !operator>(ts);
}

bool Timestamp::operator == (const Timestamp& ts) const{
    return us_ == ts.us_;
}

double Timestamp::diff(Timestamp& ts){
    int64_t dif = us_ - ts.us_;
    return static_cast<double>(dif) / static_cast<double>(sToUs);
}

Timestamp Timestamp::add_time(double seconds){
    return Timestamp(us_ + static_cast<int64_t>(seconds * sToUs));
}

} 