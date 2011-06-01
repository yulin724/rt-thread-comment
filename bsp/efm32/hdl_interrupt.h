/******************************************************************//**
 * @file 		hdl_interrupt.h
 * @brief 	USART driver of RT-Thread RTOS for EFM32
 * 	COPYRIGHT (C) 2011, RT-Thread Development Team
 * @author 	onelife
 * @version 	0.4 beta
 **********************************************************************
 * @section License
 * The license and distribution terms for this file may be found in the file LICENSE in this 
 * distribution or at http://www.rt-thread.org/license/LICENSE
 **********************************************************************
 * @section Change Logs
 * Date			Author		Notes
 * 2010-12-29	onelife		Initial creation for EFM32
 *********************************************************************/
#ifndef __HDL_INTERRUPT_H__
#define __HDL_INTERRUPT_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
enum efm32_irq_hook_type_t
{
	efm32_irq_type_dma = 0,
	efm32_irq_type_rtc,
	efm32_irq_type_timer,
	efm32_irq_type_gpio,
	efm32_irq_type_acmp,
	efm32_irq_type_usart,
	efm32_irq_type_iic
};

typedef void (*efm32_irq_callback_t)(rt_device_t device);

typedef struct 
{
	enum efm32_irq_hook_type_t type;
	rt_uint8_t unit;
	efm32_irq_callback_t cbFunc;
	void *userPtr;
} efm32_irq_hook_init_t;

typedef struct 
{
	efm32_irq_callback_t cbFunc;
	void *userPtr;
} efm32_irq_hook_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//void NMI_Handler(void);
//void MemManage_Handler(void);
//void BusFault_Handler(void);
//void UsageFault_Handler(void);
//void SVC_Handler(void);
//void DebugMon_Handler(void);
void DMA_IRQHandler_All(unsigned int channel, bool primary, void *user);
void efm32_irq_hook_register(efm32_irq_hook_init_t *hook);

#endif /* __HDL_INTERRUPT_H__ */

