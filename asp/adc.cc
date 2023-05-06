#include "adc.h"
#include "../drivers/ll/stm32f0xx.h"
#include "../drivers/ll/stm32f0xx_ll_rcc.h"
#include "../drivers/ll/stm32f0xx_ll_gpio.h"
#include "../drivers/ll/stm32f0xx_ll_bus.h"
#include "../drivers/ll/stm32f0xx_ll_tim.h"
#include "../drivers/ll/stm32f0xx_ll_dma.h"
#include "../drivers/ll/stm32f0xx_ll_adc.h"

//
// Init AdcBuffer.
//
void
ASP_AdcBuffer::init()
{
    for ( uint32_t i = 0; i != kBufferLength; i++ )
    {
        data_[i] = 0;
    }
}

void
ASP_StartProcessing( ASP_AdcBuffer* buffer)
{
    //
    // Set up GPIO PA4, PA5 pins as Analog input (see ADC additional function)
    // Docs: docs/stm32f051_family.pdf
    //
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_SetPinMode( GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode( GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_INPUT);

    /* Turn on ADC1 as peripheral */
    LL_APB1_GRP2_EnableClock( LL_APB1_GRP2_PERIPH_ADC1);

    /* Clock selection */
    LL_ADC_SetClock( ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    /* ADC Calibration process */
    if ( LL_ADC_IsEnabled( ADC1) )
    {
        LL_ADC_Disable( ADC1);
    }

    while ( LL_ADC_IsEnabled( ADC1) )
    { /* Do nothing */ }

    LL_ADC_StartCalibration( ADC1);

    while ( LL_ADC_IsCalibrationOnGoing(ADC1) )
    { /* Do nothing */ }

    /* Turn on ADC */
    LL_ADC_Enable( ADC1);
    LL_ADC_SetResolution( ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment( ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetLowPowerMode( ADC1, LL_ADC_LP_MODE_NONE);
    LL_ADC_SetSamplingTimeCommonChannels( ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);

    LL_ADC_REG_SetTriggerSource( ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    LL_ADC_REG_SetSequencerChannels( ADC1, LL_ADC_CHANNEL_4 | LL_ADC_CHANNEL_5);
    LL_ADC_REG_SetContinuousMode( ADC1, LL_ADC_REG_CONV_CONTINUOUS);
    LL_ADC_REG_SetDMATransfer( ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
    //LL_ADC_REG_SetOverrun( ADC1, LL_ADC_REG_OVR_DATA_PRESERVED);
    LL_ADC_REG_SetOverrun( ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    // Set up DMA Channel 1
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_DMA1);
    LL_DMA_SetDataTransferDirection( DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetMode( DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode( DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode( DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

    LL_DMA_SetPeriphSize( DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize( DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_SetChannelPriorityLevel( DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_VERYHIGH);

    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_1, buffer->length());
    LL_DMA_SetPeriphAddress( DMA1, LL_DMA_CHANNEL_1, (uint32_t)&(ADC1->DR));
    LL_DMA_SetMemoryAddress( DMA1, LL_DMA_CHANNEL_1, (uint32_t)buffer->data());

    LL_DMA_ClearFlag_TC1( DMA1);
    LL_DMA_EnableChannel( DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC( DMA1, LL_DMA_CHANNEL_1);

    /* Enable interrupt */
    NVIC_SetPriority( DMA1_Channel1_IRQn, 0);
    NVIC_EnableIRQ( DMA1_Channel1_IRQn);

    /* Enable ADC conversion */
    LL_ADC_SetCommonPathInternalCh( ADC, LL_ADC_PATH_INTERNAL_NONE);

    LL_ADC_REG_StartConversion( ADC1);
    LL_GPIO_TogglePin( GPIOC, LL_GPIO_PIN_8);
    return;
}



