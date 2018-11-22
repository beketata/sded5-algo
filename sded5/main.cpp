#include "main.h"


__no_init BYTE _state;
__no_init BYTE _sendByteCnt;
__no_init BYTE _getByteCnt;
__no_init BYTE _sendBuf[9];
__no_init BYTE _reqType;
__no_init BYTE _isAdapterError;

const Pin _PC11              = { (1 << 11),           AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT };	// p.21 XELTEK
const Pin _PC13              = { (1 << 13),           AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_PULLUP };
const Pin _PB14_15           = { (1 << 14)|(1 << 15), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT };	// p.12(RX), p.10(TX) SP3232

const Pin _PB20	             = { (1 << 20),           AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT };	// p.26 XELTEK
const Pin _PB21	             = { (1 << 21),           AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT };	// p.19 XELTEK
const Pin _PB22              = { (1 << 22),           AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT,    PIO_PULLUP };		// p.32 XELTEK
const Pin _PC03	             = { (1 <<  3),           AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT,    PIO_PULLUP };		// p.20 XELTEK


//------------------------------------------------------------------------------
/// Default handler for fast interrupt requests. Infinite loop.
//------------------------------------------------------------------------------
__noreturn
void defaultFiqHandler( void )
{
	while( 1 );
}

//------------------------------------------------------------------------------
/// Default handler for standard interrupt requests. Infinite loop.
//------------------------------------------------------------------------------
__noreturn
void defaultIrqHandler( void )
{
	while( 1 );
}


void ISR_Setup( BYTE a1, DWORD a2, DWORD pFn )
{
	AT91C_BASE_AIC->AIC_IDCR = (DWORD)1 << a1;	// (AIC) Interrupt Disable Command Register

	AT91C_BASE_AIC->AIC_SMR[a1] = a2;					// (AIC) Source Mode Register
	AT91C_BASE_AIC->AIC_SVR[a1] = pFn;				// (AIC) Source Vector Register

	AT91C_BASE_AIC->AIC_ICCR = (DWORD)1 << a1;	// (AIC) Interrupt Clear Command Register
}


void ISR_Enable( BYTE a1 )
{
	AT91C_BASE_AIC->AIC_IECR = (DWORD)1 << a1;	// (AIC) Interrupt Enable Command Register
}

void DBGU_Init( int a1, int a2, unsigned int a3 )
{
  AT91C_BASE_DBGU->DBGU_CR = 12;
  AT91C_BASE_DBGU->DBGU_IDR = 0xFFFFFFFF;
  AT91C_BASE_DBGU->DBGU_BRGR = a3/(16*a2);
  AT91C_BASE_DBGU->DBGU_MR = a1;
  AT91C_BASE_DBGU->DBGU_PTCR = 514;
  AT91C_BASE_DBGU->DBGU_CR = 80;
}

void SDRAM_Init()
{
	static const Pin pinsSdram = PINS_SDRAM;
	volatile DWORD *pSdram = (DWORD*)AT91C_EBI_SDRAM;
	register int i;

	// Enable corresponding PIOs
	//
  PIO_Configure( &pinsSdram );

	// Enable EBI chip select for the SDRAM
	//
	DWORD tmp = AT91C_BASE_MATRIX->MATRIX_EBI | AT91C_MATRIX_CS1A_SDRAMC;
	AT91C_BASE_MATRIX->MATRIX_EBI = tmp;

	AT91C_BASE_SDRAMC->SDRAMC_CR =
		AT91C_SDRAMC_NC_9 |
		AT91C_SDRAMC_NR_13 |
		AT91C_SDRAMC_CAS_3 | 
		AT91C_SDRAMC_NB_4_BANKS |
		AT91C_SDRAMC_DBW_32_BITS |
		AT91C_SDRAMC_TWR_3 |
		AT91C_SDRAMC_TRC_8 |
		AT91C_SDRAMC_TRP_3 |
		AT91C_SDRAMC_TRCD_3 |
		AT91C_SDRAMC_TRAS_6 |
		AT91C_SDRAMC_TXSR_9;	// 0x96338379; 1001.0000.0000.0000.0000.0000.0000.0000

  for( i=0; i<1000; i++ );
	
	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_NOP_CMD;			// Perform NOP
  pSdram[0] = 0x00000000;

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_PRCGALL_CMD;	// Set PRCHG AL
  pSdram[0] = 0x00000000;

  for( i=0; i<10000; i++ );

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 1st CBR
	pSdram[1] = 0x00000001;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR =  AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 2 CBR
	pSdram[2] = 0x00000002;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 3 CBR
	pSdram[3] = 0x00000003;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 4 CBR
	pSdram[4] = 0x00000004;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 5 CBR
	pSdram[5] = 0x00000005;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 6 CBR
	pSdram[6] = 0x00000006;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 7 CBR
	pSdram[7] = 0x00000007;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_RFSH_CMD;		// Set 8 CBR
	pSdram[8] = 0x00000008;																				// Perform CBR

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_LMR_CMD;			// Set LMR operation
	pSdram[9] = 0xCAFEDEDE;																				// Perform LMR burst=1, lat=2

	AT91C_BASE_SDRAMC->SDRAMC_TR = 0x2B5;	// (BOARD_MCK * 7) / 1000000);        // Set Refresh Timer

	AT91C_BASE_SDRAMC->SDRAMC_MR = AT91C_SDRAMC_MODE_NORMAL_CMD;	// Set Normal mode
	pSdram[0] = 0x00000000;																				// Perform Normal mode
}


void LowLevelInit()
{
  BYTE i;

	// Initialize main oscillator
	//
	AT91C_BASE_PMC->PMC_MOR = 0x4001;
	while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS ) );
	
	// Initialize PLLA at 792.576 MHz
	// MULA: PLL A Multiplier = 42
	// The PLL A Clock frequency is the PLL A input frequency multiplied by MULA + 1 = 43
	// DIVA: Divider A = 1
	// Divider is bypassed
	// 43 * 18.432 = 792.576 MHz
	//
	AT91C_BASE_PMC->PMC_PLLAR = 0x202A3F01;	// BOARD_CKGR_PLLA | BOARD_PLLACOUNT | BOARD_MULA | BOARD_DIVA;
	while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA ) );
	
	// Initialize PLLB for USB usage at 63.970 MHz (if not already locked)
	// MULB: PLL B Multiplier = 58
	// The PLL B Clock frequency is the PLL B input frequency multiplied by MULB + 1 = 59.
 	// DIVB: Divider B = 17
	// Divider output is the selected clock divided by DIVB = 17
	// ( 59 * 18.432 ) / 17 = 63.970 MHz
	//
	if( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB ) )
	{
		AT91C_BASE_PMC->PMC_PLLBR = 0x103A3F11;	// BOARD_USBDIV | BOARD_CKGR_PLLB | BOARD_PLLBCOUNT | BOARD_MULB | BOARD_DIVB;
		while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB ) );
	}
	
	// Wait for the master clock if it was already initialized
	//
	while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY ) );

	// Switch to fast clock

	// Switch to main oscillator + prescaler
	//
	AT91C_BASE_PMC->PMC_MCKR = 0x205;
	while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY ) );

	// Switch to PLL + prescaler
	//
	AT91C_BASE_PMC->PMC_MCKR = 0x206;
	while( !( AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY ) );

	//Initialize AIC

	// Set All Interrupt Vectors to Dummy func
  //  
  AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
  AT91C_BASE_AIC->AIC_SVR[0] = (int)defaultFiqHandler;

	for( i=1; i<31; i++ )
	{
    AT91C_BASE_AIC->AIC_SVR[i] = (int)defaultIrqHandler;
	}
  AT91C_BASE_AIC->AIC_SPU = (int)USB_ISR_SPU;

	// Unstack nested interrupts
	//
	for( i=0; i<8; i++ )
	{
		AT91C_BASE_AIC->AIC_EOICR = 0;
	}

	// Watchdog initialization
	//
  AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

  // Changes the mapping of the chip so that the remap area mirrors the internal RAM.
	//
	AT91C_BASE_MATRIX->MATRIX_MRCR = AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D;
	
	// Disable RTT and PIT interrupts (potential problem when program A
	// configures RTT, then program B wants to use PIT only, interrupts
	// from the RTT will still occur since they both use AT91C_ID_SYS)
	//
	AT91C_BASE_RTTC->RTTC_RTMR &= ~(AT91C_RTTC_ALMIEN | AT91C_RTTC_RTTINCIEN);
	AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;

	PIO_Configure( &_PC11 );
  PIO_Configure( &_PC13 );

  SMC_Setup();
  PIO_Configure( &_PB14_15 );

	DBGU_Init( 2048, 115200, 0x5E7B800 );

  SDRAM_Init();
}

void __noreturn main()
{
  POWER_SetVCCs( 3, 90 );

  SOCKET_Keys_Release();

	_is_EP1RX_Received = 0;
  _isAdapterError = 0;
  _LCD_isBusy = 0;

  LED_On_AdapterStatus();

  _chipInSocket.AlgoName[0] = 0;
	
  Beep( 100 );
  LEDS_BEEP_OnOff( eLED_PASS );

	LCD_Init();
	LCD_Print( eLCD_LINE1, "SUPERPRO/SP6100N" );
	LCD_Print( eLCD_LINE2, "FIRMWARE VER 1.0" );
	
	USB_Init();

  while( 1 )
  {
    USB_Debug_Mode();
    USB_ReceiveBuf_EP1RX_Parse();
  }
}

void StateInit( WORD reqType )
{
	_reqType = reqType;
	_sendByteCnt = 0;
	_getByteCnt = 0;
	_state = 0;
}


BYTE RS_GetByte_20005AB4()
{
	int i;
	WORD wRes, wTmp, setBits;

	if( _isAdapterError )
		return 0;

	PIO_Clear( &_PB21 );

	i = 0;
	while( true )
	{
		if( !PIO_Get( &_PB22 ) )
			break;

		if( i++ > 0xA00000 )
		{
			_isAdapterError = true;
			return 0;
		}
	}

	PIO_Set( &_PB20 );
	PIO_Clear( &_PB20 );
	while ( !PIO_Get( &_PC03 ) );

	if( PIO_Get( &_PB22 ) )
		_isAdapterError = true;

	// Принимаем 10 бит.
	//
	wRes = 0;
	for( i=0; i<10; i++ )
	{
		PIO_Set( &_PB20 );
		PIO_Clear( &_PB20 );
		while( !PIO_Get( &_PC03 ) );

		PIO_Set( &_PB20 );
		PIO_Clear( &_PB20 );
		while( !PIO_Get( &_PC03 ) );

		if( PIO_Get( &_PB22 ) )
		{
			wRes |= (1 << i);
		}
	}

	for( i=0; i<3; i++ )
	{
		PIO_Set( &_PB20 );
		PIO_Clear( &_PB20 );
		while( !PIO_Get( &_PC03 ) );
	}

	// Считаем количество единиц в данных.
	//
	wTmp = wRes;
	setBits = 0;
	for( i=0; i<8; i++ )
	{
		setBits += ( wTmp & 1 );
		wTmp >>= 1;
	}

	// Проверяем четность.
	//
	if( (setBits & 1) != (wTmp & 1) )
		_isAdapterError = true;

	// Проверяем наличие стоп бита.
	//
	if( !(wTmp & 2) )
		_isAdapterError = true;

	return wRes;
}


void SMC_Setup()
{
	//!!! Проблема была в том, что по документации на ISP1582 (Fig 17. стр.54)
	// пауза между окончанием сигнала записи и началом сигнала чтения должна
	// быть не менее 91 nS. А при таких значениях регистров SMC она получалась
	// равной 30 nS. И USB периодически прекращал работу, особенно в отладчике.
	// Они пытались своевать с этим программной задержкой после каждой записи
	// Delay20() на 20 пустых циклов. Но для этого процессора
	// это не работает, т.к. иногда собиралась очередь в конвеййере и данные
	// передавались в USB одной пачкой.
	
  // =========================================
  // CS0 - 0x10000000 - USB
  //  
	// 1 такт - 10 nS	(приблизительно)

	AT91C_BASE_SMC->SMC_SETUP0 = 0x00020002;
	AT91C_BASE_SMC->SMC_PULSE0 = 0x07040704;
	AT91C_BASE_SMC->SMC_CYCLE0 = 0x00070010;		// было 0x00170007
	AT91C_BASE_SMC->SMC_CTRL0  = (0x1 << 16) | AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;

  // =========================================
  // CS2 - 0x30000000 - 
  //  
  AT91C_BASE_SMC->SMC_SETUP2 = 0x00030003;		// RD и WR по 3 clk
  AT91C_BASE_SMC->SMC_PULSE2 = 0x100C0D08;
  AT91C_BASE_SMC->SMC_CYCLE2 = 0x0010000D;
  AT91C_BASE_SMC->SMC_CTRL2  = (0x1 << 16) | AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
}


// В БП есть четыре регулятора напряжения: V1-V4.
// Выходы U11 DAC подключены к соответствующим входам этих регуляторов.
// V1,V2 и V3 умножают входное напряжение на 5.
// V4 - повторитель для питания выходных драйверов U12 XILINX
// 
// В данном методе осуществляются следующие преобразования:
// V1 = (V1<15.9)?(5*v):(ERROR)
// V2 = (V2<15.9)?((5*v) + 0.7):(ERROR)
// V3 = (V3<15.9)?((5*v) + 0.7):(ERROR)
// V4 = (v<3.86)?(v):(3.86)
//
void POWER_SetVCCs( DWORD a1, WORD verifyingVccCurrent )
{
	if( a1 > 3 )
		return;

	if ( a1 <= 2 )
	{
		if( verifyingVccCurrent > 163 )
		{
			LEDS_BEEP_OnOff( _leds_beep_current | 0x10 );
		}

		if( a1 )
			verifyingVccCurrent = (verifyingVccCurrent + 7) & 0xFF;
	}
	else if( verifyingVccCurrent > 198 )
	{
		verifyingVccCurrent = 198;
	}

	PIO_Clear( &_PA16_LCD_E );

	PIO_Clear( &_PA17_LCD_RW );
	PIO_Clear( &_PB25_DAC_LCD_D7_Z );

	LEDS_PASS_RUN_ERROR_BEEP[2] = verifyingVccCurrent;
	word_30040008[a1] = verifyingVccCurrent;

	PIO_Set( &_PA17_LCD_RW );
	PIO_Set( &_PB25_DAC_LCD_D7_Z );
}

void DisableTimer( AT91PS_TC pTC, AT91_REG tc_cmr )
{
  AT91_REG v2;

  pTC->TC_CCR = AT91C_TC_CLKDIS;
  pTC->TC_IDR = 0xFFFFFFFF;
  v2 = pTC->TC_SR;
  pTC->TC_CMR = tc_cmr;
}

void WaitByTimerMks( int mks )
{
  AT91C_BASE_PMC->PMC_PCER = 0x20000;
  DisableTimer( AT91C_BASE_TC0, 0x4002 );
	AT91C_BASE_TC0->TC_RC = (31*mks)/10;
  AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN;	// 5
	
	while( !( AT91C_BASE_TC0->TC_SR & AT91C_TC_CPCS ) );	// CPCS - 1: RC Compare has occurred since the last read of the Status Register.
}

void WaitByTimerMs( int ms )
{
	for( int i=0; i<ms; i++ )
		WaitByTimerMks( 1000 );
}

void Beep( int mks )
{
  int i;

  for( i=0; i<200; i++ )
  {
    _U13_LEDS_PASS_RUN_ERROR_BEEP = _leds_beep_current | eBEEP;
    WaitByTimerMks( mks );
    _U13_LEDS_PASS_RUN_ERROR_BEEP = _leds_beep_current;
    WaitByTimerMks( 2*mks );
  }
}
