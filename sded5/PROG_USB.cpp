#include "PROG_USB.h"


__no_init WORD			_usb_Buf_EP1TX[32];
__no_init CMD_PROG	_usb_BufCmd_EP1RX;
__no_init BYTE			_is_EP1RX_Received;
__no_init char			_mesageType;

const Pin _PA07_XIL_CCLK =      { (1 <<  7), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };
const Pin _PA13_XIL_PROGRAM =   { (1 << 13), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };
const Pin _PA14_XIL_DONE_Out0 = { (1 << 14), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };
const Pin _PA14_XIL_DONE_InUp = { (1 << 14), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT,    PIO_PULLUP };
const Pin _PB06_XIL_DIN =       { (1 <<  6), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT };
const Pin _PB07_InUp =          { (1 <<  7), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT,    PIO_PULLUP };
const Pin _PC06_XIL_INIT =      { (1 <<  6), AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT,    PIO_PULLUP };

const char* _progFeatureNames[] =
{
	"Program",
	"Read",
	"Verify",
	"Blank_Check",
	"Erase",
	"Program_PPB",
	"Erase_PPB",
	"Test_PPB",
	"Program_Hi_ROM",
	"Read_Hi_ROM",
	"Verify_Hi_ROM",
	"$"
};

const char*_progProcTypes[] = {"prvbeperprv", "$" };


void USB_PROGPROC_SendReadBuf( WORD blockNum, WORD blocks, DWORD blockSize )
{
  _usb_Buf_EP1TX[0] = 5;
  _usb_Buf_EP1TX[1] = blockNum;
  _usb_Buf_EP1TX[2] = blocks;
  _usb_Buf_EP1TX[3] = 0;
  *(DWORD*)&_usb_Buf_EP1TX[4] = blockSize;	// Размер одного блока данных в секторах по 512 байт.
  _usb_Buf_EP1TX[6] = 0;

	USB_SendBuf_EP1TX();
}


void USB_BlockMemory_Send()
{
	int	i;
	int	j;

	CMD_PROG* cmdProg = &_usb_BufCmd_EP1RX;
	WORD* buf = (WORD*)cmdProg->BaseAddr;

	for( i=0; i<cmdProg->Size; i += 512 )
	{
		USB_ENDP_INDEX = eUSB_ENDP_EP2TX;
		USB_BUF_LEN = 512;

		for( j=0; j<256; j++ )
		{
			USB_DATA_PORT = *(buf++);
		}

		while( !(_USB_Int_Flag.VALUE & eUSBMSK_EP2TX_CHECK) );

		AT91C_BASE_AIC->AIC_IDCR = 1;
		_USB_Int_Flag.VALUE &= eUSBMSK_EP2TX_CLEAR;
		AT91C_BASE_AIC->AIC_IECR = 1;
	}
}


// 
//
void USB_BlockMemory_Receive()
{
  WORD len;
  DWORD i, j;

  WORD* recBuf = (WORD*)_usb_BufCmd_EP1RX.BaseAddr;

	for( i=0; _usb_BufCmd_EP1RX.Size > i; i += len )
  {
    while( !(_USB_Int_Flag.VALUE & eUSBMSK_EP2RX_CHECK) );

    AT91C_BASE_AIC->AIC_IDCR = 1;
    _USB_Int_Flag.VALUE &= eUSBMSK_EP2RX_CLEAR;
    AT91C_BASE_AIC->AIC_IECR = 1;

    USB_ENDP_INDEX = eUSB_ENDP_EP2RX;
    len = USB_BUF_LEN;

    for( j=0; j<len; j+=2, recBuf++ )
    {
      *recBuf = USB_DATA_PORT;
    }
  }
}


void USB_SendProgFeatureNames()
{
	int		i;
	int		p;
	char	ch;
	const char* pCh;
	char 	buf[512];
	
	CMD_PROG* cmdProg = &_usb_BufCmd_EP1RX;

	p = 0;

	for( i=0; i<15; i++ )
  {
    pCh = _progFeatureNames[i];
		if( *pCh == '$' )
			i = 16;

		while( true )
    {
			ch = *pCh++;
      buf[p++] = ch;
			if( ch == 0 )
				break;
    }
	}

  for( i=0; i<2; i++ )
  {
    pCh = _progProcTypes[i];

		while( true )
    {
			ch = *pCh++;
      buf[p++] = ch;
			if( ch == 0 )
				break;
    }
  }
	
	for( i=p; i<sizeof( buf ); i++ )
	{
		buf[p++] = 0;
	}

  cmdProg->BaseAddr = (BYTE*)buf;

  USB_BlockMemory_Send();
}


void USB_SendBuf_EP1TX()
{
	unsigned int i;

	USB_ENDP_INDEX = eUSB_ENDP_EP1TX;
	USB_BUF_LEN = 64;

	for( i=0; i<32; i++ )
		USB_DATA_PORT = _usb_Buf_EP1TX[i];

	while ( !(_USB_Int_Flag.VALUE & eUSBMSK_EP1TX_CHECK)
       && !(_USB_Int_Flag.VALUE & eUSBMSK_SUSP_CHECK)
       && !(_USB_Kernel_Flag.VALUE & eUSBMSK_EP2RX_CHECK)
       && !(_USB_Int_Flag.VALUE & eUSBMSK_EP0SETUP_CHECK) );

	AT91C_BASE_AIC->AIC_IDCR = 1;
	_USB_Int_Flag.VALUE &= eUSBMSK_EP1TX_CLEAR;
	AT91C_BASE_AIC->AIC_IECR = 1;
}


bool USB_ReceiveBuf_EP1RX()
{
  unsigned int len;
  unsigned int i;

	if( !(_USB_Int_Flag.VALUE & eUSBMSK_EP1RX_CHECK) )
    return false;

	AT91C_BASE_AIC->AIC_IDCR = 1;
  _USB_Int_Flag.VALUE &= eUSBMSK_EP1RX_CLEAR;
  AT91C_BASE_AIC->AIC_IECR = 1;

  USB_ENDP_INDEX = eUSB_ENDP_EP1RX;
  len = USB_BUF_LEN/2;

  for( i=0; i<len; i++ )
    ((WORD*)&_usb_BufCmd_EP1RX)[i] = USB_DATA_PORT;

  return true;
}

void USB_LibFile_Receive1()
{
	int i;
	BYTE data8;
  WORD data, len;

  PIO_Configure( &_PA13_XIL_PROGRAM );				// Настраиваем ножку PROGRAM на вывод и подаем на нее 0
	{
		PIO_Configure( &_PA14_XIL_DONE_Out0 );
  	PIO_Configure( &_PC06_XIL_INIT );
  	PIO_Configure( &_PA07_XIL_CCLK );
  	PIO_Configure( &_PB06_XIL_DIN );
  	PIO_Configure( &_PB07_InUp );							// ??? Что за ножка ???
	}

  WaitByTimerMks( 10 );												//  The minimum Low pulse time for PROGRAM is 300 ns. There is no maximum value
	{
  	while( PIO_Get( &_PC06_XIL_INIT ) );			// Ждем, когда XILINX подтвердит начало очистки памяти, переводом INIT в 0
		PIO_Set( &_PA13_XIL_PROGRAM );						// Снимаем сигнал PROGRAM
  	while( !PIO_Get( &_PC06_XIL_INIT ) );			// Ждем, когда XILINX очистит память и отпустит INIT
	}
  WaitByTimerMks( 10 );

  for( i=0; ; i += len )
  {
    if( i >= 78756 )
      break;

    while( !(_USB_Int_Flag.VALUE & eUSBMSK_EP2RX_CHECK) );

    EA_DISABLE;
    _USB_Int_Flag.VALUE &= eUSBMSK_EP2RX_CLEAR;
    EA_ENABLE;

    USB_ENDP_INDEX = 4;
    len = USB_BUF_LEN;

    for( int j=0; j<len; j += 2 )
    {
      data = USB_DATA_PORT;
			
	    for( int n=0; n<2; n++ )
			{
				data8 = (BYTE)( data >> 8*n );

				for( int k=0; k<8; k++ )
    	  {
      	  AT91C_BASE_PIOA->PIO_CODR = 0x80;			// _PA07_XIL_CCLK = 0
					{
		        if( data8 & 0x80 )
  		        AT91C_BASE_PIOB->PIO_SODR = 0x40;	// _PB06_XIL_DIN = 1
    		    else
      		    AT91C_BASE_PIOB->PIO_CODR = 0x40;	// _PB06_XIL_DIN = 0
					}
  	      AT91C_BASE_PIOA->PIO_SODR = 0x80;			// _PA07_XIL_CCLK = 0

					data8 <<= 1;
      	}
			}
    }
  }

	// Проверяем, прошла ли проверка CRC
	//
  if( !PIO_Get( &_PC06_XIL_INIT ) )
  {
    LCD_Print( eLCD_LINE2, "CRC Error!      " );
    Beep( 200 );
    while( 1 );
  }

  PIO_Configure( &_PA14_XIL_DONE_InUp );			// Отпускаем сигнал DONE для перевода XILINX в рабочий режим.
  WaitByTimerMks( 2 );

  for( i=0; i<2; i++ )
  {
    PIO_Clear( &_PA07_XIL_CCLK );
    PIO_Set( &_PA07_XIL_CCLK );
  }
}

void USB_LibFile_Receive()
{
  bool v0;
	int i;
  WORD data, len;

  PIO_Configure( &_PA13_XIL_PROGRAM );				// Настраиваем ножку PROGRAM на вывод и подаем на нее 0
	{
		PIO_Configure( &_PA14_XIL_DONE_Out0 );
  	PIO_Configure( &_PC06_XIL_INIT );
  	PIO_Configure( &_PA07_XIL_CCLK );
  	PIO_Configure( &_PB06_XIL_DIN );
  	PIO_Configure( &_PB07_InUp );							// ??? Что за ножка ???
	}

  WaitByTimerMks( 10 );												//  The minimum Low pulse time for PROGRAM is 300 ns. There is no maximum value
	{
  	while( PIO_Get( &_PC06_XIL_INIT ) );			// Ждем, когда XILINX подтвердит начало очистки памяти, переводом INIT в 0
		PIO_Set( &_PA13_XIL_PROGRAM );						// Снимаем сигнал PROGRAM
  	while( !PIO_Get( &_PC06_XIL_INIT ) );			// Ждем, когда XILINX очистит память и отпустит INIT
	}
  WaitByTimerMks( 10 );

  for( i=0; ; i += len )
  {
    v0 = ( i >= 77824 );
    if( i >= 77824 )
      v0 = ( i - 77824 >= 932 );

    if( v0 )
      break;

    while( !(_USB_Int_Flag.VALUE & eUSBMSK_EP2RX_CHECK) );

    AT91C_BASE_AIC->AIC_IDCR = 1;
    _USB_Int_Flag.VALUE &= eUSBMSK_EP2RX_CLEAR;
    AT91C_BASE_AIC->AIC_IECR = 1;

    USB_ENDP_INDEX = 4;
    len = USB_BUF_LEN;

    for( int j=0; j<len; j += 2 )
    {
      data = USB_DATA_PORT;

			for( int k=0; k<16; k++ )
      {
        AT91C_BASE_PIOA->PIO_CODR = 0x80;			// _PA07_XIL_CCLK = 0
				{
	        if( data & 1 )
  	        AT91C_BASE_PIOB->PIO_SODR = 0x40;	// _PB06_XIL_DIN = 1
    	    else
      	    AT91C_BASE_PIOB->PIO_CODR = 0x40;	// _PB06_XIL_DIN = 0
				}
        AT91C_BASE_PIOA->PIO_SODR = 0x80;			// _PA07_XIL_CCLK = 0

				data >>= 1;
      }
    }
  }

	// Проверяем, прошла ли проверка CRC
	//
  if( !PIO_Get( &_PC06_XIL_INIT ) )
  {
    LCD_Print( eLCD_LINE2, "CRC Error!      " );
    Beep( 200 );
    while( 1 );
  }

  PIO_Configure( &_PA14_XIL_DONE_InUp );			// Отпускаем сигнал DONE для перевода XILINX в рабочий режим.
  WaitByTimerMks( 2 );

  for( i=0; i<2; i++ )
  {
    PIO_Clear( &_PA07_XIL_CCLK );
    PIO_Set( &_PA07_XIL_CCLK );
  }
}


void USB_ReceiveBuf_EP1RX_Parse()
{
  if( !USB_ReceiveBuf_EP1RX() )
    return;

  if( !_is_EP1RX_Received )
		_is_EP1RX_Received = 1;

  _isAdapterError = 0;

	LEDS_BEEP_OnOff( eLED_RUN );
	
	switch( _usb_BufCmd_EP1RX.Cmd )
	{
		case 1:
		{
			PROGPROC_Parse();
		}
		break;

		case 2:
		{
			USB_LibFile_Receive();
		}
		break;

		case 5:
		{
			USB_BlockMemory_Receive();            //
		}
		break;

		case 6:
		{
			USB_BlockMemory_Send();               //
		}
		break;

		case 8:
		{
			SOCKET_CkeckInsertIC();
		}
		goto LABEL_46;

		case 11:
		{
			LEDS_BEEP_OnOff( eLEDS_OFF );
			_usb_BufCmd_EP1RX.BaseAddr = (BYTE*)&_chipInSocket;
			USB_BlockMemory_Receive();
		}
		break;

		case 12:
		{
			LEDS_BEEP_OnOff( eLEDS_OFF );
			_usb_BufCmd_EP1RX.BaseAddr = (BYTE*)&_chipInSocket;
			USB_BlockMemory_Send();
		}
		break;

		case 15:
		{
			USB_SendProgFeatureNames();
		}
		break;

		case 18:
		{
			LEDS_BEEP_OnOff( eLEDS_ON );
			WaitByTimerMs( 250 );
			LEDS_BEEP_OnOff( eLEDS_OFF );
			WaitByTimerMs( 200 );
		}
		break;

		case 79:
		{
			_is_EP1RX_Received = 2;
		}
		break;

		case 136:
		{
			_isAdapterError = _usb_BufCmd_EP1RX.ProgProcNum & 1;
			if( _usb_BufCmd_EP1RX.ProgProcNum & 1 )
				LEDS_BEEP_OnOff( eLED_ERROR );
			else
				LEDS_BEEP_OnOff( eLED_PASS );

			SOCKET_CkeckInsertIC();
		}
		goto LABEL_46;

		default:
		break;
	}

	if( _isAdapterError )
	{
		_usb_Buf_EP1TX[0] = 4;
		if( _mesageType == 'i' )
			_usb_Buf_EP1TX[0] = 0xFF04;
	}
	else
	{
		_usb_Buf_EP1TX[0] = 3;
	}

LABEL_46:
	USB_SendBuf_EP1TX();

//LABEL_47:
  if( _usb_BufCmd_EP1RX.Cmd != 11 && _usb_BufCmd_EP1RX.Cmd != 12 )
  {
    if( _usb_BufCmd_EP1RX.Cmd == 136 )
      _isAdapterError = _usb_BufCmd_EP1RX.ProgProcNum & 1;

		LED_On_AdapterStatus();
  }
}


void USB_WaitEP2TX()
{
	while( !( _USB_Int_Flag.VALUE & eUSBMSK_EP2TX_CHECK ) );

	AT91C_BASE_AIC->AIC_IDCR = 1;
	_USB_Int_Flag.VALUE &= eUSBMSK_EP2TX_CLEAR;
	AT91C_BASE_AIC->AIC_IECR = 1;
}

void USB_WaitEP2RX()
{
	while( !(_USB_Int_Flag.VALUE & eUSBMSK_EP2RX_CHECK) );

	AT91C_BASE_AIC->AIC_IDCR = 1;
	_USB_Int_Flag.VALUE &= eUSBMSK_EP2RX_CLEAR;
	AT91C_BASE_AIC->AIC_IECR = 1;
}
