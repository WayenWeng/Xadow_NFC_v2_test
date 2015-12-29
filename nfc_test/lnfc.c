
#include "string.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "ldlci2cV2.h"
#include "lnfc.h"


#define print vm_log_info


unsigned char uid[10] = {0};
char message[120] = {0};


unsigned char nfc_check_online(void)
{
	unsigned char data[6];
	unsigned char i;

	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_SCAN_ID);
	dlc_i2c_send_byte(0);

	for(i=0;i<6;i++)
	{
		data[i] = dlc_i2c_receive_byte();
	}

	print("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", data[0], data[1], data[2], data[3], data[4], data[5]);

	if(data[5] == NFC_DEVICE_ADDR)return 1;
	else return 0;
}

void nfc_adapter_init(void)
{
	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_ADAPTER_INIT_ID);
	dlc_i2c_send_byte(0);
	//print("nfc_adapter_init");
}

unsigned char *nfc_adapter_get_uid(void)
{
	unsigned char i;

	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_ADAPTER_GET_UID_ID);
	dlc_i2c_send_byte(0);

	for(i=0;i<2;i++)
	{
		uid[i] = dlc_i2c_receive_byte();
	}

	for(i=0;i<uid[1];i++)
	{
		uid[i + 2]= dlc_i2c_receive_byte();
	}
	uid[uid[1] + 2] = '\0';
	/*
	for(i=0;i<(uid[1] + 2);i++)
	{
		print("0x%x",uid[i]);
	}
	*/
	return (unsigned char *)uid;
}

unsigned char *nfc_adapter_read(void)
{
	unsigned char data[2];
	unsigned char i;

	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_ADAPTER_READ_ID);
	dlc_i2c_send_byte(0);

	for(i=0;i<2;i++)
	{
		data[i] = dlc_i2c_receive_byte();
	}

	unsigned char length = data[1];
	//unsigned char *ptr = (unsigned char *)vm_malloc(length + 2 + 1);
	unsigned char *ptr = message;
	memset(ptr, 120, 0);

	*ptr = data[0];
	*(ptr + 1) = data[1];
	*(ptr + length + 2) = '\0';

	for(i=0;i<*(ptr + 1);i++)
	{
		*(ptr + i + 2) = dlc_i2c_receive_byte();
	}

	//print("0x%x,0x%x,%s",*ptr, *(ptr+1), ptr+2);

	return ptr;
}

void nfc_adapter_write(char *string)
{
	unsigned char length;
	unsigned char i;

	length = strlen(string);
	//print("nfc_adapter_write length is %d", length);

	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_ADAPTER_WRITE_ID);
	dlc_i2c_send_byte(length);

	for(i=0;i<length;i++)
	{
		dlc_i2c_send_byte(*(string + i));
	}
}

void nfc_adapter_erase(void)
{
	dlc_i2c_configure(NFC_DEVICE_ADDR, 100);
	dlc_i2c_send_byte(NFC_ADAPTER_ERASE_ID);
	dlc_i2c_send_byte(0);
	//print("nfc_adapter_earse");
}
