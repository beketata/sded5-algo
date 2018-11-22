#include "SOCKET.h"


__no_init PROG_STRUC _chipInSocket;
__no_init BYTE _pins_GND[144];
__no_init BYTE _pins_Power[144];
__no_init BYTE _pins_Keys[144];


// Перевод всех 144-х сокетных ножек в Z состояние, отключением всех ключей.
//
void SOCKET_Keys_Release()
{
	int i;

	LogLine( "SOCKET_Keys_Release()" );

  for( i=0; i<144; i++ )
  {
    _SOCKET_CLK2_DATA = 0;		// CLK2 - V1 + V3/1
    _SOCKET_CLK1_DATA = 0;		// CLK1 - GND (после следующего цикла эти данные заедут в GND)
  }

  for( i=0; i<72; i++ )
	{
    _SOCKET_CLK3_DATA = 0;		// CLK3 - V2
    _SOCKET_CLK1_DATA = 0;		// CLK1 - V3/2
	}

	// Защелкиваем задвинутые данные во все регистры.
	// D0-STR3, D1-STR2, D2-STR1
	//
  _SOCKET_Strobes = 7;
  _SOCKET_Strobes = 0;

  for( i=0; i<54; i++ )
    _pins_Keys[i] = 0;

  for( i=0; i<144; i++ )
  {
    _pins_GND[i] = 0;
    _pins_Power[i] = 0xFF;
  }
}


void SOCKET_Keys_Set_V3H_GNG()
{
	int i, j;
	int v2;

	// Задвигаем 144 бита для платы GND ключей (N1 XELT16)
	// 
  for( i=143; i>=0; i-- )
		_SOCKET_CLK1_DATA = _pins_GND[i];

	// Задвигаем 72 бита для платы V3 ключей (N2 XELT15)
	//
  for( i=53; i>44; i-- )
  {
		v2 = _pins_Keys[i];
    for( j=0; j<8; j++ )
    {
			_SOCKET_CLK1_DATA = (v2 & 0x80) != 0;
			v2 = 2 * v2 & 0xFF;
    }
  }

  _SOCKET_Strobes = 4;												// STR1
  _SOCKET_Strobes = 0;
}


void SOCKET_Keys_Set_200024AC( DWORD a1, int a2 )
{
  BYTE v2; // r9
  BYTE v3; // r8
  BYTE v4; // r10
  unsigned int i; // r4 MAPDST
  BYTE v6; // r6
  unsigned int j; // r5 MAPDST
  int v9; // r6
  int m; // r4
  BYTE v12; // r6

	v2 = _pins_Power[a1];
	if( v2 <= 2 )
  {
    v3 = ((a1 >> 3) + 18*v2) & 0xFF;
    v4 = a1 & 7;
    if( a2 )
      _pins_Keys[v3] &= ~(1 << v4);
    else
      _pins_Keys[v3] |= 1 << v4;

		if( v2 == 1 )
		{
			for ( i=26; i>17; i-- )
			{
				v6 = _pins_Keys[i];
				for( j = 0; j < 8; j = (j + 1) & 0xFF )
				{
          _SOCKET_CLK3_DATA = (v6 & 0x80) != 0;

          v6 = 2 * v6 & 0xFF;
        }
      }
    }
    else if( v2 != 2 || a1 <= 71 )
    {
      for( i=44; i>35; i-- )
      {
        v9 = _pins_Keys[i];
        for( j=0; j<8; j++ )
        {
					_SOCKET_CLK2_DATA = (v9 & 0x80) != 0;

          v9 = 2 * v9 & 0xFF;
        }
      }

      for( m = 9; m; m-- )
      {
        v12 = _pins_Keys[m - 1];
        for( j=0; j<8; j++ )
        {
          _SOCKET_CLK2_DATA = (v12 & 0x80) != 0;

          v12 = 2 * v12 & 0xFF;
        }
      }
    }
    else
    {
      SOCKET_Keys_Set_V3H_GNG();
    }

		_SOCKET_Strobes = 7;
    _SOCKET_Strobes = 0;
  }
}


void SOCKET_CkeckInsertIC()
{
	int i;
	WORD numOfPins;
	WORD volts;
	WORD numOfContactedPins;
	//WORD v4;
	//unsigned int v10;

	LogLine( "SOCKET_CkeckInsertIC()" );

	BYTE bufPinsConnected[144] = {0};

//	if( _chipInSocket.VCC_XilinxVCCO )
//		volts = (WORD)( 5.12*_chipInSocket.VCC_XilinxVCCO ) + 2;	//  5*_chipInSocket.VCC_XilinxVCCO + 2;
//	else
	volts = DAC_VOLT_4( 3.33 );								// 170
	POWER_SetVCCs( 3, volts );									// Устанавливаем напряжение для выходных драйверов XILINX

	if( _chipInSocket.NumOfPins <= 48 )
		numOfPins = 48;
	else
		numOfPins = _chipInSocket.NumOfPins;

	_isAdapterError = 0;

	SOCKET_Keys_Release();														// Перевод всех 144-х сокетных ножек в Z состояние, отключением всех ключей.

	LEDS_BEEP_OnOff( _leds_beep_current | 0x80 );
	
	// *****************************************
	// После загрузки любой прошивки для XILINX
	// на всех ее выводах автоматически устанавливаются нули.
	// *****************************************

	// Запрашиваем у хоста прошивку для XILINX "GENERAL~.BIN" с внутренней подтяжкой pull-up
	// XIL_SOCKET_PIN_STATE:
	// 0 - Настройка ножки на вывод и вывод в нее 0
	// 1 - Настройка ножки на вывод и вывод в нее 1
	// 2 - Настройка ножки на вввод с внутренней подтяжкой pull-up (приблизительно 47K в плюс питания)
	//
	_usb_Buf_EP1TX[0] = 18;
	_usb_Buf_EP1TX[1] = 0;
	USB_SendBuf_EP1TX();
	USB_LibFile_Receive();

	for( i=0; i<numOfPins; i++ )
	{
		XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12
		XIL_SOCKET_PIN_STATE = 0;									// Подаем на выбранную ножку 0
	}
	while( i < 144 )
	{
		XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12
		XIL_SOCKET_PIN_STATE = 2;									// Настраиваем ножку на ввод.
		i++;
  }

	for( i=0; i<numOfPins; i++ )
	{
		XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12
		XIL_SOCKET_PIN_STATE = 2;									// Настраиваем ножку на ввод.

		WaitByTimerMks( 10 );

		volts = XIL_SOCKET_PIN_STATE;
		XIL_SOCKET_PIN_STATE = 0;									// Подаем на выбранную ножку 0
	}

	// Запрашиваем у хоста прошивку для XILINX "GENERAL_.BIN" с внутренней подтяжкой pull-down
	// XIL_SOCKET_PIN_STATE:
	// 0 - Настройка ножки на вывод и вывод в нее 0
	// 1 - Настройка ножки на вывод и вывод в нее 1
	// 2 - Настройка ножки на вввод с внутренней подтяжкой pull-down (приблизительно 12K в землю)
	//
	//
	_usb_Buf_EP1TX[0] = 17;
	_usb_Buf_EP1TX[1] = 0;
	USB_SendBuf_EP1TX();
	USB_LibFile_Receive();

	// Подаем на все ножки 1-цы
	//
	for( i=0; i<144; i++ )
  {
		if( _chipInSocket.Pins[i] == 0 )
			continue;

		XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12
		
		if( i < numOfPins )
    	XIL_SOCKET_PIN_STATE = 1;								// Подаем на выбранную ножку 1 (напряжением, которое установили в начале этого метода для выходных драйверов XILINX).
		else
	    XIL_SOCKET_PIN_STATE = 2;								// Остальные ножки настраиваем на ввод.
  }

	WaitByTimerMs( 10 );

	// Поочередно каждую ножку настраиваем на ввод и меряем на ней напряжение.
	// Если ножка контачит с соответствующим контактом сокеты, на ножке должна читаться 1 (через защитные диоды, обычно через земляную ногу).
	//
  for( i=0; i<numOfPins; i++ )
  {
		if( _chipInSocket.Pins[i] == 0 )
			continue;
			 
    XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12

		XIL_SOCKET_PIN_STATE = 2;									// Настраиваем ножку на ввод.

		WaitByTimerMks( 10 );

    if( XIL_SOCKET_PIN_STATE & 1 )						// Измеряем на этой ножке напряжение.
      bufPinsConnected[i] = 1;								// Если напряжение есть - значит ножка контачит.

    XIL_SOCKET_PIN_STATE = 1;									// Подаем на выбранную ножку 1 (напряжением, которое установили в начале этого метода).
  }

  for( i=0; i<numOfPins; i++ )
  {
    XIL_SOCKET_PIN_NUM = i;										// Выбираем очередную ножку у XILINX U12

//		XIL_SOCKET_PIN_STATE = 0;									// Подаем на выбранную ножку 0.
    XIL_SOCKET_PIN_STATE = 2;									// Настраиваем ножку на ввод.
  }

	numOfContactedPins = 0;
  for( i=0; i<numOfPins; i++ )
  {
		// Если ножка в микросхеме задействована, проверяем, контачила ли она при проверке.
		//
    if( _chipInSocket.Pins[i] )								// 1 и 2 - ножка в микросхеме задействована (2 - GND)
    {
      if( !bufPinsConnected[i] )
      {
        _isAdapterError = 1;

				break;
      }

			numOfContactedPins++;
    }
  }

	// Проверка микросхемы на "раковое" вставление в сокету.
	//
	/*
  if( !_isAdapterError && ( _chipInSocket.field_3 & 2 ) )
  {
    if( _chipInSocket.VerifyingVccCurrent_dev10 <= DAC_VOLT_1_2_3( 2.25 ) )		// 23
    {
      POWER_SetVCCs( 0, _chipInSocket.VerifyingVccCurrent_dev10 );
      POWER_SetVCCs( 1, _chipInSocket.VerifyingVccCurrent_dev10 );
      POWER_SetVCCs( 2, _chipInSocket.VerifyingVccCurrent_dev10 );
    }
    else
    {
      POWER_SetVCCs( 0, DAC_VOLT_1_2_3( 2.25 ) );		// 23
      POWER_SetVCCs( 1, DAC_VOLT_1_2_3( 2.25 ) );		// 23
      POWER_SetVCCs( 2, DAC_VOLT_1_2_3( 2.25 ) );		// 23
    }

    for( i=0; i<numOfPins; i++ )
    {
      if( _chipInSocket.Pins[i] == 2 )				// Если эта ножка у микросхемы GND
      {
        XIL_SOCKET_PIN_NUM = i;								// Выбираем земляную ножку у XILINX U12
				XIL_SOCKET_PIN_STATE = 0;							// Подаем на земляную ножку 0.

        if( i >= 71 )
          _pins_Power[i] = 2;
        else
          _pins_Power[i] = 1;

        SOCKET_Keys_Set_200024AC( i, 0 );
      }
    }

    v4 = 0;
    for( i=0; i<numOfPins; i++ )
    {
      XIL_SOCKET_PIN_NUM = i;									// Выбираем очередную ножку у XILINX U12

      if( XIL_SOCKET_PIN_STATE & 1 )
        v4++;

      XIL_SOCKET_PIN_STATE = 2;								// Настраиваем ножку на ввод.
    }

    for( i=0; i<144; i++ )
    {
      _SOCKET_CLK3_DATA = 0;
      _SOCKET_CLK1_DATA = 0;
    }

    for( i=0; i<72; i++ )
      _SOCKET_CLK1_DATA = 0;

    _SOCKET_Strobes = 7;
    _SOCKET_Strobes = 0;

    for( i=0; i<54; i++ )
      _pins_Keys[i] = 0;

    if( (numOfContactedPins/3) > v4 )
      _isAdapterError = 1;
  }
	*/

	// Запрашиваем у хоста прошивку для XILINX для конкретной микросхемы.
	// В некоторых случаях для простых микросхем это "GENERAL~.BIN" с внутренней подтяжкой pull-up
	//

#ifdef _NOXIL_
	_usb_Buf_EP1TX[0] = 17;
#else
	_usb_Buf_EP1TX[0] = 19;
#endif

  USB_SendBuf_EP1TX();
  USB_LibFile_Receive1();


/*
	_IC_CTRL = 0xFF;
	while( 1 )
	{
		for( DWORD i=0; i<0x10000; i++ )
		{
			_IC_DATA = i;
		}
	}
*/	
	
	
	SOCKET_Keys_Release();														// Перевод всех 144-х сокетных ножек в Z состояние, отключением всех ключей.

	if( _isAdapterError )
  {
		// Если при проверке были неконтачащие ножки, поблочно отправляем информацию обо всех ножках.
		//
    _usb_Buf_EP1TX[0] = 21;
    USB_SendBuf_EP1TX();
		
    for( i=0; i<64; i++ )
      ((BYTE*)_usb_Buf_EP1TX)[i] = bufPinsConnected[i];
    USB_SendBuf_EP1TX();

    for( i=0; i<64; i++ )
			((BYTE*)_usb_Buf_EP1TX)[i] = bufPinsConnected[i+64];
    USB_SendBuf_EP1TX();

    for( i=0; i<16; i++ )
      ((BYTE*)_usb_Buf_EP1TX)[i] = bufPinsConnected[i+128];
    for( ; i<64; i++ )
      ((BYTE*)_usb_Buf_EP1TX)[i] = 0;
  }
  else
  {
		// Если при проверке неконтачащих ножек не обнаружено.
		//
    _usb_Buf_EP1TX[0] = 20;
  }
}


void SOCKET_SetupPowerPins()
{
	LogLine( "SOCKET_SetupPowerPins()" );

	switch( _chipInSocket.Package )
	{
	}

	_pins_GND[35] = 1;
	_pins_GND[45] = 1;
	_pins_GND[119] = 1;
	_pins_GND[121] = 1;

	_pins_Power[65] = 2;												// VCC
	_pins_Power[59] = 2;												// VCC2
	_pins_Power[48] = 2;												// VCCQ 
	_pins_Power[79] = 2;												// VCCQ 
}


void SOCKET_PowerOff()
{
	LogLine( "SOCKET_PowerOff()" );
	
	#ifdef _MYXIL_
	{
		_IC_ENABLE = 0xFF;												// Запрещаем работу XILINX переводом всех ножек, идущих на сокету в Z
	}
	#endif
	
  POWER_SetVCCs( 0, 0 );
  POWER_SetVCCs( 1, 0 );
  POWER_SetVCCs( 2, 0 );

	SOCKET_Keys_Release();
}


void SOCKET_PowerOn()
{
	int i;

	LogLine( "SOCKET_PowerOn()" );

	SOCKET_SetupPowerPins();
	
//	POWER_SetVCCs( 0, _chipInSocket.VCC_field_A);  // Напряжение питания
//  POWER_SetVCCs( 1, _chipInSocket.VerifyingVccCurrent_dev10);
  POWER_SetVCCs( 1, DAC_VOLT_1_2_3( 3.30 ) );
  POWER_SetVCCs( 2, DAC_VOLT_1_2_3( 3.30 ) );
	
	// Задвигаем 144 бита для платы GND ключей (N1 XELT16)
	// 
  for( i=0; i<144; i++ )
	{
		_SOCKET_CLK1_DATA = _pins_GND[143-i];
	}

	// Двумя циклами по 72 задвигаем 144 бита для ключей питания V3.
	// После строба на соответствующие ножки подастся питание V3.

	for( i=0; i<72; i++ )
	{
		// Задвигаем 72 бита для платы V3H ключей (N2 XELT15)
		//
		_SOCKET_CLK1_DATA = ( _pins_Power[143-i] == 2 )?( 1 ):( 0 );

		// Задвигаем 72 бита для платы V3L ключей (N3 XELT14)
		//
		_SOCKET_CLK2_DATA = ( _pins_Power[71-i] == 2 )?( 1 ):( 0 );
	}

	// Задвигаем 72 бита для платы V1 ключей (N4 XELT12)
	//
  for( i=0; i<72; i++ )
	{
		_SOCKET_CLK2_DATA = 0;
		_SOCKET_CLK3_DATA = 0;
	}

  _SOCKET_Strobes = 7;
  _SOCKET_Strobes = 0;

	WaitByTimerMs( 100 );
}
