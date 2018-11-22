#include "common.h"
#include "main.h"
#include "USB.h"
#include "kernel.h"
#include "D14.h"
#include "usb_init.h"


__no_init volatile D14_CNTRL_REG D14_Cntrl_Reg @0x10000000;
__no_init KERNEL _USB_Kernel_Flag;
__no_init BYTE* Descriptor_Ptr;
__no_init USB_INTERRUPT _USB_Int_Flag;
__no_init USB_DEVICE_REQ _USB_Device_Request;
__no_init USB_DEVICE _USB_Device;
__no_init FILESIZE FileSize;
__no_init WORD _Temp;
__no_init ULI _bytecount;

/*
void USB_reserved();
void USB_get_status();
void USB_clear_feature();
void USB_set_feature();
void USB_set_address();
void USB_get_descriptor();
void USB_get_config();
void USB_set_config();
void USB_get_interface();
void USB_set_interface();
void USB_wait_ep0rx();
void USB_wait_ep0tx();
void USB_ep0in_ack();
void USB_ep0out_ack();
*/
void USB_Setup();

// USB Standard request functions
//
void( * const _USB_StandardDeviceRequest[] )() =
{
	USB_get_status,
	USB_clear_feature,
	USB_reserved,
	USB_set_feature,
	USB_reserved,
	USB_set_address,
	USB_get_descriptor,
	USB_reserved,
	USB_get_config,
	USB_set_config,
	USB_get_interface,
	USB_set_interface,
	USB_reserved,
	USB_reserved,
	USB_reserved,
	USB_reserved
};


void USB_Debug_Mode()
{
	// Endpoint 0 received setup packet processing
	if( _USB_Int_Flag.BITS.EP0SETUP )
	{
		EA_DISABLE;
		USB_Setup();
		EA_ENABLE;
	}

	// Suspend processing
	if( _USB_Int_Flag.BITS.SUSP )
	{
		// Clear pending flag
		EA_DISABLE;
		_USB_Int_Flag.BITS.SUSP = 0;
		EA_ENABLE;

		// ISP1582 enters suspend state
		D14_Cntrl_Reg.D14_MODE.BITS.GOSUSP = 1;
		D14_Cntrl_Reg.D14_MODE.BITS.GOSUSP = 0;
	}

	// Suspend recovery processing
	if( _USB_Int_Flag.BITS.RESUME )
	{
		// Clear recovery flag
		EA_DISABLE;
    _USB_Int_Flag.BITS.RESUME = 0;
    EA_ENABLE;

		// Generate a recovery signal
		D14_Cntrl_Reg.D14_MODE.BITS.SNDRSU = 1;
		D14_Cntrl_Reg.D14_MODE.BITS.SNDRSU = 0;
  }

	// High speed state
	if( _USB_Int_Flag.BITS.HS_STAT )
	{
		// Clear high speed status flag
		EA_DISABLE;
		_USB_Int_Flag.BITS.HS_STAT = 0;
		EA_ENABLE;
		
		if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
		{
			// Set the high speed mode status
			_USB_Kernel_Flag.BITS.HS_FS_State = HIGH_SPEED;

			// Reconfigure high speed endpoints
			USB_Init_Endpoint();
		}
  }
}

void USB_Set_EndpIndex_ControlFunc( eUSB_ENDP endpIndex, BYTE controlFunc )
{
  D14_Cntrl_Reg.D14_ENDPT_INDEX = (BYTE)endpIndex;
  D14_Cntrl_Reg.D14_CONTROL_FUNCTION.VALUE = controlFunc;
}

void USB_Vendor_Specific_Command()
{
	if( _USB_Device_Request.bRequest == 0x0C )
	{
		switch( _USB_Device_Request.wIndex )
		{
			case GET_FIRMWARE_VERSION:
			{
				if( _USB_Device_Request.wLength == 1 )
				{
					//select endpoint
					D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;

					//set the data length to 1 byte
					D14_Cntrl_Reg.D14_BUFFER_LENGTH = 1;

					//sent 0x21 as data
					D14_Cntrl_Reg.D14_DATA_PORT = 0x21;// Version number
					USB_wait_ep0tx();
					USB_ep0out_ack();
		
					break;
				}
			}

			case SETUP_DMA_REQUEST:
			{
				//Out Token Ack detected
				USB_wait_ep0rx();

				D14_Cntrl_Reg.D14_ENDPT_INDEX = 0;

				//
				FileSize.Size.Address[0] = D14_Cntrl_Reg.D14_DATA_PORT;
				FileSize.Size.Address[1] = D14_Cntrl_Reg.D14_DATA_PORT;
				FileSize.Size.Address[2] = D14_Cntrl_Reg.D14_DATA_PORT;

				// Read transfer bytes
				//
				_Temp = D14_Cntrl_Reg.D14_DATA_PORT;
				_bytecount = ( _Temp << 8 ) | ( _Temp >> 8 );

				// Read transmission method
				//
				FileSize.Size.DIR = D14_Cntrl_Reg.D14_DATA_PORT;
				USB_ep0in_ack();
				_USB_Kernel_Flag.BITS.Transfer_Start = 1; //Sign transmission data begins
			}
			break;

			default:
			{
				if( _USB_Device_Request.bmRequestType == 0xC0 )
				{
					USB_ep0in_ack();
				}
				else if( _USB_Device_Request.bmRequestType == 0x40 )
				{
					D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0RX;
					D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;

					while( !_USB_Int_Flag.BITS.EP0SETUP )
					{
						// Read ISP1582 interrupt register
						//
						_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

						// Write back ISP1582 interrupt register
						//
						D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
						if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
							return;
					}

					_USB_Int_Flag.BITS.EP0TX = 0;
				}
			}
			break;
		}
	}
}

__irq __arm void USB_ISR_SPU()
{
  D14_Cntrl_Reg.D14_UNLOCK_DEVICE = 0xAA37;

  D14_Cntrl_Reg.D14_MODE.BITS.GLINTE = 0;			// Disable USB Global Interrupt
	
	_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;
	_USB_Int_Flag.VALUE &= 0x3FDB9;
	D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;	// Биты прерываний очищаются записью в них 1

	D14_Cntrl_Reg.D14_MODE.BITS.GLINTE = 1;			// Enable USB Global Interrupt

	AT91C_BASE_AIC->AIC_EOICR = 0;							// (AIC) End of Interrupt Command Register
}

__fiq __arm void USB_ISR()
{
  D14_Cntrl_Reg.D14_UNLOCK_DEVICE = 0xAA37;

  D14_Cntrl_Reg.D14_MODE.BITS.GLINTE = 0;			// Disable USB Global Interrupt

	_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;
	_USB_Int_Flag.VALUE &= 0x3FDB9;
	D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;	// Биты прерываний очищаются записью в них 1

  if( _USB_Int_Flag.BITS.BRESET )
  {
    _USB_Int_Flag.BITS.BRESET = 0;
		_USB_Kernel_Flag.VALUE |= 0x4000;

    if( _USB_Int_Flag.BITS.HS_STAT )
    {
			_USB_Int_Flag.BITS.HS_STAT = 0;
			_USB_Kernel_Flag.VALUE &= 0xFF7FFFFF;
    }
    else
    {
			_USB_Kernel_Flag.VALUE |= 0x00800000;
    }

    USB_INTs_Setup();
  }
  else if( _USB_Int_Flag.BITS.VBUS )
	{
		_USB_Int_Flag.BITS.VBUS = 0;
		D14_Cntrl_Reg.D14_MODE.BITS.SOFTCT = 1;		// SoftConnect: Logic 1 enables the connection of the 1.5k pull-up resistor on pin RPU to the DP pin
	}
	else if( _USB_Int_Flag.BITS.EP3RX )
	{
		_USB_Int_Flag.BITS.EP3RX = 0;

		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP3RX;
		if( ( D14_Cntrl_Reg.D14_DATA_PORT == 0xF190 ) && ( _isInProgProc == 1 ) )
		{
			// !!!
			// v41 = sub_20002758;
			// v1FFC = (char *)v41 + 4;
		}
  }

	D14_Cntrl_Reg.D14_MODE.BITS.GLINTE = 1;			// Enable USB Global Interrupt

	AT91C_BASE_AIC->AIC_EOICR = 0;							// (AIC) End of Interrupt Command Register
}


void USB_Setup()
{
	_USB_Int_Flag.BITS.EP0SETUP = 0;						// Clear the EP0SETUP interrupt flag

	_USB_Kernel_Flag.BITS.Bus_Reset = 0;				// Clear bus reset flag

	D14_Cntrl_Reg.D14_ENDPT_INDEX = 0x20;				// Select endpoint 0 SETUP buffer

  WaitByTimerMks( 1 );

	// Read USB request
	//
	_Temp = D14_Cntrl_Reg.D14_DATA_PORT;
	
	// Store request type
	//
	_USB_Device_Request.bmRequestType = (BYTE)(_Temp);

	// Store request number
	//
	_USB_Device_Request.bRequest = (BYTE)(_Temp>>8);

	// Read 16bit request data
	//
	_Temp = D14_Cntrl_Reg.D14_DATA_PORT;
	_USB_Device_Request.wValue = (_Temp>>8)|(_Temp<<8);

	// Read 16bit request index
	//
	_Temp = D14_Cntrl_Reg.D14_DATA_PORT;
	_USB_Device_Request.wIndex = (_Temp>>8)|(_Temp<<8);;

	// Read 16bit length
	//
	_USB_Device_Request.wLength = D14_Cntrl_Reg.D14_DATA_PORT;

  if( _USB_Device_Request.bmRequestType & 0x80 )
    USB_Set_EndpIndex_ControlFunc( eUSB_ENDP_EP0TX, 4 );
  else
    USB_Set_EndpIndex_ControlFunc( eUSB_ENDP_EP0RX, 4 );

	BYTE type = _USB_Device_Request.bmRequestType & 0x60;
	BYTE req = _USB_Device_Request.bRequest & 0x0F;

	if( type == 0x40 )
	{
		USB_Vendor_Specific_Command();
	}
	else if( type == 0 )
	{
		_USB_StandardDeviceRequest[req]();
	}
}


// -------------------------------------------
//

void USB_reserved( void )
{
}

void USB_get_descriptor()
{
	BYTE count;
	BYTE i;
	alignas(2) BYTE buf[CONFIG_LENGTH];
	BYTE bufIndex;

	bufIndex = 0;

	switch( _USB_Device_Request.wValue )
	{
		case 1:	// Device descriptor
		{
			// Selection descriptor
			if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
				Descriptor_Ptr = (BYTE*)&USB_Standard_Device_Descriptor; //set pointer to device descriptor

			if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
				Descriptor_Ptr = (BYTE*)&USB_HS_Device_Descriptor; //set pointer to device descriptor

			// Send descriptor
			for( i=0; i<Descriptor_Ptr[0]; i++ )
			{
				buf[bufIndex++] = Descriptor_Ptr[i];
			}
		}
		break;

		case 2:  // Get configuration descriptor
		{
			_USB_Int_Flag.BITS.EP0RX = 0;
			Descriptor_Ptr = (BYTE*)&USB_Standard_Config_Descriptor; //set pointer to configuration descriptor
			count = Descriptor_Ptr[0];								// get configuration Descriptor length

			//check if only configuration descriptor required
			//检查描述符长度,决定是只发送配置描述符还是描述符集合
			if( count >= _USB_Device_Request.wLength ) //Send only Configuration descriptor
			{
				//send descriptor
				//发送描述符
				for( i=0; i<_USB_Device_Request.wLength; i++ )
				{
					buf[bufIndex++] = Descriptor_Ptr[i];
				}
			}
			else // send Configuration,Interface and Endpoint descriptor
			{
				// Send descriptor set (configuration + interface + endpoint)
				if( _USB_Device_Request.wLength >= CONFIG_LENGTH )
				{
					//写入配置描述符
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					//选择高速还是全速接口描述符
					if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_Bulk_Interface_Descriptor; //set pointer to interface descriptor

					if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_HS_Bulk_Interface_Descriptor; //set pointer to interface descriptor

					//写入接口描述符
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					//选择高速还是全速端点1IN描述符
					if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_Int_In_Descriptor; //set pointer to interface descriptor

					if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_HS_Int_In_Descriptor; //set pointer to interface descriptor

					//写入端点1IN描述符
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					//选择高速还是全速端点1OUT描述符
					if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_Int_Out_Descriptor; //set pointer to interface descriptor

					if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_HS_Int_Out_Descriptor; //set pointer to interface descriptor

					//写入端点1OUT描述符
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					// Choose high speed or full speed endpoint 2IN descriptor
					if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_Bulk_In_Descriptor; //set pointer to interface descriptor

					if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_HS_Bulk_In_Descriptor; //set pointer to interface descriptor

					// Write endpoint 2IN descriptor
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					// Choose high speed or full speed endpoint 2OUT descriptor
					if( _USB_Kernel_Flag.BITS.HS_FS_State == FULL_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_Bulk_Out_Descriptor; //set pointer to interface descriptor

					if( _USB_Kernel_Flag.BITS.HS_FS_State == HIGH_SPEED )
						Descriptor_Ptr = (BYTE*)&USB_HS_Bulk_Out_Descriptor; //set pointer to interface descriptor

					// Write endpoint 2OUT descriptor
					for( i=0; i<Descriptor_Ptr[0]; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}
					
					Descriptor_Ptr = (BYTE*)&USB_Iso_Out_Descriptor;
        	for( i=0; i<USB_Iso_Out_Descriptor[0].bLength; i++ )
        	{
						buf[bufIndex++] = Descriptor_Ptr[i];
        	}

					Descriptor_Ptr = (BYTE*)&USB_Iso_In_Descriptor;
        	for( i=0; i<USB_Iso_In_Descriptor[0].bLength; i++ )
        	{
						buf[bufIndex++] = Descriptor_Ptr[i];
        	}
				}
			}
		}
		break;

		// Get string descriptor
		//
		case 3:
		{
			Descriptor_Ptr = (BYTE*)&USB_Standard_String_ID; //set pointer to interface descriptor
			count = *Descriptor_Ptr;					// get configuration Descriptor length
			if( _USB_Device_Request.wLength < count )
				count = _USB_Device_Request.wLength; //get the remaining length

			// send string ID
			for( i=0; i<count; i++ )
			{
				buf[bufIndex++] = Descriptor_Ptr[i];
			}
		}
		break;

		case 4:
			USB_reserved();
		break;

		// Get the endpoint descriptor
		//
		case 5:
		{
			_Temp = 0;
			if( ( _USB_Device_Request.wValue & 0x00ff ) > 0x0001 )
			{
				D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;
				while( !_USB_Int_Flag.BITS.EP0SETUP )
				{
					// Read ISP1582 interrupt register
					_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

					// Write back ISP1582 interrupt register
					D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
					if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
						return;
				}

				_USB_Int_Flag.BITS.EP0TX = 0;
			}
			else
			{
				Descriptor_Ptr = (BYTE*)&USB_Bulk_Out_Descriptor; //set pointer to Bulk Out endpoint descriptor
				count = Descriptor_Ptr[0];				// get configuration Descriptor length

				if( count >= _USB_Device_Request.wLength )
				{
					for( i=0; i<_USB_Device_Request.wLength; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}
				}
				else
				{
					_USB_Device_Request.wLength -= count; //send Endpoint descriptor
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Bulk_In_Descriptor[0]; //set pointer to Bulk In endpoint descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					if( count >= _USB_Device_Request.wLength )
					{
						for( i=0; i<_USB_Device_Request.wLength; i++ )
						{
							buf[bufIndex++] = Descriptor_Ptr[i];
						}
					}
					else
					{
						_USB_Device_Request.wLength -= count;	// send Endpoint descriptor
						for( i=0; i<count; i++ )
						{
							buf[bufIndex++] = Descriptor_Ptr[i];
						}
					}
				}
			}
		}
		break;

		case 6:	// Get device limit descriptor
		{
			Descriptor_Ptr = (BYTE*)&USB_HS_Device_Qualifier[0]; // set pointer to High Speed device descriptor
			count = Descriptor_Ptr[0];					// get configuration Descriptor length

			if( _USB_Device_Request.wLength < count )
			{
				count = _USB_Device_Request.wLength;
			}

			for( i=0; i<count; i++ )
			{
				buf[bufIndex++] = Descriptor_Ptr[i];
			}
		}
		break;

		case 7:	// Get speed configuration descriptor
		{
			Descriptor_Ptr = (BYTE*)&USB_Other_Speed_Config_Descriptor[0];	// set pointer to configuration descriptor
			count = Descriptor_Ptr[0];					// get configuration Descriptor length

			if( count >= _USB_Device_Request.wLength ) // Send only Configuration descriptor
			{
				for( i=0; i<_USB_Device_Request.wLength; i++ )
				{
					buf[bufIndex++] = Descriptor_Ptr[i];
				}
			}
			else																		// send Configuration,Interface and Endpoint descriptor
			{
				if( _USB_Device_Request.wLength >= 32 )
				{
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Bulk_Interface_Descriptor[0]; //set pointer to interface descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Int_In_Descriptor[0]; //set pointer to interface descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Int_Out_Descriptor[0]; //set pointer to interface descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Bulk_In_Descriptor[0]; //set pointer to interface descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}

					Descriptor_Ptr = (BYTE*)&USB_Bulk_Out_Descriptor[0]; // set pointer to interface descriptor
					count = Descriptor_Ptr[0];			// get configuration Descriptor length
					for( i=0; i<count; i++ )
					{
						buf[bufIndex++] = Descriptor_Ptr[i];
					}
				}
			}
		}
		break;

		case 8:
		break;

		default:
		{
			D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
			D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;
			while( !_USB_Int_Flag.BITS.EP0SETUP )
			{
				// Read ISP1582 interrupt register
				_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

				// Write back ISP1582 interrupt register
				D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
				if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
					return;
			}

			_USB_Int_Flag.BITS.EP0TX = 0;
		}
		break;
	}
	
	if( bufIndex )
	{
		// Select the endpoint number
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;

		D14_Cntrl_Reg.D14_BUFFER_LENGTH = bufIndex;

		bufIndex++;																// in case of bufIndex is odd

		for( i=0; i<bufIndex; i+=2 )
		{
			D14_Cntrl_Reg.D14_DATA_PORT = *(WORD*)&buf[i];
		}

		USB_wait_ep0tx();
		USB_ep0out_ack();
	}
}

//Set_Address
void USB_set_address()
{
	_USB_Int_Flag.BITS.EP0RX = 0;
	D14_Cntrl_Reg.D14_ADDRESS.BITS.DEVADDR = ( _USB_Device_Request.wValue >> 8 );
	
	// State Response
	USB_ep0in_ack();
}

//Set_Config
void USB_set_config()
{
	_USB_Int_Flag.BITS.EP0RX = 0;
	if( _USB_Device_Request.wValue == 0 )
	{
		_USB_Device_Config_Value = 0;
		USB_ep0in_ack();
	}
	else if( _USB_Device_Request.wValue == 0x0100 )
	{
		// Configuring device
		_USB_Device_Config_Value = (BYTE)( _USB_Device_Request.wValue >> 8 );
		// _USB_Device.BITS.State = USB_Configured;
		USB_ep0in_ack();
	}
	else
	{
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;

		while( !_USB_Int_Flag.BITS.EP0SETUP )
		{
			//读取ISP1582中断寄存器
			_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

			//回写ISP1582中断寄存器
			D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
			if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
				return;
		}

		_USB_Int_Flag.BITS.EP0TX = 0;
	}
}

void USB_get_config()
{
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
	D14_Cntrl_Reg.D14_BUFFER_LENGTH = 1;
	D14_Cntrl_Reg.D14_DATA_PORT = _USB_Device_Config_Value;

	USB_wait_ep0tx();
	USB_ep0out_ack();
}

void USB_clear_feature()
{
	unsigned char bRecipient = _USB_Device_Request.bmRequestType & 0x0f;

	//端点
	if( bRecipient == 0x02 && _USB_Device_Request.wValue == 0x0000 )
	{
		switch( _USB_Device_Request.wIndex )
		{
		case  0x8000:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
			break;
		case  0x0000:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0RX;
			break;
		case  0x8100:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 3;
			break;
		case  0x0100:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 2;
			break;
		case  0x8200:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 5;
			break;
		case  0x0200:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 4;
			break;
		case  0x8300:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 7;
			break;
		case  0x0300:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 6;
			break;
		case  0x8400:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 9;
			break;
		case  0x0400:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 8;
			break;
		case  0x8500:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 11;
			break;
		case  0x0500:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 10;
			break;
		case  0x8600:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 13;
			break;
		case  0x0600:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 12;
			break;
		case  0x8700:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 15;
			break;
		case  0x0700:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 14;
			break;
		default:
			break;
		}

		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 0;
		_USB_Kernel_Flag.BITS.Endpt_Stall = 0;
		D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = OFF; //disable FIFO
		D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON; //disable FIFO
		USB_ep0in_ack();
		_USB_Device.BITS.Halt = 0;
	}
	else if( bRecipient == 0x00 && _USB_Device_Request.wValue == 0x0100 )
	{ //远程唤醒
		_USB_Device.BITS.Remote_Wakeup = 0;
		USB_ep0in_ack();
	}
	else
	{
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;
		while( !_USB_Int_Flag.BITS.EP0SETUP )
		{
			//读取ISP1582中断寄存器
			_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;
			//回写ISP1582中断寄存器
			D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
			if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
				return;
		}
		_USB_Int_Flag.BITS.EP0TX = 0;
	}
}

void USB_get_status()
{
	if( _USB_Device_Request.bmRequestType == 0x80 )
	{
		//端点0IN返回0x01,0x00两字节
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_BUFFER_LENGTH = 2;
		D14_Cntrl_Reg.D14_DATA_PORT = 1;

		//等待发送完毕
		USB_wait_ep0tx();
		USB_ep0out_ack();
	}

	if( _USB_Device_Request.bmRequestType == 0x81 )
	{
		//端点0IN返回0x00,0x00两字节
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_BUFFER_LENGTH = 2;
		D14_Cntrl_Reg.D14_DATA_PORT = 0;

		USB_wait_ep0tx();
		USB_ep0out_ack();
	}

	if( _USB_Device_Request.bmRequestType == 0x82 )
	{
		_Temp = _USB_Device.BITS.Halt;
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_BUFFER_LENGTH = 2;
		D14_Cntrl_Reg.D14_DATA_PORT = _Temp;

		USB_wait_ep0tx();
		USB_ep0out_ack();
	}
}

//Set_Feature特性设置
//不支持测试模式
void USB_set_feature()
{
	unsigned char bRecipient = _USB_Device_Request.bmRequestType & 0x0f;
	//端点
	if( bRecipient == 0x02 && _USB_Device_Request.wValue == 0x0000 )
	{
		switch( _USB_Device_Request.wIndex )
		{
		case  0x8000:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 1;
			break;
		case  0x0000:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 0;
			break;
		case  0x8100:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 3;
			break;
		case  0x0100:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 2;
			break;
		case  0x8200:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 5;
			break;
		case  0x0200:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 4;
			break;
		case  0x8300:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 7;
			break;
		case  0x0300:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 6;
			break;
		case  0x8400:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 9;
			break;
		case  0x0400:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 8;
			break;
		case  0x8500:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 11;
			break;
		case  0x0500:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 10;
			break;
		case  0x8600:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 13;
			break;
		case  0x0600:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 12;
			break;
		case  0x8700:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 15;
			break;
		case  0x0700:
			D14_Cntrl_Reg.D14_ENDPT_INDEX = 14;
			break;
		default:
			break;
		}

		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;
		_USB_Kernel_Flag.BITS.Endpt_Stall = 1;
		D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = OFF; //disable FIFO
																										//D14_Cntrl_Reg.D14_ENDPT_TYPE.BITS.ENABLE = ON; //disable FIFO
		USB_ep0in_ack();
		_USB_Device.BITS.Halt = 1; // 0;
	}
	else if( bRecipient == 0x00 && _USB_Device_Request.wValue == 0x0100 )
	{ //远程唤醒
		_USB_Device.BITS.Remote_Wakeup = 1;
		USB_ep0in_ack();
	}
	else
	{
		D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;
		while( !_USB_Int_Flag.BITS.EP0SETUP )
		{
			//读取ISP1582中断寄存器
			_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

			//回写ISP1582中断寄存器
			D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
			if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
				return;
		}
		_USB_Int_Flag.BITS.EP0TX = 0;
	}
}

//Get_Interface Status
void USB_get_interface()
{
	if( _USB_Device_Request.wValue != 0 )
	{
		D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STALL = 1;

		while( !_USB_Int_Flag.BITS.EP0SETUP )
		{
			//读取ISP1582中断寄存器
			_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;
			//回写ISP1582中断寄存器
			D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
			if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
				return;
		}
		_USB_Int_Flag.BITS.EP0TX = 0;
	}
	else
	{
		D14_Cntrl_Reg.D14_ENDPT_INDEX = 1;
		D14_Cntrl_Reg.D14_BUFFER_LENGTH = 1;

		if( !_USB_Device.BITS.Alter_Interface )
			D14_Cntrl_Reg.D14_DATA_PORT = 0;
		else
			D14_Cntrl_Reg.D14_DATA_PORT = 1;

		USB_wait_ep0tx();
		USB_ep0in_ack();
	}
}

//Set_Interface Status
void USB_set_interface()
{
	if( _USB_Device_Request.wIndex == 0 && _USB_Device_Request.wValue == 0x0100 )
		_USB_Device.BITS.Alter_Interface = 1;

	if( _USB_Device_Request.wIndex == 0x0100 && _USB_Device_Request.wValue == 0 )
		_USB_Device.BITS.Alter_Interface = 0;

	if( _USB_Device_Request.wIndex == 0 && _USB_Device_Request.wValue == 0 )
		_USB_Device.BITS.Alter_Interface = 0;

	USB_ep0in_ack();
}

//端点0IN状态响应
void USB_ep0in_ack()
{
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0TX;
	D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STATUS = 1;
	//wait_ep0tx();
}

//端点0IN状态响应
void USB_wait_ep0tx()
{
	//end of status stage
	while( !_USB_Int_Flag.BITS.EP0TX )
	{
		//读取ISP1582中断寄存器
		_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

		//回写ISP1582中断寄存器
		D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;
		if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
			return;
	}

	_USB_Int_Flag.BITS.EP0TX = 0;
}

//端点0OUT状态响应
void USB_ep0out_ack()
{
	//设置状态响应
	//acknowledge status stage
	D14_Cntrl_Reg.D14_ENDPT_INDEX = eUSB_ENDP_EP0RX;
	D14_Cntrl_Reg.D14_CONTROL_FUNCTION.BITS.STATUS = 1;
	// D14_Cntrl_Reg.D14_CONTROL_FUNCTION.VALUE = 2;
	//wait_ep0rx();
}

//端点0OUT状态响应
void USB_wait_ep0rx()
{
	while( !_USB_Int_Flag.BITS.EP0RX )
	{
		//读取ISP1582中断寄存器
		_USB_Int_Flag.VALUE |= D14_Cntrl_Reg.D14_INT.VALUE;

		//回写ISP1582中断寄存器
		D14_Cntrl_Reg.D14_INT.VALUE = _USB_Int_Flag.VALUE;

		if( _USB_Int_Flag.BITS.SUSP || _USB_Int_Flag.BITS.BRESET )
			return;
	}
	_USB_Int_Flag.BITS.EP0RX = 0;
}

