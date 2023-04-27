#include <stdint.h>
#include "drivers/ll/stm32f0xx.h"
#include "drivers/ll/stm32f0xx_ll_rcc.h"
#include "drivers/ll/stm32f0xx_ll_gpio.h"
#include "drivers/ll/stm32f0xx_ll_bus.h"
#include "drivers/ll/stm32f0xx_ll_tim.h"
#include "drivers/ll/stm32f0xx_ll_dma.h"

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

struct LED_Stripe
{
    static const uint32_t LedsCount = 30;
};

static const uint32_t AHB_Frequency_kHz = 48'000;

struct LED_TransferTime
{
    //
    // Datasheet edition
    //
    // Note: Reset cycle is 50mu but...
    static const uint32_t Reset_ns  = 75'000;
    static const uint32_t Period_ns =  1'250;
    static const uint32_t T0H_ns    =    400;
    static const uint32_t T1H_ns    =    800;
    static const uint32_t T0L_ns    =    850;
    static const uint32_t T1L_ns    =    450;
    static const uint32_t Error_ns  =    150;

    static const uint32_t Frequency_kHz = ( 1'000'000 / Period_ns );

    static const uint32_t Period = ( AHB_Frequency_kHz / Frequency_kHz );
    static const uint32_t T0H    = ( Period * T0H_ns ) / Period_ns;
    static const uint32_t T0L    = ( Period * T0L_ns ) / Period_ns;
    static const uint32_t T1H    = ( Period * T1H_ns ) / Period_ns;
    static const uint32_t T1L    = ( Period * T1L_ns ) / Period_ns;

    static const uint32_t Error = (Error_ns * Period) / Period_ns;
    static_assert( Error > Period - (T1H + T1L), "Too big integer error..." );
};

struct LED_Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class LED_PwmBuffer
{
public:
    static const uint32_t kResetCycles = LED_TransferTime::Reset_ns / LED_TransferTime::Period_ns;
    static const uint32_t kLedsCount = LED_Stripe::LedsCount;

    static const uint32_t kPulseWidth0 = LED_TransferTime::T0H - 1;
    static const uint32_t kPulseWidth1 = LED_TransferTime::T1H - 1;

    static const uint32_t kBitsInColor = 24;
    static const uint32_t kBufferLength = kResetCycles + kLedsCount * kBitsInColor;

    static const uint32_t kProtectMask = 0x0f0f0f;


    void writeLED( LED_Color color, uint32_t position);

    uint32_t getPulseWidth( bool bit) { return bit ? kPulseWidth1 : kPulseWidth0; }

    void init();

    const uint32_t* data() { return data_; }
    uint32_t length() { return kBufferLength; }

private:
    uint32_t data_[kBufferLength] = {0};
};


void
LED_PwmBuffer::init()
{
    for ( uint32_t i = kResetCycles; i != kBufferLength; i++ )
    {
        data_[i] = kPulseWidth0;
    }
}

void
LED_PwmBuffer::writeLED( LED_Color color,
                         uint32_t led_index)
{
    if ( led_index >= kLedsCount )
    {
        return;
    }

    //
    // W2812b color is composition of 24bit data:
    // G7 G6 G5 G4 G3 G2 G1 G0 R7 R6 R5 R4 R3 R2 R1 R0 B7 B6 B5 B4 B3 B2 B1 B0
    //
    // We have to follow the order of GRB to sent data and the high bit sent at first.
    //
    uint32_t color_pack = (color.blue << 0U)
                          | (color.red << 8U)
                          | (color.green << 16U);

    color_pack = color_pack & kProtectMask;

    for ( uint32_t i = 0; i != kBitsInColor; i++ )
    {
        data_[kResetCycles + led_index * kBitsInColor + i] = getPulseWidth( color_pack & (1U << (kBitsInColor - i - 1U)));
    }
}

static LED_PwmBuffer gBuffer;

/*
 * Configure timer to output compare mode
 */
static void
DMA_Test()
{
    gBuffer.init();

    /*
     * Setup timer to output compare mode
     */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_TIM2);
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_SetDataTransferDirection( DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetMemoryAddress( DMA1, LL_DMA_CHANNEL_2, (uint32_t)gBuffer.data());
    LL_DMA_SetMemoryIncMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetMemorySize( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_WORD);

    LL_DMA_SetMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_2, gBuffer.length());

    LL_DMA_SetPeriphAddress( DMA1, LL_DMA_CHANNEL_2, (uint32_t)(&TIM2->CCR2));
    LL_DMA_SetPeriphIncMode( DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_NOINCREMENT);
    LL_DMA_SetPeriphSize( DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_WORD);

    LL_DMA_ClearFlag_TC2( DMA1);
    LL_DMA_ClearFlag_HT2( DMA1);

    LL_DMA_EnableIT_TC( DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_EnableIT_HT( DMA1, LL_DMA_CHANNEL_2);

    LL_TIM_SetAutoReload( TIM2, LED_TransferTime::Period - 1);
    LL_TIM_SetCounterMode( TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_UPDATE( TIM2);
    LL_TIM_EnableDMAReq_UPDATE( TIM2);
    LL_TIM_CC_SetDMAReqTrigger( TIM2, LL_TIM_CCDMAREQUEST_UPDATE);

    // Timer Channel 2
    LL_TIM_OC_SetPolarity( TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_SetMode( TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH2( TIM2, 0);

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
        LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
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

    LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_9);
    LL_DMA_ClearFlag_TE2( DMA1);
    LL_DMA_ClearFlag_TC2( DMA1);
    LL_DMA_ClearFlag_HT2( DMA1);
    //LL_TIM_ClearFlag_CC1(TIM2);
}

void delay()
{
        for ( uint32_t t = 0; t < 500000; t++ )
        {
        }
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
    uint8_t i = 0;
    for ( ;; )
    {
        for ( i = 0; i < LED_Stripe::LedsCount; i++ )
        {
            gBuffer.writeLED( LED_Color{0, 10, 0}, i);
            delay();
        }

        for ( i = LED_Stripe::LedsCount; i != 0; i-- )
        {
            gBuffer.writeLED( LED_Color{0, 0, 0}, i);
            delay();
        }
    }
    return 0;
}


