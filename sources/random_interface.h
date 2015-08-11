#ifdef _MSC_VER
#pragma once
#endif

#ifndef _RANDOM_INTERFACE_H_
#define _RANDOM_INTERFACE_H_

#include "readonly_interface.h"

class RandomSequence;

class IRandom : private IReadOnly {
public:
    IRandom() {}
    ~IRandom() {}
    virtual void request(int n, RandomSequence* rseq) = 0;
};

#endif  // _RANDOM_INTERFACE_H_