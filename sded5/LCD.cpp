#include "LCD.h"

const char _aPrvbestd12345u[]	= "prvbestd12345uoULmijzf";

const char* _infoMessages[] =
{
	"PROGRAMMING...",
	"READING...",
	"VERIFYING...",
	"BLANKCHECKING...",
	"ERASING...",
	"SECURITY...",
	"ENCRYPTION...",
	"PROTECTING...",
	"LOCK BIT 1...",
	"LOCK BIT 12...",
	"LOCK BIT 123...",
	"SECTOR ERASING...",
	"SECTOR PROTECT...",
	"UNPROTECTING...",
	"OPERATING...",
	"VERIFYING(VCCH)",
	"VERIFYING(VCCL)"
};

const char* _okMessages[] =
{
	"PROGRAM OK!",
	"READ OK!",
	"VERIFY OK!",
	"BLANKCHECK OK!",
	"ERASE OK!",
	"SECURITY OK!",
	"ENCRYPTION OK!",
	"PROTECT OK!",
	"LOCK BIT 1 OK!",
	"LOCK BIT 12 OK!",
	"LOCK BIT 123 OK!",
	"SECTOR ERASE OK!",
	"PRO SECTOR OK!",
	"UNPROTECT OK!",
	"OPERATE OK!",
	"VERIFY(VCCH) OK!",
	"VERIFY(VCCL) OK!"
};


const char* _errorMessages[] =
{
	"PROGRAM ERROR!",
	"READ ERROR!",
	"VERIFY ERROR!",
	"BLANKCHECK ERROR!",
	"ERASE ERROR!",
	"SECURITY ERROR!",
	"ENCRYPTION ERROR!",
	"PROTECT ERROR!",
	"LOCK BIT 1 ERROR!",
	"LOCK BIT 12 ERR!",
	"LOCK BIT 123 ERR!",
	"SECTOR ERASE ERR!",
	"PRO SECTOR ERR!",
	"UNPROTECT ERROR!",
	"OPERATE ERROR!",
	"VERIFY(VCCH)ERR!",
	"VERIFY(VCCL)ERR!",
	"OFF LINE MODE",
	"ID ERROR!"
};


const Pin _PA16_LCD_E							= { (1 << 16),                     AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT };
const Pin _PA17_LCD_RW						= { (1 << 17),                     AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT };
const Pin _PA18_LCD_RS						= { (1 << 18),                     AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT };
const Pin _PB25_DAC_LCD_D7_Z			= { (1 << 25),                     AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT };	// По 1 - переключение D7 на ввод для чтения.
const Pin _PA16_17_18_LCD_E_RW_RS	= { (1 << 16)|(1 << 17)|(1 << 18), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT };

__no_init BYTE _leds_beep_current;
__no_init BYTE _LCD_isBusy;


void LCD_Init()
{
  _LCD_isBusy = 0;

	PIO_Configure( &_PA16_LCD_E );
  PIO_Configure( &_PA17_LCD_RW );
  PIO_Configure( &_PA18_LCD_RS );
  PIO_Configure( &_PB25_DAC_LCD_D7_Z );

  LCD_WriteInstruction( 1 );									// Clear LCD Display
  LCD_WriteInstruction( 0x38 );								// Function set: 8-bit/2-line/5x8-font size
  LCD_WriteInstruction( 0x0E );								// Display On: Cursor On
  LCD_WriteInstruction( 6 );									// Entry Mode Set
}


// Ожидаем с таймаутом, когда LCD модуль освободится для обработки комманд.
// Если вышли по таймауту, взведется флаг _LCD_isBusy
//
void LCD_WaitForReady()
{
  unsigned int v0;
  char v1;
  char v2;
  char v3;

  v0 = 0;
	while( !_LCD_isBusy )
  {
    PIO_Set( &_PA17_LCD_RW );									// Set Read Mode
    PIO_Set( &_PB25_DAC_LCD_D7_Z );						// Переключаем D7 на ввод.
    PIO_Clear( &_PA18_LCD_RS );								// Select Instruction Register
    PIO_Clear( &_PA16_LCD_E );								// Clear Latch Signal

    v1 = ( v0 >= 8192 );
    v2 = ( v0 == 8192 );
		if( v0 >= 8192 )
    {
      v1 = ( ( v0 - 8192 ) >= 3808 );
      v2 = ( v0 == 12000 );
    }

    if( !v2 & v1 )
      _LCD_isBusy = 1;
    else
      v0++;

    PIO_Set( &_PA16_LCD_E );									// Prepare for Latch
    WaitByTimerMks( 2 );

    v3 = _U13_DAC_LCD_Data;
    PIO_Clear( &_PA16_LCD_E );								// Latch Data

    if( !(v3 & 0x80) )												// Check busy flag
    {
      WaitByTimerMks(2);
      return;
    }
  }
}


void LCD_PrintNextChar( char ch )
{
	LCD_WaitForReady();
	if( _LCD_isBusy )
		return;

	PIO_Clear( &_PA16_LCD_E );									// Clear Latch Signal
	PIO_Clear( &_PA17_LCD_RW );									// Set Write Mode
	PIO_Clear( &_PB25_DAC_LCD_D7_Z );						// Переключаем D7 на вывод.
	WaitByTimerMks( 2 );

	PIO_Set( &_PA18_LCD_RS );										// Select Data Register
	PIO_Set( &_PA16_LCD_E );										// Prepare for Latch
	WaitByTimerMks( 2 );

	_U13_DAC_LCD_Data = ch;
	WaitByTimerMks( 2 );

	PIO_Clear( &_PA16_LCD_E );									// Latch Data
	WaitByTimerMks( 2 );
}


void LCD_WriteInstruction( BYTE instr )
{
	LCD_WaitForReady();
	if( _LCD_isBusy )
		return;

	PIO_Clear( &_PA16_17_18_LCD_E_RW_RS );			// Clear Latch Signal, Set Write Mode, Select Instruction Register
	PIO_Clear( &_PB25_DAC_LCD_D7_Z );						// Переключаем D7 на вывод.
	WaitByTimerMks( 2 );

	PIO_Set( &_PA16_LCD_E );										// Prepare for Latch
	WaitByTimerMks( 2 );

	_U13_DAC_LCD_Data = instr;
	WaitByTimerMks( 2 );

	PIO_Clear( &_PA16_LCD_E );									// Latch Data
	WaitByTimerMks( 2 );
}


void LCD_PrintChar( eLCD_LINE lcdLine, BYTE pos, char ch )
{
  LCD_WriteInstruction( lcdLine + pos );	// Set Display Data Ram Address
  LCD_PrintNextChar( ch );
}


void LCD_Print( eLCD_LINE lcdLine, const char* str )
{
	int i;
  char ch;

	if( !_LCD_isBusy )
  {
    LCD_PrintChar( lcdLine, 19, ' ' );
    LCD_WriteInstruction( lcdLine );

    for( i=0; i<20; i++ )
    {
      if( *str )
      {
        ch = *(str++);
        LCD_PrintNextChar( ch );
      }
      else
      {
        LCD_PrintNextChar( ' ' );
      }
    }

    LCD_WriteInstruction( 16 );
  }
}


void LED_On_AdapterStatus()
{
	if( _isAdapterError )
		LEDS_BEEP_OnOff( eLED_ERROR );
	else
		LEDS_BEEP_OnOff( eLED_PASS );
}


void LEDS_BEEP_OnOff( BYTE led )
{
	_leds_beep_current = led;
//!!!	_U13_LEDS_PASS_RUN_ERROR_BEEP = led;
}


void Beep_AdapterStatus()
{
	if( _isAdapterError )
	{
		Beep( 200 );
		WaitByTimerMs( 500 );
		Beep( 200 );
	}
	else
	{
		Beep( 100 );
	}
}


void LCD_PrintMessage( bool isOkMessage )
{
	const char** v1;
	unsigned int i;

  if ( _isAdapterError )
    v1 = _errorMessages;
	else if( isOkMessage )
		v1 = _okMessages;
  else
    v1 = _infoMessages;

  // Program, Read, Verify, Blankcheck, Erase, ...
  //  
	for( i=0; i<sizeof( _aPrvbestd12345u ) && _aPrvbestd12345u[i] != _mesageType; i++ );

	if( i < sizeof( _aPrvbestd12345u ) )
  	LCD_Print( eLCD_LINE1, v1[i] );
	else
	LCD_Print( eLCD_LINE1, "                " );
		
	LCD_Print( eLCD_LINE2, "                " );

	LCD_PrintChar( eLCD_LINE1, 18, ' ' );
}
