#ifndef __EDEN_TIMESTAMP_H__
#define __EDEN_TIMESTAMP_H__

#include <cstdint>
#include <utility> 
#include <string>
#include "object.h"

namespace EDEN{

class Timestamp : public Object{
private:
    int64_t us_;
public:
    Timestamp();
    explicit Timestamp(int64_t us);

    static int64_t sToUs;

    std::string to_string() const;
    std::string to_formatted_string() const;

    void swap(Timestamp& ts);
    bool is_valid() const;

    int64_t us() const;

    static Timestamp now();

    static Timestamp invaild();

    bool operator > (const Timestamp& ts) const;
    bool operator >= (const Timestamp& ts) const;
    bool operator < (const Timestamp& ts) const;
    bool operator <= (const Timestamp& ts) const;
    bool operator == (const Timestamp& ts) const;

    double diff(Timestamp& ts);
    Timestamp add_time(double seconds);
};
}

#endif