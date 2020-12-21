#ifndef __EDEN_NONCOPYABLE_H__
#define __EDEN_NONCOPYABLE_H__

#include "object.h"

namespace EDEN{

class Noncopyable : public Object{
public:
    Noncopyable() {}
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator = (const Noncopyable&) = delete;
};

}

#endif
