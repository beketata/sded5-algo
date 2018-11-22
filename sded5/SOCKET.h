#pragma once

#include "common.h"
#include "main.h"
#include "PIO.h"
#include "PROG_USB.h"


extern BYTE _pins_Power[144];
extern BYTE _pins_GND[144];
extern PROG_STRUC	_chipInSocket;


extern "C"
{
	void SOCKET_Keys_Release();
	void SOCKET_CkeckInsertIC();
	void SOCKET_Keys_Set_200024AC( DWORD, int );
	void SOCKET_Keys_Set_V3H_GNG();
	void SOCKET_SetupPowerPins();
	void SOCKET_PowerOn();
	void SOCKET_PowerOff();
}
