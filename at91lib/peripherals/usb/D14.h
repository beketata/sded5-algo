#pragma once


#include "common.h"
#include "standard.h"
#include "kernel.h"
#include "usb_init.h"
#include "USB.h"
#include "reqcode.h"


/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                 http://www.zlgmcu.com
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 刘英斌
** 日　期:2003-03-13
** 描　述:ISP1581 V1.0
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
typedef union
{
	struct
	{
		BYTE DEVADDR : 7,
		DEVEN : 1;

	} BITS;

	BYTE VALUE;

} ADDRESS_REG;

typedef union
{
	struct
	{
		WORD SOFTCT : 1,
		PWROFF : 1,
		WKUPC : 1,
		GLINTE : 1,
		SFRESET : 1,
		GOSUSP : 1,
		SNDRSU : 1,
		DISGLBL : 1,

		VBUSSTAT : 1,
		DMA_CLKON : 1,
		RESERVED : 6;

	} BITS;

	WORD VALUE;

} USB_MODE;

typedef union
{
	struct
	{
		BYTE INTPOL : 1,
		INTLVL : 1,
		DDBGMODOUT : 2,
		DDBGMODIN : 2,
		CDBGMOD : 2;

	} BITS;

	BYTE VALUE;

} INT_CONFIG;
/*
typedef union
{
	struct
	{
		BYTE IEBRST : 1;
		BYTE IESOF : 1;
		BYTE IEPSOF : 1;
		BYTE IESUSP : 1;
		BYTE IERESM : 1;
		BYTE IEHS_STA : 1;
		BYTE IEDMA : 1;
		BYTE IEVBUS : 1;

		BYTE IEP0SETUP : 1;
		BYTE RESERVED2 : 1;
		BYTE IEP0RX : 1;
		BYTE IEP0TX : 1;
		BYTE IEP1RX : 1;
		BYTE IEP1TX : 1;
		BYTE IEP2RX : 1;
		BYTE IEP2TX : 1;

		BYTE IEP3RX : 1;
		BYTE IEP3TX : 1;
		BYTE IEP4RX : 1;
		BYTE IEP4TX : 1;
		BYTE IEP5RX : 1;
		BYTE IEP5TX : 1;
		BYTE IEP6RX : 1;
		BYTE IEP6TX : 1;

		BYTE IEP7RX : 1;
		BYTE IEP7TX : 1;
		BYTE RESERVED1 : 6;

	} BITS;

	DWORD VALUE;

} USB_INT_ENABLE;
*/
typedef union
{
	struct
	{
		WORD BRESET : 1,
		SOF : 1,
		PSOF : 1,
		SUSP : 1,
		RESUME : 1,
		HS_STAT : 1,
		DMA : 1,
		VBUS : 1,

		EP0SETUP : 1,
		RESERVED2 : 1,
		EP0RX : 1,
		EP0TX : 1,
		EP1RX : 1,
		EP1TX : 1,
		EP2RX : 1,
		EP2TX : 1;

		WORD EP3RX : 1,
		EP3TX : 1,
		EP4RX : 1,
		EP4TX : 1,
		EP5RX : 1,
		EP5TX : 1,
		EP6RX : 1,
		EP6TX : 1,

		EP7RX : 1,
		EP7TX : 1,
		RESERVED1 : 6;

	} BITS;

	DWORD VALUE;

} USB_INTERRUPT;


typedef union
{
	struct
	{
		WORD FFOSZ7_0 : 8,

		FFOSZ10_8 : 3,
		NTRANS : 2,
		RESERVED2 : 3;

	} BITS;

	WORD VALUE;

} ENDPT_MAXSIZE;

typedef union
{
	struct
	{
		BYTE DIR : 1,
		ENDPIDX : 4,
		EP0SETUP : 1,
		RESERVED : 2;

	} BITS;

	BYTE VALUE;

} ENDPT_INDEX;

typedef union
{
	struct
	{
		BYTE STALL : 1,
		STATUS : 1,
		RESERVED2 : 1,
		VENDP : 1,
		CLBUF : 1,
		RESERVED1 : 3;

	} BITS;

	BYTE VALUE;

} CONTROL;

typedef union
{
	struct
	{
		WORD ENDPTYP : 2,
		DBLBUF : 1,
		ENABLE : 1,
		ZERO_PKT_DIS : 1,
		RESERVED1 : 3,

		RESRVED2 : 8;

	} BITS;

	WORD VALUE;

} ENDPT_TYPE;

typedef union
{
	struct
	{
		BYTE RTOK : 1;
		BYTE ERROR : 4;
		BYTE RESERVED : 1;
		BYTE DATA01 : 1;
		BYTE UNREAD : 1;

	} BITS;

	BYTE VALUE;

} ERROR_CODE;

typedef union
{
	struct
	{
		WORD  RESERVED : 8,

		OUT0SH : 1,
		OUT1SH : 1,
		OUT2SH : 1,
		OUT3SH : 1,
		OUT4SH : 1,
		OUT5SH : 1,
		OUT6SH : 1,
		OUT7SH : 1;

	} BITS;

	WORD VALUE;

} VALIDSHORT;

typedef union
{
	struct
	{
		WORD SOFL : 8,

		SOFH : 3,
		USOF : 3,
		RESERVED : 2;

	} BITS;

	WORD VALUE;

} FRAME_NO;

typedef union
{
	struct
	{
		BYTE SE0_NAK : 1,
		JSTATE : 1,
		KSTATE : 1,
		PRBS : 1,
		FORCEFS : 1,
		LPBK : 1,
		PHYTEST : 1,
		FORCEHS : 1;

	} BITS;

	BYTE VALUE;

} TESTMODE;

typedef union
{
	struct
	{
		WORD WIDTH : 1,
		RES1 : 1,
		DMA_MODE : 2,
		BURST : 3,
		CNTENA : 1,

		RES2 : 8;

	} BITS;

	WORD VALUE;

} GDMA_CONFIG;

typedef union
{
	struct
	{
		BYTE PIO_MODE : 3,
		UDMA_MODE : 2,
		ATA_MODE : 1,
		IGNORE_IORDY : 1,
		RES2 : 1;

	} BITS;

	BYTE VALUE;

} UDMA_CONFIG;

typedef union
{
	struct
	{
		BYTE READ_POL : 1,
		WRITE_POL : 1,
		DREQ_POL : 1,
		ACK_POL : 1,
		MASTER : 1,
		EOT_POL : 1,
		ENDIAN : 2;

	} BITS;

	BYTE VALUE;

} DMA_HARDWARE;

typedef union
{
	struct
	{
		BYTE DMA_STROBE : 5,
		RES : 3;

	} BITS;

	BYTE VALUE;

} DMA_STROBE;

typedef union
{
	struct DMA_INT_BITS
	{
		WORD CMD_AUTO_COMPLETE : 1,
		INTRQ_PENDING : 1,
		TASKFILE_READ_COMPLETE : 1,
		BSY_DRQ_POLL_DONE : 1,
		START_READ_1F0_RD_FIFO : 1,
		RD_1F0_FIFO_EMPTY : 1,
		WR_1F0_FIFO_FULL : 1,
		WR_1F0_FIFO_EMPTY : 1,

		DMA_DONE : 1,
		PENDING_INTRQ : 1,
		INT_EOT : 1,
		EXT_EOT : 1,
		RES1 : 4;

	} BITS;

	WORD VALUE;

} DMA_INT;

typedef union
{
	struct
	{
		WORD CMD_AUTO_COMPLETE : 1,
		INTRQ_PENDING : 1,
		TASKFILE_READ_COMPLETE : 1,
		BSY_DRQ_POLL_DONE : 1,
		START_READ_1F0_RD_FIFO : 1,
		RD_1F0_FIFO_EMPTY : 1,
		WR_1F0_FIFO_FULL : 1,
		WR_1F0_FIFO_EMPTY : 1,

		DMA_DONE : 1,
		RES1 : 7;

	} BITS;

	WORD VALUE;

} DMA_INT_ENABLE;

/*
typedef union DMA_ENDPT
{
struct DMA_ENDPT_BITS
{
BYTE DMADIR : 1;
BYTE EDPIDX : 3;
BYTE NOT_USED :  4;
}BITS;
BYTE VALUE;
}DMA_ENDPT;
*/
typedef union
{
	struct
	{
		BYTE PIO_SEQ_STATE : 3,
		PIO_CMD_STATE : 5;

	} BITS;

	BYTE VALUE;

} DMA_STATE_1;

typedef union
{
	struct
	{
		BYTE RES : 8;

	} BITS;

	BYTE VALUE;

} DMA_STATE_2;

__packed
typedef struct alignas(4)
{
	ADDRESS_REG D14_ADDRESS alignas(4);										// 00
	BYTE DUMMY_01;
	BYTE DUMMY_02;
	BYTE DUMMY_03;

	ENDPT_MAXSIZE D14_ENDPT_MAXPKTSIZE alignas(4);					// 04
	BYTE DUMMY_06;
	BYTE DUMMY_07;

	ENDPT_TYPE D14_ENDPT_TYPE alignas(4);				// 08
	BYTE DUMMY_0A;
	BYTE DUMMY_0B;

	USB_MODE D14_MODE alignas(4);								// 0C
	BYTE DUMMY_0E;
	BYTE DUMMY_0F;

	INT_CONFIG D14_INT_CONFIG alignas(4);									// 10
	BYTE DUMMY_11;
	BYTE DUMMY_12;
	BYTE DUMMY_13;

	USB_INTERRUPT D14_INT_ENABLE alignas(4);								// 14

	USB_INTERRUPT D14_INT alignas(4);											// 18

	WORD D14_BUFFER_LENGTH alignas(4);											// 1C
	BYTE DUMMY_1E;
	BYTE DUMMY_1F;

	WORD D14_DATA_PORT alignas(4);													// 20
	BYTE DUMMY_22;
	BYTE DUMMY_23;

	VALIDSHORT D14_VALIDATE_SHORT alignas(4);							// 24
	BYTE DUMMY_26;
	BYTE DUMMY_27;

	CONTROL D14_CONTROL_FUNCTION alignas(4);								// 28
	BYTE DUMMY_29;
	BYTE DUMMY_2A;
	BYTE DUMMY_2B;

	BYTE D14_ENDPT_INDEX alignas(4);												// 2C
	BYTE DUMMY_2D;
	BYTE DUMMY_2E;
	BYTE DUMMY_2F;

	BYTE D14_DMA_COMMAND alignas(4);												// 30
	BYTE DUMMY_31;
	BYTE DUMMY_32;
	BYTE DUMMY_33;

	BYTE D14_DMA_TRANSFER_COUNTER_LSB alignas(4);					// 34
	BYTE D14_DMA_TRANSFER_COUNTER_BYTE2;
	BYTE D14_DMA_TRANSFER_COUNTER_BYTE3;
	BYTE D14_DMA_TRANSFER_COUNTER_MSB;

	GDMA_CONFIG D14_GDMA_CONFIG alignas(4);								// 38
	BYTE DUMMY_3A;
	BYTE DUMMY_3B;

	DMA_HARDWARE D14_DMA_HARDWARE alignas(4);							// 3C
	BYTE DUMMY_3D;
	BYTE DUMMY_3E;
	BYTE DUMMY_3F;

	BYTE D14_DATA_TASKFILE_LSB alignas(4);									// 40
	BYTE DATA_TASKFILE_BYTE2;
	BYTE DATA_TASKFILE_BYTE3;
	BYTE DATA_TASKFILE_MSB;

	BYTE D14_CMD_STATUS_TASKFILE alignas(4);
	BYTE DUMMY_45;
	BYTE DUMMY_46;
	BYTE DUMMY_47;

	BYTE D14_ERROR_FEATURE_TASKFILE alignas(4);
	BYTE D14_INTERRUPT_TASKFILE;
	BYTE D14_SECTOR_NUMBER;
	BYTE D14_BYTECOUNT_LSB_TASKFILE;

	BYTE D14_BYTECOUNT_MSB_TASKFILE alignas(4);
	BYTE D14_DRIVE_SELECT_TASKFILE;
	BYTE D14_ALT_STATUS_DEVCNTRL_TASKFILE;
	BYTE D14_TASKFILE;

	DMA_INT D14_DMA_INT alignas(4);
	BYTE DUMMY_52;
	BYTE DUMMY_53;

	DMA_INT_ENABLE D14_DMA_INT_ENABLE alignas(4);
	BYTE DUMMY_56;
	BYTE DUMMY_57;

	BYTE D14_DMA_ENDPOINT alignas(4);
	BYTE DUMMY_59;
	BYTE DUMMY_5A;
	BYTE DUMMY_5B;

	DMA_STATE_1 D14_DMA_STATE_1 alignas(4);
	BYTE DUMMY_5D;
	BYTE DUMMY_5E;
	BYTE DUMMY_5F;

	DMA_STROBE DMA_STROBE_TIMING alignas(4);
	BYTE DUMMY_61;
	BYTE DUMMY_62;
	BYTE DUMMY_63;

	BYTE UDMA_BYTE_COUNT_LSB alignas(4);
	BYTE UDMA_BYTE_COUNT_MSB;
	BYTE DUMMY_66;
	BYTE COUNT_1F0;

	BYTE PMU_TRANSFER_COUNT_0 alignas(4);
	BYTE PMU_TRANSFER_COUNT_1;
	BYTE PMU_TRANSFER_COUNT_2;
	BYTE PMU_TRANSFER_COUNT_3;

	BYTE DUMMY_6C alignas(4);
	BYTE DUMMY_6D;
	BYTE DUMMY_6E;
	BYTE DUMMY_6F;

	BYTE D14_CHIP_ID_LSB alignas(4);												// 70
	BYTE D14_CHIP_ID_MBYTE;
	BYTE D14_CHIP_ID_MSB;
	BYTE DUMMY_73;

	FRAME_NO D14_FRAME_NUMBER alignas(4);
	BYTE DUMMY_76;
	BYTE DUMMY_77;

	WORD D14_SCRATCH_REGISTER alignas(4);
	BYTE DUMMY_7A;
	BYTE DUMMY_7B;

	WORD D14_UNLOCK_DEVICE alignas(4);											// 7C
	BYTE DUMMY_7E;
	BYTE DUMMY_7F;

	ERROR_CODE D14_ERROR_CODE alignas(4);
	BYTE DUMMY_81;
	BYTE DUMMY_82;
	BYTE DUMMY_83;

	TESTMODE D14_TEST_MODE alignas(4);											// 84
	BYTE DUMMY_85;
	BYTE DUMMY_86;
	BYTE DUMMY_87;

} D14_CNTRL_REG;


extern volatile D14_CNTRL_REG D14_Cntrl_Reg;
extern KERNEL _USB_Kernel_Flag;
extern USB_DEVICE _USB_Device;
extern USB_INTERRUPT _USB_Int_Flag;
extern USB_DEVICE_REQ _USB_Device_Request;

extern "C"
{
	__fiq __arm void USB_ISR();
	__irq __arm void USB_ISR_SPU();
	void USB_Debug_Mode();

	void USB_reserved();
	void USB_get_status();
	void USB_clear_feature();
	void USB_set_feature();
	void USB_set_address();
	void USB_get_descriptor();
	void USB_get_config();
	void USB_set_config();
	void USB_get_interface();
	void USB_set_interface();
	void USB_wait_ep0rx();
	void USB_wait_ep0tx();
	void USB_ep0in_ack();
	void USB_ep0out_ack();
};
