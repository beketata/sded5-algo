#pragma once

#include "common.h"
#include "main.h"
#include "doch_ata.h"


extern "C"
{
	void PROGPROC_Parse();
	void PROGPROC_Test();

	void PROGPROC_FLWRITE_IO_WORD();
	void PROGPROC_FLREAD_IO_WORD();
	void PROGPROC_hal_blk_read_nor();
	void PROGPROC_hal_blk_write_nor();
		
	void PROGPROC_PowerOn();
	WORD IC_Read( DWORD addr );
	void IC_Write( DWORD addr, WORD data );
	void SetMemory( void* buf, int size, Byte bt );
}
