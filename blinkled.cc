#include <stdint.h>
#include "drivers/ll/stm32f0xx.h"
#include "drivers/ll/stm32f0xx_ll_rcc.h"
#include "drivers/ll/stm32f0xx_ll_gpio.h"
#include "drivers/ll/stm32f0xx_ll_bus.h"
#include "drivers/ll/stm32f0xx_ll_tim.h"
#include "drivers/ll/stm32f0xx_ll_dma.h"
#include "drivers/ll/stm32f0xx_ll_system.h"
#include "led/ws2812b.h"
#include "asp/adc.h"

void
FW_ClockingInit()
{
    LL_FLASH_SetLatency( LL_FLASH_LATENCY_1);

    // (1) Clock HSE and wait for oscillations to setup.
    LL_RCC_HSE_Enable();
    while ( !LL_RCC_HSE_IsReady() )
    { /* Do nothing */ }

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    // (3) Select PREDIV output as PLL input (4 MHz):
    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    LL_RCC_PLL_ConfigDomain_SYS( LL_RCC_PLLSOURCE_HSE_DIV_2,
                                 LL_RCC_PLL_MUL_12);

    // (5) Enable PLL:
    LL_RCC_PLL_Enable();
    while ( !LL_RCC_PLL_IsReady() )
    { /* Do nothing */ }

    // (6) Configure AHB frequency to 48 MHz:
    LL_RCC_SetAHBPrescaler( LL_RCC_SYSCLK_DIV_1);

    // (7) Select PLL as SYSCLK source:
    LL_RCC_SetSysClkSource( LL_RCC_SYS_CLKSOURCE_PLL);
    while ( LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL )
    { /* Do nothing */ }

    // (8) Set APB frequency to 48 MHz
    LL_RCC_SetAPB1Prescaler( LL_RCC_APB1_DIV_1);
}

static LED_PwmBuffer gPwmBuffer;
static ASP_AdcBuffer gAdcBuffer;

extern "C"
void DMA1_Channel1_IRQHandler( void)
{
    LL_GPIO_TogglePin( GPIOC, LL_GPIO_PIN_8);

    uint32_t mean = 0;
    const uint16_t* data = gAdcBuffer.data();
    for ( uint32_t i = 0; i != gAdcBuffer.length(); ++i )
    {
        mean += data[i];
    }

    mean = mean / gAdcBuffer.length();

    static int32_t max_level = 0;
    if ( max_level <= 0 )
    {
        max_level = 1;
    }

    int32_t new_blink = ( LED_Stripe::LedsCount * mean ) / max_level;
    if ( new_blink >= (int32_t)LED_Stripe::LedsCount )
    {
        new_blink = LED_Stripe::LedsCount;
    }

    //
    // Running average filter
    //
    static int32_t n_blink = 0;
    n_blink += ( new_blink - n_blink ) >> 4;
    max_level += ( n_blink - max_level ) >> 8;

    for ( int32_t i = n_blink; i != LED_Stripe::LedsCount; ++i )
    {
        gPwmBuffer.writeLED( LED_Color{0, 0, 0}, i);
    }

    for ( int32_t i = 0; i != n_blink; ++i )
    {
        gPwmBuffer.writeLED( { 5, 0, 5}, i);
    }

    LL_DMA_ClearFlag_TC1( DMA1);
}

int main()
{

    FW_ClockingInit();

    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_GPIO_SetPinMode( GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode( GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);

    gPwmBuffer.init();
    gAdcBuffer.init();

    ASP_StartProcessing( &gAdcBuffer);
    LED_StartTransfer( &gPwmBuffer);

    for ( ;; )
    {
    }

    return 0;
}

