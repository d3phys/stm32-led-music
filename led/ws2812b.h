#pragma once
#include <stdint.h>

constexpr static const uint32_t AHB_Frequency_kHz = 48000;

struct LED_Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

//
// WS2812b datasheet time values:
// Docs: docs/ws2812b.pdf
//
struct LED_Stripe
{
    constexpr static const uint32_t LedsCount = 144;
};

//
// WS2812b datasheet time values:
// Docs: docs/ws2812b.pdf
//
struct LED_TransferTime
{
    // Reset cycle 50mu is small.
    constexpr static const uint32_t Reset_ns  = 75000U;
    constexpr static const uint32_t Period_ns =  1250U;
    constexpr static const uint32_t T0H_ns    =   400U;
    constexpr static const uint32_t T1H_ns    =   800U;
    constexpr static const uint32_t T0L_ns    =   850U;
    constexpr static const uint32_t T1L_ns    =   450U;
    constexpr static const uint32_t Error_ns  =   150U;

    constexpr static const uint32_t Frequency_kHz = ( 1000000U / Period_ns );

    constexpr static const uint16_t Period = ( AHB_Frequency_kHz / Frequency_kHz );
    constexpr static const uint16_t T0H    = ( Period * T0H_ns ) / Period_ns;
    constexpr static const uint16_t T0L    = ( Period * T0L_ns ) / Period_ns;
    constexpr static const uint16_t T1H    = ( Period * T1H_ns ) / Period_ns;
    constexpr static const uint16_t T1L    = ( Period * T1L_ns ) / Period_ns;

    constexpr static const uint32_t Error = (Error_ns * Period) / Period_ns;
    static_assert( Error > Period - (T1H + T1L), "Too big integer error..." );
};

class LED_PwmBuffer
{
public:
    constexpr static const uint32_t kResetCycles = LED_TransferTime::Reset_ns / LED_TransferTime::Period_ns;
    constexpr static const uint32_t kLedsCount = LED_Stripe::LedsCount;

    constexpr static const uint16_t kPulseWidth0 = LED_TransferTime::T0H - 1;
    constexpr static const uint16_t kPulseWidth1 = LED_TransferTime::T1H - 1;

    constexpr static const uint32_t kBitsInColor = 24;
    constexpr static const uint32_t kBufferLength = kResetCycles + kLedsCount * kBitsInColor;

    constexpr static const uint32_t kProtectMask = 0x0f0f0f;
    constexpr static const uint32_t kMaxColorValue = 16;

    void writeLED( LED_Color color, uint32_t position);

    uint16_t getPulseWidth( bool bit) { return bit ? kPulseWidth1 : kPulseWidth0; }

    void init();

    const uint16_t* data() { return data_; }
    uint32_t length() { return kBufferLength; }

private:
    uint16_t data_[kBufferLength] = {0};
};

void LED_StartTransfer( LED_PwmBuffer* buffer);
