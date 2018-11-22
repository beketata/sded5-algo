#pragma once


#include "common.h"
#include "main.h"
#include "SOCKET.h"


extern char			_mesageType;
extern BYTE			_is_EP1RX_Received;
extern WORD			_usb_Buf_EP1TX[32];
extern CMD_PROG	_usb_BufCmd_EP1RX;
extern void( * const _USB_StandardDeviceRequest[] )();
//extern DWORD 		_USB_Device;

extern "C"
{
	bool USB_ReceiveBuf_EP1RX();
	void USB_SendBuf_EP1TX();
	void USB_ReceiveBuf_EP2RX();
	void USB_SendBuf_EP2TX();
	void USB_Send_CopyToBufEP1TX( WORD, BYTE* );
	void USB_SendProgFeatureNames();
	void USB_Setup();
	void USB_ReceiveBuf_EP1RX_Parse();
	void USB_BlockMemory_Send();
	void USB_BlockMemory_Receive();
	void USB_PROGPROC_SendReadBuf( WORD blockNum, WORD blocks, DWORD blockSize );
	void USB_WaitEP2TX();
	void USB_WaitEP2RX();
	void USB_LibFile_Receive();
	void USB_LibFile_Receive1();
	
	void USB_Vendor_Specific_Command_();
}
