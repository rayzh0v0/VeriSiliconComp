#include "osal_task_api.h"
#include "uart_printf.h"
#include "hello_verisilicon.h"

void hello_verisilicon(void *param)
{
    uart_printf("Hello VeriSilicon");
    osal_delete_task(NULL);
}
