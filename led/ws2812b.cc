#include "ws2812b.h"
#include "../drivers/ll/stm32f0xx.h"
#include "../drivers/ll/stm32f0xx_ll_rcc.h"
#include "../drivers/ll/stm32f0xx_ll_gpio.h"
#include "../drivers/ll/stm32f0xx_ll_bus.h"
#include "../drivers/ll/stm32f0xx_ll_tim.h"
#include "../drivers/ll/stm32f0xx_ll_dma.h"

//
// Init PwmBuffer.
//
void
LED_PwmBuffer::init()
{
    for ( uint32_t i = kResetCycles; i != kBufferLength; i++ )
    {
        data_[i] = getPulseWidth( 0);
    }
}

//
// Set LED color.
//
void
LED_PwmBuffer::writeLED( LED_Color color,    // color
                         uint32_t led_index) // led index (from 0 to kLedsCount)
{
    if ( led_index >= kLedsCount )
    {
        return;
    }

    if ( color.blue >= 8 )
        color.blue = 8;
    if ( color.red >= 8)
        color.red = 8;
    if ( color.green >= 8)
        color.green = 8;

    //
    // W2812b color is composition of 24bit data:
    // G7 G6 G5 G4 G3 G2 G1 G0 R7 R6 R5 R4 R3 R2 R1 R0 B7 B6 B5 B4 B3 B2 B1 B0
    //
    // We have to follow the order of GRB to sent data and the high bit sent at first.
    //
    uint32_t color_pack = (color.blue << 0U)
                          | (color.red << 8U)
                          | (color.green << 16U);

    // FIXME: ProtectMask -> clamp
    //color_pack = color_pack & kProtectMask;

    for ( uint32_t i = 0; i != kBitsInColor; i++ )
    {
        data_[kResetCycles + led_index * kBitsInColor + i] = getPulseWidth( color_pack & (1U << (kBitsInColor - i - 1U)));
    }
}

//
// Start transfer and PWM generation.
//
void
LED_StartTransfer( LED_PwmBuffer* buffer) // PWM buffer
{
    //
    // Set up GPIO PA1 pin as PWM output.
    // Table 14. Alternate functions selected through GPIOA_AFR registers for port A
    // Docs: docs/stm32f051_family.pdf
    //
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode( GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7( GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_0);

    LL_APB1_GRP2_EnableClock( LL_APB1_GRP2_PERIPH_TIM15);
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_DMA1);

    LL_TIM_DisableCounter( TIM15);
    LL_DMA_DisableChannel( DMA1, LL_DMA_CHANNEL_5);
    LL_TIM_CC_DisableChannel( TIM15, LL_TIM_CHANNEL_CH1);

    //
    // Set up DMA and PWM communication:
    //
    // TIM15->ARR Update --> TIM DMA request --> DMA
    //                                             | (transfer and increment)
    // PwmBuffer: [18][18][18][18][18][29][18][29][29][18][18][18]
    //                                             |
    // TIM15->CCR1 = pulse width  <----------------/
    //           \-----> PWM generation
    //
    LL_DMA_SetDataTransferDirection( DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetMemoryAddress( DMA1, LL_DMA_CHANNEL_5, (uint32_t)buffer->data());
    LL_DMA_SetMemoryIncMode( DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetMemorySize( DMA1, LL_DMA_CHANNEL_5, LL_DMA_MDATAALIGN_HALFWORD);

    LL_DMA_SetMode( DMA1, LL_DMA_CHANNEL_5, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_5, buffer->length());

    LL_DMA_SetPeriphAddress( DMA1, LL_DMA_CHANNEL_5, (uint32_t)(&TIM15->CCR1));
    LL_DMA_SetPeriphIncMode( DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_NOINCREMENT);
    LL_DMA_SetPeriphSize( DMA1, LL_DMA_CHANNEL_5, LL_DMA_PDATAALIGN_HALFWORD);

    LL_TIM_SetAutoReload( TIM15, LED_TransferTime::Period - 1);
    LL_TIM_SetCounterMode( TIM15, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_UPDATE( TIM15);

    //
    // Table 29. Summary of the DMA requests for each channel
    // on STM32F03x, STM32F04x and STM32F05x devices
    // Docs: docs/stm32f0xx_rm.pdf
    //
    LL_TIM_EnableDMAReq_UPDATE( TIM15);
    LL_TIM_CC_SetDMAReqTrigger( TIM15, LL_TIM_CCDMAREQUEST_UPDATE);

    LL_TIM_OC_SetPolarity( TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetMode( TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH1( TIM15, 0);

    LL_DMA_EnableChannel( DMA1, LL_DMA_CHANNEL_5);
    LL_TIM_EnableAllOutputs( TIM15);
    LL_TIM_CC_EnableChannel( TIM15, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter( TIM15);
}


