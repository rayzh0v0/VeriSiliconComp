#include "massage.h"
#include "uart_printf.h"
#include "osal_task_api.h"
#include "bsp.h"
#include "vpi_event.h"

int sys_manager_handler(void *cobj, uint32_t event_id, void *param) {
	unsigned int val;

	val = (unsigned int)param;
	uart_printf("sys manager rcv: %x", val);

	return 0;
}

int custom_manager_handler(void *cobj, uint32_t event_id, void *param) {

    return 0;
}


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

static void task_custom_mgr(void *param)
{
	osal_sleep(1000);

	void* pCustomManager;

	pCustomManager = vpi_event_new_manager(COBJ_CUSTOM_MGR, custom_manager_handler);
	vpi_event_register(EVENT_SYS_TEST, pCustomManager);

	uart_printf("custom task register cplt");

	void* val = (void*)0xa5a5;
	vpi_event_notify(EVENT_SYS_TEST, val);

	while(1)
	{
		osal_sleep(1000);
		vpi_event_notify(EVENT_SYS_TEST, ++val);
		vpi_event_listen(pCustomManager);
	}
}

void massage_init()
{
	osal_create_task(task_sys_mgr, "my_sys", 256, 4, NULL);
	osal_create_task(task_custom_mgr, "my_custom", 256, 5, NULL);
}

