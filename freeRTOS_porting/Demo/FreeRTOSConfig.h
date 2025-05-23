#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 * Adjust these based on your system's hardware and application needs.
 *----------------------------------------------------------*/

/* CPU and clock configuration */			
#define configCPU_CLOCK_HZ                      ( 48000000ULL )  /* Update with your CPU clock frequency */
#define configTICK_RATE_HZ                      ( 1000 )         /* 1ms SysTick interrupt */
#define configMAX_PRIORITIES                    ( 5 )            /* Adjust based on application requirements */
#define configMINIMAL_STACK_SIZE                ( 128 )          /* Minimum stack size for idle tasks */
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 16 * 1024 ) ) /* Define based on available RAM */
#define configMAX_TASK_NAME_LEN                 ( 16 )           /* Max length for task names */

/* Kernel options */
#define configUSE_PREEMPTION                    1               /* Enable preemption */
#define configUSE_TIME_SLICING                  1               /* Enable time slicing */
#define configUSE_IDLE_HOOK                     0               /* Idle hook (optional) */
#define configUSE_TICK_HOOK                     0               /* Tick hook (optional, e.g., for debugging) */
#define configCHECK_FOR_STACK_OVERFLOW          0               /* Disable stack overflow checks */
#define configUSE_MALLOC_FAILED_HOOK            0               /* Disable malloc failure hook */
#define configUSE_16_BIT_TICKS                  0               /* Use 32-bit ticks (required for Cortex-M) */


/* Co-routine options */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Software timers */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( 3 )
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )

/* API function inclusion */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1

/* Cortex-M specific definitions */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS                     __NVIC_PRIO_BITS /* Use system's priority bits if defined */
#else
    #define configPRIO_BITS                     4               /* Default: 4 priority bits */
#endif

#define __FPU_PRESENT 1        // Cortex-M7 typically has an FPU
#define __NVIC_PRIO_BITS 4     // S32K3xx has 4 bits of priority


/* Priority group for ARM Cortex-M7 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY     15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Assertion handling */
#define configASSERT( x )                       if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }


#endif /* FREERTOS_CONFIG_H */


