#include "doch_ata.h"


DOCH_Error ready( DOCH_Reg reg, Byte mask, Byte on_bits, Dword milliseconds )
{
	Byte status;
	Dword ms = 0;

	AT91C_BASE_PMC->PMC_PCER = 0x20000;
	DisableTimer( AT91C_BASE_TC0, 0x4002 );
	AT91C_BASE_TC0->TC_RC = (31*1000/*mks*/)/10;
	AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN;
	
	while( true )
	{
		status = DOCHREAD_ATA_REG( reg );

		if( ( status & mask ) == on_bits )
		{
			// AT91_REG cv = AT91C_BASE_TC0->TC_CV;
			return DOCH_OK;
		}

		/*If an error was detected - return the error code*/
		if( ( ( status & DOCH_ERROR ) == DOCH_ERROR ) && ( ( status & DOCH_BUSY ) != DOCH_BUSY ) )
		{
			DOCH_Error error = (DOCH_Error)DOCHREAD_ATA_REG( DOCH_ERROR_REG );

			/*Special Case - Device returned "0" in the error register.
			 This DOES NOT mean that everything is OK, since the error bit was raised.
			 Hence, we will return "DOCH_ATA_ERROR_ABORT"*/
			if( error == DOCH_OK )
				error = DOCH_ATA_ERROR_ABORT;

			return error;
		}

		if( AT91C_BASE_TC0->TC_SR & AT91C_TC_CPCS )
		{
			ms++;
			
			DisableTimer( AT91C_BASE_TC0, 0x4002 );
			AT91C_BASE_TC0->TC_RC = (31*1000/*mks*/)/10;
			AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN;
		}
		
		if( milliseconds == ms )
			return DOCH_TimedOut;
	}
}
