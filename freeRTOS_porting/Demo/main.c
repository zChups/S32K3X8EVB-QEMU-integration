#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpuart.h"
#include "pit.h"

#include <stdio.h>

#define mainTASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )


void vTask1(void *pvParameters) {
	(void)pvParameters;
    	for (;;) {
        	my_printf("Task 1 running\n");
        	vTaskDelay(pdMS_TO_TICKS(2000)); 
    	}
}

void vTask2(void *pvParameters) {
	(void)pvParameters;
   	for (;;) {
        	my_printf("Task 2 running\n");
        	vTaskDelay(pdMS_TO_TICKS(3000)); 
    	}
}

int main(void) {
    	// Initialize hardware (UART, PIT, etc.)
    	__asm("CPSIE I");  // Enable interrupts in ARM architecture
    	lpuart_init();
    	pit_init();

    	// Create tasks
    	xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    	xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    	// Start the scheduler
    	vTaskStartScheduler();

    	// Should never reach here
    	for (;;);
    	return 0;
}

