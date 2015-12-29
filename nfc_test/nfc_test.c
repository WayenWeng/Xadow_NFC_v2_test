#include "vmsystem.h" 
#include "vmtype.h"
#include "vmsystem.h"
#include "vmthread.h"
#include "vmcmd.h"
#include "vmlog.h"
#include "vmtimer.h"
#include "ResID.h"
#include "nfc_test.h"
#include "ldlcgpio.h"
#include "ldlci2cV2.h"
#include "lnfc.h"
#include "string.h"


#define COMMAND_PORT  1000


VM_TIMER_ID_PRECISE sys_timer_id = 0;


unsigned char sys_time = 0;

unsigned char test_flag = 0;


void command_callback(vm_cmd_command_t *param, void *user_data)
{
    vm_log_info("cmd = %s", (char*)param->command_buffer);

    if(strcmp("scan",(char*)param->command_buffer) == 0)
    {
    	unsigned char status = nfc_check_online();
    	if(status)vm_log_info("NFC is on-line.");
    	else vm_log_info("NFC is off-line.");
    }
    else if(strcmp("ada",(char*)param->command_buffer) == 0)
    {
    	nfc_adapter_init();
    	vm_log_info("nfc_adapter_init");
    }
    else if(strcmp("uid",(char*)param->command_buffer) == 0)
    {
    	unsigned char i;
    	unsigned char *data;

    	nfc_adapter_init();
    	vm_thread_sleep(50);
    	data = nfc_adapter_get_uid();

    	for(i=0;i<(data[1] + 2);i++)
    	{
    		vm_log_info("0x%x",data[i]);
    	}
    }
    else if(strcmp("read",(char*)param->command_buffer) == 0)
    {
    	unsigned char *data = nfc_adapter_read();
    	vm_log_info("0x%x,0x%x,%s",*data, *(data+1), data+2);
    	//vm_free(data);
    }
    else if(strcmp("write",(char*)param->command_buffer) == 0)
    {
    	char str[8] = {'X','a','d','o','w',' ','0','\0'};
    	char *ptr = str;
    	str[6] = '0' + sys_time ++;
    	if(sys_time > 9)sys_time = 0;
    	nfc_adapter_write(ptr);
    	vm_log_info("nfc_adapter_write length is %d", strlen(ptr));
    }
    else if(strcmp("erase",(char*)param->command_buffer) == 0)
    {
    	nfc_adapter_erase();
    	vm_log_info("nfc_adapter_earse");
    }
    else if(strcmp("start",(char*)param->command_buffer) == 0)
    {
    	test_flag = 1;
    }
    else if(strcmp("stop",(char*)param->command_buffer) == 0)
    {
    	test_flag = 0;
    }
    else
    {
    	nfc_adapter_write((char*)param->command_buffer);
    }
}

void sys_timer_callback(VM_TIMER_ID_PRECISE sys_timer_id, void* user_data)
{
	vm_log_info("Now is time loop.");

	unsigned char i;
	unsigned char *data;

	if(test_flag == 1)
	{
		data = nfc_adapter_get_uid();
		for(i=0;i<(data[1] + 2);i++)vm_log_info("0x%x",data[i]);

		//unsigned char *data = nfc_adapter_read();
		//vm_log_info("0x%x,0x%x,%s",*data, *(data+1), data+2);
	}
}

void handle_sysevt(VMINT message, VMINT param)
{
    switch (message)
    {
        case VM_EVENT_CREATE:
			vm_log_info("Open AT Port & Reg Callback");
			vm_cmd_open_port(COMMAND_PORT, command_callback, NULL);

			pinMode(18, INPUT);
			pinMode(13, OUTPUT);
			digitalRead(18);
			digitalWrite(13, HIGH);

            sys_timer_id = vm_timer_create_precise(500, sys_timer_callback, NULL);
            break;
        case VM_EVENT_PAINT:
            break;
        case VM_EVENT_QUIT:
            break;
    }
}

void vm_main(void) 
{
    /* register system events handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
}
