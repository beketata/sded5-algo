#pragma once

#include "common.h"
#include "main.h"
#include "PROG_USB.h"


#define _LEDS_PASS_RUN_ERROR_BEEP (*(BYTE*)0x30040000)

extern const Pin _PA16_LCD_E;
extern const Pin _PA17_LCD_RW;
extern const Pin _PA18_LCD_RS;
extern const Pin _PB25_DAC_LCD_D7_Z;

extern BYTE _LCD_isBusy;
extern BYTE _leds_beep_current;


extern "C"
{
	void LCD_Init();
	void LCD_WriteInstruction( BYTE );
	void LCD_PrintNextChar( char );
	void LCD_Print( eLCD_LINE, const char* );
	void LCD_PrintMessage( bool isOkMessage );
	void LED_On_AdapterStatus();
	void LEDS_BEEP_OnOff( BYTE led );
	void Beep_AdapterStatus();
}
