/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2010-04-09     fify         for M16C
 */

#include <rthw.h>
#include <rtthread.h>

#include "iom16c62p.h"
#include "board.h"
#include "bsp.h"

extern int  rt_application_init(void);
#ifdef RT_USING_FINSH
extern void finsh_system_init(void);
extern void finsh_set_device(const char* device);
#endif

#ifdef RT_USING_HEAP
#ifdef __ICCM16C__
#pragma section="DATA16_HEAP"
#endif
#endif

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
	/* init board */
	rt_hw_board_init();

	/* show version */
	rt_show_version();

	/* init tick */
	rt_system_tick_init();
    
	/* init kernel object */
	rt_system_object_init();
    
	/* init timer system */
	rt_system_timer_init();
    
#ifdef RT_USING_HEAP
#ifdef __ICCM16C__
    rt_system_heap_init(__segment_begin("DATA16_HEAP"),__segment_end("DATA16_HEAP"));
#endif
#endif

	/* init scheduler system */
	rt_system_scheduler_init();
    
#ifdef RT_USING_DEVICE
	/* init all device */
	rt_device_init_all();
#endif

	/* init application */
	rt_application_init();

#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
	finsh_set_device("uart0");
#endif

    /* init timer thread */
    rt_system_timer_thread_init();
    
	/* init idle thread */
	rt_thread_idle_init();
    
	/* start scheduler */
	rt_system_scheduler_start();
    
	/* never reach here */
	return ;
}

int main(void)
{
	/* disable interrupt first */
	rt_hw_interrupt_disable();

	/* init system setting */
    system_init();
    
	/* startup RT-Thread RTOS */
	rtthread_startup();

	return 0;
}
