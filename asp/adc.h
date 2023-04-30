#pragma once
#include <stdint.h>

class ASP_AdcBuffer
{
public:
    constexpr static const uint32_t kBufferLength = 100;

    void init();
    const uint16_t* data() { return data_; }
    uint32_t length() { return kBufferLength; }

private:
    uint16_t data_[kBufferLength] = {0};
};

void ASP_StartProcessing( ASP_AdcBuffer* buffer);


