#include "PROGPROC.h"


__no_init BYTE _icCtrl;
__no_init BYTE _isInProgProc;


typedef struct
{
	const char Letter;
	void (* const Proc)();
} PROG_PROC;

const PROG_PROC _progProcedures[] =
{
	{ 'i', PROGPROC_Test },											//  0
	{ 'w', PROGPROC_FLWRITE_IO_WORD },					//  1
	{ 'r', PROGPROC_FLREAD_IO_WORD },						//  2
	{ '.', PROGPROC_hal_blk_write_nor },				//  3
	{ '.', PROGPROC_hal_blk_read_nor },					//  4
	{ '.', 0 },																	//  5
	{ '.', 0 },																	//  6
	{ '.', 0 },																	//  7
	{ '.', 0 },																	//  8
	{ '.', 0 },																	//  9
	{ '.', 0 },																	// 10
	{ '.', 0 },																	// 11
	{ '.', 0 },																	// 12
	{ '.', 0 },																	// 13
	{ '.', PROGPROC_PowerOn },									// 14
	{ '.', SOCKET_PowerOff }										// 15
};


#ifdef _MYXIL_

WORD IC_Read()
{
	register Word data;
	
	LogLine( "IC_Read()" );

	_icCtrl &= ~0x02; _IC_CTRL = _icCtrl;				// OE = 0
	data = _IC_DATA;														// Считываем данные.
	_icCtrl |= 0x02; _IC_CTRL = _icCtrl;				// OE = 1
	
	return data;
}

void IC_Write( WORD data )
{
	_IC_DATA = data;														// Выводим данные.
	_icCtrl &= ~0x01; _IC_CTRL = _icCtrl;				// WE = 0
	_icCtrl |= 0x01; _IC_CTRL = _icCtrl;				// WE = 1	// Записываем данные
}

void IC_Reset()
{
	int i;

	LogLine( "IC_Reset()" );
	
	_icCtrl = 0xF7; _IC_CTRL = _icCtrl;					// RSTIN = 0; E5 ST/PU Reset, active low. Input
	{
		WaitByTimerMks( 1 );

		// Ждем, когда появится сигнал BUSY
		//
		i = 0;
		while( _IC_CTRL & 1 )
		{
			i++;
		}
		
		WaitByTimerMks( 1 );
	}
	_icCtrl = 0xFF; _IC_CTRL = _icCtrl;					// RSTIN = 1; E5 ST/PU Reset, active low. Input

	WaitByTimerMks( 1 );

	// Ждем, когда пропадет сигнал BUSY
	//
	i = 0;
	while( ( _IC_CTRL & 1 ) == 0 )
	{
		i++;
	}

	WaitByTimerMks( 1 );
}

#elif _NOXIL_

// Ножки микросхемы SDED5
//
//  80 pin - JP1
// 100 pin - JP2
//
const BYTE _pin_VCC1 = 88;										// E7 2.77 - VCC1

const BYTE _pin_CE = 22;											// J2 1.23 - Chip Enable, active low. Input
const BYTE _pin_OE = 36;											// J3 1.37 - Output Enable, active low. Input
const BYTE _pin_WE = 73;											// D6 2.94 - Write Enable, active low. Input
const BYTE _pin_GPIO_TIMER = 4;								// E1 1.5  - RSRVD. This signal may be left floating or pulled up. Input/output
const BYTE _pin_ID0 = 104;										// G8 2.61 - Identification. Configuration control to support up to two chips cascaded in the same memory window. Input
																							// Chip 1: ID0 = VSS
																							// Chip 2: ID0 = VCCQ
const BYTE _pin_LOCK = 103;										// F8 2.64 - Lock, active low. When active, provides full hardware data protection of selected partitions. Input

const BYTE _pin_WARM_RST = 120;								// J9 2.45 - RSRVD. This signal may be left floating or pulled up. Input
const BYTE _pin_BUSY = 61;										// F5 1.62 - CMOS output Busy. Active low. Indicates that mDOC is initializing and should not be accessed. Output
const BYTE _pin_RSTIN = 60;										// E5 1.61 - ST/PU Reset, active low. Input
const BYTE _pin_CLK = 80;											// L6 2.85 - ST External clock input used for burst mode data transfers. If not used may be left floating. Input
const BYTE _pin_DMARQ = 105;									// H8 2.62 - CMOS output DMA request. If not used may be left floating. Output
const BYTE _pin_IRQ = 118;										// G9 2.47 - CMOS output Interrupt Request. Active low. If not used may be left floating. Output

// Serial Interface
//
const BYTE _pin_SCS = 132;										// G10 2.33 - ST/PU/CMOS 3-STATE Serial Interface chip select. Active low. If not used may be left floating. Input/Output
const BYTE _pin_SO = 133;											// H10 2.34 - ST/PU/CMOS 3-STATE Serial Interface data out (In Serial slave mode). If not used may be left floating. Output/Input
const BYTE _pin_SI = 134;											// J10 2.31 - ST/PU/CMOS 3-STATE Serial Interface data in (In serial slave mode). If not used may be left floating. Input/Output
const BYTE _pin_SCLK = 135;										// K10 2.32 - ST/PU/CMOS 3-STATE Serial Interface clock. If not used may be left floating. Input/Output

// Бит адреса (с A1) - ножка из 144
//
BYTE const _icAddr[] = { 20, 19, 18, 34, 33, 32, 31, 87, 89, 90, 101, 102 };	// A1 ... A12
BYTE* const _icAddrToSet = 0;
BYTE const _icAddrToClear[] = { 21, 47, 46, 130, 131 };	// A0, A13 ... A16

//const BYTE _icAddr[] = { 20, 19, 18, 34 };		// A1, A2, A3, A4
//const BYTE _icAddrToSet[] = { 101 };					// A11
//const BYTE _icAddrToClear[] = { 21, 33, 32, 31, 87, 89, 90, 102, 47, 46, 130, 131 };	// A0, A5 .... A10, A12 ... A16 

// Бит данных (с D0) - ножка из 144
//
const BYTE _icData[16] = { 37, 49, 52, 64, 78, 94, 91, 107, 38, 50, 51, 66, 93, 92, 108, 106 };


void IC_DataIn()
{
	LogLine( "IC_DataIn()" );

	// Настраиваем ножки данных на ввод.
	//
	for( int i=0; i<sizeof( _icData ); i++ )
	{
		XIL_SOCKET_PIN_NUM = _icData[i];					// Выбираем очередную ножку данных у микросхемы в сокете.
		XIL_SOCKET_PIN_STATE = 2;									// Настраиваем ножку на ввод.
	}
}

void IC_SetAddr( DWORD addr )
{
	int i;

	LogLine( "IC_SetAddr()" );

	addr >>= 1;																	// Т.к. шина данных 16-и разрядная и A0 посажен на GND.

	for( i=0; i<sizeof( _icAddr ); i++ )
	{
 		XIL_SOCKET_PIN_NUM = _icAddr[i];					// Выбираем очередную адресную ножку у микросхемы в сокете.
  	XIL_SOCKET_PIN_STATE = (BYTE)( ( addr >> i ) & 1 );	// Выводим на ножку 0 или 1
	}
}


WORD IC_GetData()
{
	int i;
	WORD data = 0;

	for( i=0; i<sizeof( _icData ); i++ )
	{
		XIL_SOCKET_PIN_NUM = _icData[i];					// Выбираем очередную ножку данных у микросхемы в сокете.
		data |= ( ( XIL_SOCKET_PIN_STATE & 1 ) << i );
	}

	return data;
}


void IC_SetData( WORD data )
{
	int i;

	for( i=0; i<sizeof( _icData ); i++ )
	{
		XIL_SOCKET_PIN_NUM = _icData[i];					// Выбираем очередную ножку данных у микросхемы в сокете.
		XIL_SOCKET_PIN_STATE = ( ( data >> i ) & 1 );
	}
}

void IC_SetDefaultPinsLevel()
{
	int i;

	LogLine( "IC_SetDefaultPinsLevel()" );

	XIL_SOCKET_PIN_NUM = _pin_VCC1;
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод.

	// Настраиваем неиспользуемые адресные ножки.
	//
	if( _icAddrToClear )
	{
		for( i=0; i<sizeof(_icAddrToClear); i++ )
		{
			XIL_SOCKET_PIN_NUM = _icAddrToClear[i];
			XIL_SOCKET_PIN_STATE = 0;								// = 0
		}
	}

	if( _icAddrToSet )
	{
		for( i=0; i<sizeof(_icAddrToSet); i++ )
		{
			XIL_SOCKET_PIN_NUM = _icAddrToSet[i];
			XIL_SOCKET_PIN_STATE = 1;								// = 1
		}
	}

	// Настраиваем ножки данных на ввод.
	//
	IC_DataIn();

	XIL_SOCKET_PIN_NUM = _pin_GPIO_TIMER;
	XIL_SOCKET_PIN_STATE = 2;										// _pin_GPIO_TIMER = Pulled Up
	
	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 1;										// OE = 1 (inactive)

	XIL_SOCKET_PIN_NUM = _pin_CE;
	XIL_SOCKET_PIN_STATE = 1;										// CE = 1 (inactive)

	XIL_SOCKET_PIN_NUM = _pin_WE;
	XIL_SOCKET_PIN_STATE = 1;										// WE = 1 (inactive)
	
	XIL_SOCKET_PIN_NUM = _pin_LOCK;
	XIL_SOCKET_PIN_STATE = 1;										// LOCK = 1 (inactive)
	
	XIL_SOCKET_PIN_NUM = _pin_ID0;
	XIL_SOCKET_PIN_STATE = 0;										// ID0 = 0 (Chip 1)
	
	// Control Pins
	
	XIL_SOCKET_PIN_NUM = _pin_WARM_RST;					// J9 RSRVD. This signal may be left floating or pulled up. Input
	XIL_SOCKET_PIN_STATE = 1;										// = 1 (inactive)

	XIL_SOCKET_PIN_NUM = _pin_BUSY;							// F5 CMOS output Busy. Active low. Indicates that mDOC is initializing and should not be accessed. Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод.

	XIL_SOCKET_PIN_NUM = _pin_RSTIN;						// E5 ST/PU Reset, active low. Input
	XIL_SOCKET_PIN_STATE = 0;										// = 0 (active)

	XIL_SOCKET_PIN_NUM = _pin_CLK;							// L6 ST External clock input used for burst mode data transfers. If not used may be left floating. Input
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up

	XIL_SOCKET_PIN_NUM = _pin_DMARQ;						// H8 CMOS output DMA request. If not used may be left floating. Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up

	XIL_SOCKET_PIN_NUM = _pin_IRQ;							// G9 CMOS output Interrupt Request. Active low. If not used may be left floating. Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up
	
	// Serial Interface
	
	XIL_SOCKET_PIN_NUM = _pin_SCS;							// G10 ST/PU/CMOS 3-STATE Serial Interface chip select. Active low. If not used may be left floating. Input/Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up

	XIL_SOCKET_PIN_NUM = _pin_SO;								// H10 ST/PU/CMOS 3-STATE Serial Interface data out (In Serial slave mode). If not used may be left floating. Output/Input
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up

	XIL_SOCKET_PIN_NUM = _pin_SI;								// J10 ST/PU/CMOS 3-STATE Serial Interface data in (In serial slave mode). If not used may be left floating. Input/Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up

	XIL_SOCKET_PIN_NUM = _pin_SCLK;							// K10 ST/PU/CMOS 3-STATE Serial Interface clock. If not used may be left floating. Input/Output
	XIL_SOCKET_PIN_STATE = 2;										// Настраиваем ножку на ввод. Pulled Up
}


void IC_Reset()
{
	int i;

	LogLine( "IC_Reset()" );

	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 1;										// OE = 1 (inactive)

	XIL_SOCKET_PIN_NUM = _pin_CE;
	XIL_SOCKET_PIN_STATE = 1;										// CE = 1 (inactive)
	
	XIL_SOCKET_PIN_NUM = _pin_RSTIN;						// E5 ST/PU Reset, active low. Input
	XIL_SOCKET_PIN_STATE = 0;										// = 0 (active)
	{
		WaitByTimerMks( 1 );

		// Ждем, когда появится сигнал BUSY
		//
		i = 0;
		XIL_SOCKET_PIN_NUM = _pin_BUSY;
		while( XIL_SOCKET_PIN_STATE & 1 )
		{
			i++;
		}
		
		WaitByTimerMks( 1 );
	}
	XIL_SOCKET_PIN_NUM = _pin_RSTIN;						// E5 ST/PU Reset, active low. Input
	XIL_SOCKET_PIN_STATE = 1;										// = 1 (inactive)

	WaitByTimerMks( 1 );

	// Ждем, когда пропадет сигнал BUSY
	//
	i = 0;
	XIL_SOCKET_PIN_NUM = _pin_BUSY;
	while( ( XIL_SOCKET_PIN_STATE & 1 ) == 0 )
	{
		i++;
	}

	WaitByTimerMks( 1 );
}

void IC_Write( WORD data )
{
	// Выставляем данные.
	//
	IC_SetData( data );

	XIL_SOCKET_PIN_NUM = _pin_WE;
	XIL_SOCKET_PIN_STATE = 0;									// WE = 0

	// Записываем данные.
	//
	XIL_SOCKET_PIN_NUM = _pin_WE;
	XIL_SOCKET_PIN_STATE = 1;									// WE = 1
}

WORD IC_Read()
{
	register Word data;
	
	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 0;									// OE = 0 (active)
	{
		data = IC_GetData();										// Считываем данные.
	}
	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 1;									// OE = 1 (inactive)
	
	return data;
}

#endif


void PROGPROC_Parse()
{
//	WORD volts;

	LogLine( "PROGPROC_Parse()" );

	_isInProgProc = 1;
	_mesageType = 'o';													// Operating

	LCD_WriteInstruction( 1 );             			// Clear LCD Display

	LEDS_BEEP_OnOff( _leds_beep_current | 0x80 );

//	if( _chipInSocket.VCC_XilinxVCCO )
//		volts = 5*_chipInSocket.VCC_XilinxVCCO + 2;
//	else
//	volts = DAC_VOLT_4( 3.33 );								// 170

//	POWER_SetVCCs( 3, volts );									// Устанавливаем напряжение для выходных драйверов XILINX

	if( !_isAdapterError )
  {
		_usb_Buf_EP1TX[0] = 3;
		_usb_Buf_EP1TX[1] = 0;

		if( _usb_BufCmd_EP1RX.ProgProcNum < sizeof_array( _progProcedures ) )
		{
			_mesageType = _progProcedures[_usb_BufCmd_EP1RX.ProgProcNum].Letter;
			_progProcedures[_usb_BufCmd_EP1RX.ProgProcNum].Proc();
		}

//!!!		LCD_PrintMessage( 1 );
	}

  _isInProgProc = 0;
}


WORD IC_Read( DWORD addr )
{
	register Word data;
	
	LogLine( "IC_Read()" );

#ifdef _MYXIL_
	
	_IC_ADDR = addr;
	_icCtrl &= ~0x02; _IC_CTRL = _icCtrl;			// OE = 0
	data = _IC_DATA;													// Считываем данные.
	_icCtrl |= 0x02; _IC_CTRL = _icCtrl;			// OE = 1

#elif _NOXIL_	

	// Выставляем адрес на адресные ножки.
	//
	IC_SetAddr( addr );

	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 0;									// OE = 0 (active)
	{
		data = IC_GetData();										// Считываем данные.
	}
	XIL_SOCKET_PIN_NUM = _pin_OE;
	XIL_SOCKET_PIN_STATE = 1;									// OE = 1 (inactive)

#else

	_IC_ADDR = addr;
	data = _IC_DATA_RD;

#endif

	return data;
}

void IC_Write( DWORD addr, WORD data )
{
#ifdef _MYXIL_	

	_IC_ADDR = addr;
	_IC_DATA = data;													// Выводим данные.
	_icCtrl &= ~0x01; _IC_CTRL = _icCtrl;			// WE = 0
	_icCtrl |= 0x01; _IC_CTRL = _icCtrl;			// WE = 1	// Записываем данные

#elif _NOXIL_	

	// Выставляем адрес и данные.
	//
	IC_SetAddr( addr );
	IC_SetData( data );

	XIL_SOCKET_PIN_NUM = _pin_WE;
	XIL_SOCKET_PIN_STATE = 0;									// WE = 0

	// Записываем данные.
	//
	XIL_SOCKET_PIN_NUM = _pin_WE;
	XIL_SOCKET_PIN_STATE = 1;									// WE = 1

	IC_DataIn();
	
#else

	_IC_ADDR = addr;
	_IC_DATA_WR = data;

#endif
}


void PROGPROC_PowerOn()
{
	LogLine( "PROGPROC_PowerOn()" );
	
	_leds_beep_current = eLED_RUN;
	_U13_LEDS_PASS_RUN_ERROR_BEEP = _leds_beep_current | 0x80;	// !!! 0x80 как-то активизирует XILINX

	SOCKET_PowerOff();

#ifdef _MYXIL_

	SOCKET_PowerOn();

	_IC_ENABLE = 0;															// Разрешаем работу XILINX

	IC_Reset();

	_icCtrl = 0xFB;
	_IC_CTRL = _icCtrl;													// CE = 0
	
#elif _NOXIL_

	IC_SetDefaultPinsLevel();
	
	SOCKET_PowerOn();

	IC_Reset();

	XIL_SOCKET_PIN_NUM = _pin_CE;
	XIL_SOCKET_PIN_STATE = 0;

#else

	SOCKET_PowerOn();

	_IC_RSTIN = 1;
  _IC_LOCK = 1;
  _IC_CE = 0;
  word_30000002 = 0;
  _IC_ADDR = 0;

	_IC_RSTIN = 0;
	{
  	WaitByTimerMks( 20 );
	}
  _IC_RSTIN = 1;
	
	// Ждем отпускания сигнала BUSY у SDED5 после сброса.
	//
	int i = 0;
	bool v1;
  for( ; ; i++ )
  {
    v1 = i >= 0x3D000;
    if ( i >= 0x3D000 )
      v1 = i - 0x3D000 >= 0x90;
    if( v1 )
		{
      break;
		}

    WaitByTimerMks( 6 );
    if( _IC_STAT & 0x100 )
      break;
  }

#endif
}


void PROGPROC_FLWRITE_IO_WORD()
{
	DWORD addr = (DWORD)_usb_BufCmd_EP1RX.BaseAddr;
	IC_Write( addr, (WORD)_usb_BufCmd_EP1RX.Size );
}

void PROGPROC_FLREAD_IO_WORD()
{
	DWORD addr = (DWORD)_usb_BufCmd_EP1RX.BaseAddr;
	_usb_Buf_EP1TX[1] = IC_Read( addr );
}

void PROGPROC_hal_blk_read_nor()
{
	WORD sectors = (WORD)_usb_BufCmd_EP1RX.Size;
	const WORD addr = (DWORD)_usb_BufCmd_EP1RX.BaseAddr;

	for( int block=0; block<sectors; block++ )
	{
		USB_ENDP_INDEX = eUSB_ENDP_EP2TX;
		USB_BUF_LEN = 512;

		#ifdef _MYXIL_
		{
			_IC_ADDR = addr;
			for( int i=0; i<256; i++ )
			{
				USB_DATA_PORT = IC_Read();
			}
		}
		#elif _NOXIL_
		{
			IC_SetAddr( addr );
			for( int i=0; i<256; i++ )
			{
				USB_DATA_PORT = IC_Read();
			}
		}
		#else
		{
			for( int i=0; i<256; i++ )
			{
				USB_DATA_PORT = IC_Read( addr );
			}
		}
		#endif

		USB_WaitEP2TX();

		// Ждем готовности после чтения каждого сектора, кроме последнего.
		//
		if( block < ( sectors - 1 ) )
		{
			DOCH_Error err = ready( DOCH_STATUS_REG, ( DOCH_READY | DOCH_DRQ | DOCH_BUSY | DOCH_ERROR ), ( DOCH_READY | DOCH_DRQ ), 1000 );
		}
	}
}

void PROGPROC_hal_blk_write_nor()
{
	DWORD i;
	register Word data;
	WORD block;
	const DWORD addr = (DWORD)_usb_BufCmd_EP1RX.BaseAddr;
	WORD sectors = (WORD)_usb_BufCmd_EP1RX.Size;

	for( block=0; block<sectors; block++ )
	{	
		USB_ENDP_INDEX = eUSB_ENDP_EP2RX;
		USB_BUF_LEN = 512;
		
		USB_WaitEP2RX();

		// Ждем готовности перед записью каждого сектора, кроме первого.
		//
//		if( block )
		{
			DOCH_Error err = ready( DOCH_STATUS_REG, ( DOCH_READY | DOCH_DRQ | DOCH_BUSY | DOCH_ERROR ), ( DOCH_READY | DOCH_DRQ ), 1000 );
		}

		#ifdef _MYXIL_
		{
			_IC_ADDR = addr;
			for( i=0; i<256; i++ )
			{
				data = USB_DATA_PORT;
				IC_Write( data );
			}
		}
		#elif _NOXIL_
		{
			IC_SetAddr( addr );
			for( i=0; i<256; i++ )
			{
				data = USB_DATA_PORT;
				IC_Write( data );
			}

			IC_DataIn();
		}
		#else
		{
			for( i=0; i<256; i++ )
			{
				data = USB_DATA_PORT;
				IC_Write( addr, data );
			}
		}
		#endif
	}
}


void ClearMemory( void* buf, int size )
{
	for( int i=0; i<size; i++ )
	{
		((Byte*)buf)[i] = 0;
	}
}

void SetMemory( void* buf, int size, Byte bt )
{
	for( int i=0; i<size; i++ )
	{
		((Byte*)buf)[i] = bt;
	}
}


void PROGPROC_Test()
{
	PROGPROC_PowerOn();
	SOCKET_PowerOff();
}
