#include "common.h"
#include "main.h"
#include "USB.h"
#include "kernel.h"
#include "D14.h"
#include "usb_init.h"


__no_init BYTE _USB_Device_Config_Value;


void USB_Init()
{
	_USB_Device.VALUE = 0;
	_USB_Int_Flag.VALUE = 0;
	_USB_Kernel_Flag.VALUE = 0;
	_USB_Device_Request = {0};

	ISR_Setup( AT91C_ID_FIQ, 0, (DWORD)USB_ISR );

	D14_Cntrl_Reg.D14_UNLOCK_DEVICE = 0xAA37;

	// Soft Reset
	//
  D14_Cntrl_Reg.D14_MODE.VALUE = 0x96;
  WaitByTimerMs( 100 );
  D14_Cntrl_Reg.D14_MODE.BITS.SFRESET = 0;

	D14_Cntrl_Reg.D14_INT_CONFIG.VALUE = 0x54;

  if( !( D14_Cntrl_Reg.D14_CHIP_ID_MBYTE == 0x82 && D14_Cntrl_Reg.D14_CHIP_ID_MSB == 0x15 ) )
  {
    LCD_Print( eLCD_LINE4, "USB ID ERROR!" );
    Beep( 200 );
    while( 1 );
  }

	_USB_Kernel_Flag.BITS.HS_FS_State = 1;

  USB_INTs_Setup();

	ISR_Enable( 0 );
}

void USB_INTs_Setup()
{
	if( D14_Cntrl_Reg.D14_MODE.BITS.VBUSSTAT )	// VBUSSTAT VBUS Pin Status: This bit reflects the VBUS pin status.
		D14_Cntrl_Reg.D14_MODE.VALUE = 0x87;
	else
		D14_Cntrl_Reg.D14_MODE.VALUE = 0x86;

  D14_Cntrl_Reg.D14_INT_CONFIG.VALUE = 0x54;

  D14_Cntrl_Reg.D14_INT_ENABLE.VALUE = 0x3FDB9;

  USB_Init_Endpoint();

  _USB_Device_Config_Value = 0;
	
	_USB_Device.BITS.State = 0;
	_USB_Device.BITS.Alter_Interface = 0;
	_USB_Device.BITS.Big_Endian = 1;

  _USB_Kernel_Flag.BITS.Tx_Done = 0;

  D14_Cntrl_Reg.D14_MODE.BITS.GLINTE = 1;
}

//***********************************************************************
// Routine  : Init Endpoint
// Input : ISP1582 FIFO
// Output : None
// Function : To initialize ISP1581 endpoint FIFO
//***********************************************************************
//
void USB_Init_Endpoint()
{
	WORD packSize;

	packSize = 64;
	if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
		packSize = 512;

	// Interrupt In & Out
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP1RX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = 64;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x12;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP1TX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = 64;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x12;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	// Bulk In & Out
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP2RX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = packSize;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x16;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP2TX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = packSize;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x16;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	// Iso In & Out
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP3RX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = 64;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x12;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP3TX;
	D14_Cntrl_Reg.D14_ENDPT_MAXPKTSIZE.VALUE = 64;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.VALUE = 0x12;
	D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON;	// enable FIFO

	// Set the default address to be valid
	D14_Cntrl_Reg.D14_ADDRESS.VALUE = 0x80;
}

