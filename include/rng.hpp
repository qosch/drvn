#pragma once
#include <stdint.h>

class RandomNumberGenerator
{
    public:
    void init(void);
    uint32_t get(void);
};