#include <stdint.h>
#include "drivers/ll/stm32f0xx.h"
#include "drivers/ll/stm32f0xx_ll_rcc.h"
#include "drivers/ll/stm32f0xx_ll_gpio.h"
#include "drivers/ll/stm32f0xx_ll_bus.h"
#include "drivers/ll/stm32f0xx_ll_tim.h"
#include "drivers/ll/stm32f0xx_ll_dma.h"

//---------------
// RCC Registers
//---------------

//#define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)0x40021000U // Clock Control Register
//#define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)0x40021004U // PLL Configuration Register
//#define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)0x40021014U // AHB1 Peripheral Clock Enable Register
//#define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)0x4002102CU // Clock configuration register 2
//
////----------------
//// GPIO Registers
////----------------
//
//#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)0x48000800U // GPIO port mode register
//#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)0x48000804U // GPIO port output type register

//------
// Main
//------

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND (CPU_FREQENCY/1000U)

void ClockingInit()
{
    // (1) Clock HSE and wait for oscillations to setup.
    //while ((*REG_RCC_CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

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

#define rcc_config ClockingInit

#if 0
void GpioInit()
{
    // (1) Enable GPIOC clocking:
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOC);

    // (2) Configure PC8 mode and type:
    LL_GPIO_SetPinMode( GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType( GPIOC, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);

    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode( GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7( GPIOA, LL_GPIO_PIN_5, LL_GPIO_AF_2);

    /*
     * Setup timer to output compare mode
     */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_SetPrescaler( TIM2, 479);
    LL_TIM_SetAutoReload( TIM2, 999);
    LL_TIM_OC_SetCompareCH1( TIM2, 15);
    LL_TIM_CC_EnableChannel( TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_SetPolarity( TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
    //LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_TOGGLE);
    LL_TIM_OC_SetMode( TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_SetCounterMode( TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_CC1( TIM2);
    LL_TIM_EnableCounter( TIM2);
}
#endif


/*
 * Clock on GPIOC and set one led
 */
static void gpio_config(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
    return;
}

/*
 * Configure timer to output compare mode
 */
static void timers_config(void)
{
    /*
     * Configure output channel
     */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_5, LL_GPIO_AF_2);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_1, LL_GPIO_AF_2);

#if 0
    /*
     * Setup timer to output compare mode
     */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_SetPrescaler(TIM2, 479);
    LL_TIM_SetAutoReload(TIM2, 999);
    LL_TIM_OC_SetCompareCH1(TIM2, 800);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
    //LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_TOGGLE);
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_EnableCounter(TIM2);
    /*
     * Setup NVIC
     */
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
#endif
    return;
}

static uint32_t gBuffer[] = {
    100,
    200,
    300,
    400,
    500,
    600,
    700,
    800,
    900};

const  uint16_t kBufferSize = sizeof( gBuffer) / sizeof(uint32_t);


/*
 * Configure timer to output compare mode
 */
static void
DMA_Test()
{

    /*
     * Setup timer to output compare mode
     */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_TIM2);
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_DMA1);

    // The first one
    //LL_DMA_DisableChannel( DMA1, LL_DMA_CHANNEL_3);

    LL_DMA_SetDataTransferDirection( DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    //LL_DMA_SetMemoryAddress( DMA1, LL_DMA_CHANNEL_2, (uint32_t)(gBuffer));
    LL_DMA_SetMemoryAddress( DMA1, LL_DMA_CHANNEL_2, (uint32_t)gBuffer);
    LL_DMA_SetMemoryIncMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetMemorySize( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_WORD);
    LL_DMA_SetMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_CIRCULAR);

    LL_DMA_SetPeriphAddress( DMA1, LL_DMA_CHANNEL_2, (uint32_t)(&TIM2->CCR2));
    LL_DMA_SetPeriphIncMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_NOINCREMENT);
    LL_DMA_SetPeriphSize( DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_WORD);

    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_2, kBufferSize);

    NVIC_EnableIRQ( DMA1_Channel2_3_IRQn);
    NVIC_SetPriority( DMA1_Channel2_3_IRQn, 0);

    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_SetPrescaler( TIM2, 47999);
    LL_TIM_SetAutoReload( TIM2, 999);
    LL_TIM_SetCounterMode( TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_UPDATE( TIM2);
    LL_TIM_EnableDMAReq_UPDATE( TIM2);
    LL_TIM_CC_SetDMAReqTrigger( TIM2, LL_TIM_CCDMAREQUEST_UPDATE);

    LL_TIM_OC_SetCompareCH2( TIM2, 0);
    LL_TIM_OC_SetPolarity( TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetMode( TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);

    LL_DMA_ClearFlag_TC2( DMA1);
    LL_DMA_ClearFlag_HT2( DMA1);

    LL_DMA_EnableIT_TC( DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_EnableIT_HT( DMA1, LL_DMA_CHANNEL_2);

    /*
     * Setup NVIC
     */
    NVIC_EnableIRQ( TIM2_IRQn);
    NVIC_SetPriority( TIM2_IRQn, 1);

    LL_DMA_EnableChannel( DMA1, LL_DMA_CHANNEL_2);
    LL_TIM_CC_EnableChannel( TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_EnableCounter( TIM2);

    return;
}

extern "C"
void TIM2_IRQHandler(void)
{
    //static int i = 0;
    if ( LL_TIM_IsActiveFlag_UPDATE( TIM2) )
    {
//        LL_TIM_OC_SetCompareCH2( TIM2, gBuffer[i]);
//        i = (i + 1) % kBufferSize;
//
//        LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
    }

    //LL_TIM_ClearFlag_CC1(TIM2);
    //LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
    LL_TIM_ClearFlag_UPDATE( TIM2);
}

extern "C"
void DMA1_Channel2_3_IRQHandler(void)
{

    if ( LL_DMA_IsActiveFlag_TC2( DMA1) )
    {
        //LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_9);
    }

    //LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_9);
    LL_DMA_ClearFlag_TE2( DMA1);
    LL_DMA_ClearFlag_TC2( DMA1);
    LL_DMA_ClearFlag_HT2( DMA1);
    //LL_TIM_ClearFlag_CC1(TIM2);
}

int main(void)
{
    rcc_config();
    gpio_config();
    timers_config();

    //LL_GPIO_TogglePin( GPIOC, LL_GPIO_PIN_8);

    //LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
    DMA_Test();
    //for ( int i = 0; i < 12312320; i++ )
    //{
    //}

    //LL_TIM_OC_SetCompareCH1(TIM2, 40);
    while (1)
    {
    }
    return 0;
}
