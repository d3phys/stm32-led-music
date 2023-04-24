.syntax unified

.align 4
.section .text

.thumb_func
.global Reset_Handler
Reset_Handler:

.CopyDataSection:
    ldr r0, __data_start_lma_val
    ldr r1, __data_start_vma_val
    ldr r2, __data_end_vma_val

.CopyDataSectionLoop:
    cmp r1, r2
    beq .EndCopyDataSectionLoop

    ldr r4, [r0, #0]
    str r4, [r1, #0]

    adds r0, r0, #4
    adds r1, r1, #4
    b .CopyDataSectionLoop
.EndCopyDataSectionLoop:

.ZeroBssSection:
    movs r0, 0
    ldr r1, __bss_start_vma_val
    ldr r2, __bss_end_vma_val

.ZeroBssSectionLoop:
    cmp r1, r2
    beq .EndZeroBssSectionLoop

    str r0, [r1, #0]

    adds r1, r1, #4
    b .ZeroBssSectionLoop
.EndZeroBssSectionLoop:

.ExecuteCode:
    // Execute actual software:
    blx main

.Halt:
    b .Halt

.size Reset_Handler, .-Reset_Handler

.section .text
Default_Handler:
    b Default_Handler
.size Default_Handler, .-Default_Handler

__bss_start_vma_val:
.word __bss_start_vma
__bss_end_vma_val:
.word __bss_end_vma

__data_start_lma_val:
.word __data_start_lma
__data_start_vma_val:
.word __data_start_vma
__data_end_vma_val:
.word __data_end_vma

/******************************************************************************
*
* The minimal vector table for a Cortex M0.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
.section .vector_table
.VectorTable:

.word  __stack_start
.word  Reset_Handler
.word  NMI_Handler
.word  HardFault_Handler
.word  0
.word  0
.word  0
.word  0
.word  0
.word  0
.word  0
.word  SVC_Handler
.word  0
.word  0
.word  PendSV_Handler
.word  SysTick_Handler
.word  WWDG_IRQHandler                   /* Window WatchDog              */
.word  PVD_IRQHandler                    /* PVD through EXTI Line detect */
.word  RTC_IRQHandler                    /* RTC through the EXTI line    */
.word  FLASH_IRQHandler                  /* FLASH                        */
.word  RCC_CRS_IRQHandler                /* RCC and CRS                  */
.word  EXTI0_1_IRQHandler                /* EXTI Line 0 and 1            */
.word  EXTI2_3_IRQHandler                /* EXTI Line 2 and 3            */
.word  EXTI4_15_IRQHandler               /* EXTI Line 4 to 15            */
.word  TSC_IRQHandler                    /* TSC                          */
.word  DMA1_Channel1_IRQHandler          /* DMA1 Channel 1               */
.word  DMA1_Channel2_3_IRQHandler        /* DMA1 Channel 2 and Channel 3 */
.word  DMA1_Channel4_5_IRQHandler        /* DMA1 Channel 4 and Channel 5 */
.word  ADC1_COMP_IRQHandler              /* ADC1, COMP1 and COMP2        */
.word  TIM1_BRK_UP_TRG_COM_IRQHandler    /* TIM1 Break, Update, Trigger and Commutation */
.word  TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
.word  TIM2_IRQHandler                   /* TIM2                         */
.word  TIM3_IRQHandler                   /* TIM3                         */
.word  TIM6_DAC_IRQHandler               /* TIM6 and DAC                 */
.word  0                                 /* Reserved                     */
.word  TIM14_IRQHandler                  /* TIM14                        */
.word  TIM15_IRQHandler                  /* TIM15                        */
.word  TIM16_IRQHandler                  /* TIM16                        */
.word  TIM17_IRQHandler                  /* TIM17                        */
.word  I2C1_IRQHandler                   /* I2C1                         */
.word  I2C2_IRQHandler                   /* I2C2                         */
.word  SPI1_IRQHandler                   /* SPI1                         */
.word  SPI2_IRQHandler                   /* SPI2                         */
.word  USART1_IRQHandler                 /* USART1                       */
.word  USART2_IRQHandler                 /* USART2                       */
.word  0                                 /* Reserved                     */
.word  CEC_CAN_IRQHandler                /* CEC and CAN                  */
.word  0                                 /* Reserved                     */

.size .VectorTable, .-.VectorTable

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

.weak      NMI_Handler
.thumb_set NMI_Handler,Default_Handler

.weak      HardFault_Handler
.thumb_set HardFault_Handler,Default_Handler

.weak      SVC_Handler
.thumb_set SVC_Handler,Default_Handler

.weak      PendSV_Handler
.thumb_set PendSV_Handler,Default_Handler

.weak      SysTick_Handler
.thumb_set SysTick_Handler,Default_Handler

.weak      WWDG_IRQHandler
.thumb_set WWDG_IRQHandler,Default_Handler

.weak      PVD_IRQHandler
.thumb_set PVD_IRQHandler,Default_Handler

.weak      RTC_IRQHandler
.thumb_set RTC_IRQHandler,Default_Handler

.weak      FLASH_IRQHandler
.thumb_set FLASH_IRQHandler,Default_Handler

.weak      RCC_CRS_IRQHandler
.thumb_set RCC_CRS_IRQHandler,Default_Handler

.weak      EXTI0_1_IRQHandler
.thumb_set EXTI0_1_IRQHandler,Default_Handler

.weak      EXTI2_3_IRQHandler
.thumb_set EXTI2_3_IRQHandler,Default_Handler

.weak      EXTI4_15_IRQHandler
.thumb_set EXTI4_15_IRQHandler,Default_Handler

.weak      TSC_IRQHandler
.thumb_set TSC_IRQHandler,Default_Handler

.weak      DMA1_Channel1_IRQHandler
.thumb_set DMA1_Channel1_IRQHandler,Default_Handler

.weak      DMA1_Channel2_3_IRQHandler
.thumb_set DMA1_Channel2_3_IRQHandler,Default_Handler

.weak      DMA1_Channel4_5_IRQHandler
.thumb_set DMA1_Channel4_5_IRQHandler,Default_Handler

.weak      ADC1_COMP_IRQHandler
.thumb_set ADC1_COMP_IRQHandler,Default_Handler

.weak      TIM1_BRK_UP_TRG_COM_IRQHandler
.thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler

.weak      TIM1_CC_IRQHandler
.thumb_set TIM1_CC_IRQHandler,Default_Handler

.weak      TIM2_IRQHandler
.thumb_set TIM2_IRQHandler,Default_Handler

.weak      TIM3_IRQHandler
.thumb_set TIM3_IRQHandler,Default_Handler

.weak      TIM6_DAC_IRQHandler
.thumb_set TIM6_DAC_IRQHandler,Default_Handler

.weak      TIM14_IRQHandler
.thumb_set TIM14_IRQHandler,Default_Handler

.weak      TIM15_IRQHandler
.thumb_set TIM15_IRQHandler,Default_Handler

.weak      TIM16_IRQHandler
.thumb_set TIM16_IRQHandler,Default_Handler

.weak      TIM17_IRQHandler
.thumb_set TIM17_IRQHandler,Default_Handler

.weak      I2C1_IRQHandler
.thumb_set I2C1_IRQHandler,Default_Handler

.weak      I2C2_IRQHandler
.thumb_set I2C2_IRQHandler,Default_Handler

.weak      SPI1_IRQHandler
.thumb_set SPI1_IRQHandler,Default_Handler

.weak      SPI2_IRQHandler
.thumb_set SPI2_IRQHandler,Default_Handler

.weak      USART1_IRQHandler
.thumb_set USART1_IRQHandler,Default_Handler

.weak      USART2_IRQHandler
.thumb_set USART2_IRQHandler,Default_Handler

.weak      CEC_CAN_IRQHandler
.thumb_set CEC_CAN_IRQHandler,Default_Handler
