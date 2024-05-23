/*
 * Implementation function description:
 * Custom task loop sends data from the arr array.
 * System task receives data sent by custom task and displays them on console.
 */

#include "massage.h"
#include "uart_printf.h"
#include "osal_task_api.h"
#include "bsp.h"
#include "vpi_event.h"

uint32_t arr[] = {
	0xa5a0, 0xa5a1, 0xa5a2, 0xa5a3, 0xa5a4,
	0xa5a5, 0xa5a6, 0xa5a7, 0xa5a8, 0xa5a9
};

/**
 * @brief Call back function of system manager
 *        Use to receive massage from custom task
 *
 * @param Same as EventHandler
 *
 * @return Any ack back(Based on current understanding)
 */
static int system_manager_handler(void *cobj, uint32_t event_id, void *param) {
	uart_printf("system rcv: %lx", *(uint32_t*)param);

	return 0;
}

/**
 * @brief Call back function of custom manager
 *        Currently have nothing to do
 *
 * @param Same as EventHandler
 *
 * @return Any ack back(Based on current understanding)
 */
static int custom_manager_handler(void *cobj, uint32_t event_id, void *param) {

    return 0;
}

/**
 * @brief Call back function of system massage task
 *        Listening notification from event system
 *
 * @param Params from osal_create_task
 *
 */
static void task_system_mgr(void *param)
{
	void* pSysManager;

	pSysManager = vpi_event_new_manager(COBJ_SYS_MGR, system_manager_handler);
	vpi_event_register(EVENT_SYS_TEST, pSysManager);

	uart_printf("system task register cplt");

	while(1)
	{
		vpi_event_listen(pSysManager);
	}
}

/**
 * @brief Call back function of system massage task
 *        Send data to event system
 *
 * @param Params from osal_create_task
 *
 */
static void task_custom_mgr(void *param)
{
	osal_sleep(1000);

	void* pCustomManager;
	int i = 0;
	int arr_len = sizeof(arr) / sizeof(arr[0]);

	pCustomManager = vpi_event_new_manager(COBJ_CUSTOM_MGR, custom_manager_handler);
	vpi_event_register(EVENT_SYS_TEST, pCustomManager);

	uart_printf("custom task register cplt");

	while(1)
	{
		uart_printf("custom send: %lx", *arr+i);
		vpi_event_notify(EVENT_SYS_TEST, (void*)(arr+i));
		i = (i + 1) % arr_len;
		osal_sleep(1000);
		vpi_event_listen(pCustomManager);
	}
}

/**
 * @brief Initialize massage tasks
 *
 */
int massage_init(void)
{
	// create system message task
	if(osal_create_task(task_system_mgr, "my_system", 256, 4, NULL) == NULL){
		uart_printf("system message create error");
		return -1;
	}

	// create custom message task
	if(osal_create_task(task_custom_mgr, "my_custom", 256, 5, NULL) == NULL){
		uart_printf("custom message create error");
		return -1;
	}

	return 0;
}

