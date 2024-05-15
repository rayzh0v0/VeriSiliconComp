#include "massage.h"
#include "uart_printf.h"
#include "osal_task_api.h"
#include "bsp.h"
#include "vpi_event.h"

/**
 * @brief Call back function of system manager
 *        Use to receive massage from custom task
 *
 * @param Same as EventHandler
 *
 * @return Any data as an ack. Here use 0 (Based on current understanding)
 */
static int sys_manager_handler(void *cobj, uint32_t event_id, void *param) {
	unsigned int val;

	val = (unsigned int)param;
	uart_printf("system rcv: %x", val);

	return 0;
}

/**
 * @brief Call back function of custom manager
 *        Currently have nothing to do
 *
 * @param Same as EventHandler
 *
 * @return Any data as an ack. Here use 0 (Based on current understanding)
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
static void task_sys_mgr(void *param)
{
	void* pSysManager;

	pSysManager = vpi_event_new_manager(COBJ_SYS_MGR, sys_manager_handler);
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
	void* val;

	pCustomManager = vpi_event_new_manager(COBJ_CUSTOM_MGR, custom_manager_handler);
	vpi_event_register(EVENT_SYS_TEST, pCustomManager);

	uart_printf("custom task register cplt");

	val = (void*)0xa5a5;

	while(1)
	{
		uart_printf("custom send: %x", (unsigned int)val);
		vpi_event_notify(EVENT_SYS_TEST, val++);
		osal_sleep(1000);
		vpi_event_listen(pCustomManager); // From development manual I know need to do this, but why?
	}
}

/**
 * @brief Initialize massage tasks
 *
 */
void massage_init(void)
{
	osal_create_task(task_sys_mgr, "my_sys", 256, 4, NULL);
	osal_create_task(task_custom_mgr, "my_custom", 256, 5, NULL);

	return;
}

