#pragma once

#include "common.h"
#include "LCD.h"
#include "D14.h"


extern WORD				_usb_Buf_EP1TX[32];
extern BYTE				_isAdapterError;
extern BYTE				_isInProgProc;


extern "C"
{
	void LowLevelInit();
	void SDRAM_Init();
	void ISR_Setup( BYTE, DWORD, DWORD );
	void Beep( int );
	void DBGU_Init( int a1, int a2, unsigned int a3 );

	void PROGPROC_Parse();
	void SOCKET_CkeckInsertIC();
	void WaitByTimerMks( int );

	void SMC_Setup();

	void ISR_Setup( BYTE, DWORD, DWORD );
	void ISR_Enable( BYTE );

	void	WaitByTimerMks( int );
	void	WaitByTimerMs( int );

	void POWER_SetVCCs( DWORD a1, WORD verifyingVccCurrent );
	void DisableTimer( AT91PS_TC pTC, AT91_REG tc_cmr );
}
