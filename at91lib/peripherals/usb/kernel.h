#pragma once


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
//sfr AUXR = 0x8e;
//sfr CKCON = 0x8f;

#define MAX_BUF 64
//#define                  CHAP9_COMPLIANT   15                //use for chapter 9 and 8 test
//max packet size is 512
//#define USB_ENABLED 3     //to test FIFO must disable DMA_ENABLED and PIO_ENABLED
//USAGE FOR ATA AND ATAPI DEVICE
//#define PIO_ENABLED 1   // to test PIO mode must disable DMA_ENABLED
//#define MDMA_TRANSFER 5 //ATA MDMA Transfer Mode
//#define UDMA_TRANSFER 6 //ATA UDMA Transfer Mode
//#define PIO_TRANSFER 7 //ATA PIO Transfer Mode
//#define DEVICE_ENABLED  8
#define ATAPI_DEVICE 9
//USAGE FOR THE GENERIC DMA MODE
//#define DMA_ENABLED 2 //Select DMA Mode
//#define GDMA_MODE 10 //GDMA Slave Mode 
//#define MDMA_MODE 11 //MDMA Master Mode
//#define MPIO_MODE 17 //Manual PIO Mode
//#define PING_PONG 15 //set the descriptor to PID 1582
#define SBUS_RESET                  0         
#define SSUSPEND                    1
#define SPOWER                      2
#define SWAKEUP                     3
#define SCMD                        4
#define SREAD                       5
#define SWRITE                      6
#define SSTATUS                     7
#define Crystal_Freq 24
#define DEVICE_RESET 0x0C
#define ENABLE_INTRQ 0x08
#define DISABLE_INTRQ 0x0A
#define SETUP_DMA_REQUEST     0x7104
#define GET_FIRMWARE_VERSION    0x7204
#define GET_SET_TWAIN_REQUEST   0x7304
#define MASTER_DRIVE 0xA0
#define ATA_MASTER 0
#define ID_ATAPI_DEVICE 0xA1
#define ID_ATA_DEVICE 0xEC
#define ATAPI_SOFTRESET 0x08
#define SET_FEATURE 0xEF
#define INIT_DRIVE 0x91
#define Transfer_Mode 0x03
#define Read 0
#define Write 1
#define GDMA_Read 1
#define GDMA_Write 0
#define GDMA_Start 1
#define GDMA_Stop 0
#define OUT 0
#define IN 1
#define OFF 0
#define ON 1
#define Endpt_Zero 0
#define Endpt_One 1
#define Endpt_Two 2
#define Endpt_Three 3
#define Endpt_Four 4
#define Endpt_Five 5
#define Endpt_Six 6
#define Endpt_Seven 7
#define Control_Type 0
#define Iso_Type 1
#define Bulk_Type 2
#define Int_Type 3
/*
#define Control_Type 0
#define Iso_Type 3
#define Bulk_Type 1
#define Int_Type 2
*/
/*
#define Get_Status 0x00
#define Clear_Feature 0x01
#define Set_Feature 0x03
#define Set_Address 0x05
#define Get_Descriptor 0x06
#define Set_Descriptor 0x07
#define Get_Config 0x08
#define Set_Config 0x09
#define Get_Interface 0x0A
#define Set_Interface 0x0B
#define Synch_Frame 0x0C
*/
#define USB_Default 0
#define USB_Addressed 1
#define USB_Configured 2
#define ATAPI_Cmd_Packet_Phase          0x09
#define ATAPI_Message_Phase 0x0B
#define ATAPI_Read_Phase    0x0A
#define ATAPI_Write_Phase   0x08
#define ATAPI_Read_Phase_NODRQ    0x02
#define ATAPI_Write_Phase_NODRQ   0x00
#define ATAPI_Status_Phase 0x03
#define Error_Phase   0xAA
#define No_Data_Phase   0x55
#define Data_Host2Device 0x00
#define Data_Device2Host 0x80
#define TransferData2Host 0x01
#define TransferData2Device 0x02
#define TransferNoData 0x00
#define ATAPI_PACKET_COMMAND 0xA0
#define GDMA_Read_Command 0x00
#define GDMA_Write_Command 0x01
#define UDMA_Read_Command 0x02
#define UDMA_Write_Command 0x03
#define PIO_Read_Command 0x04
#define PIO_Write_Command 0x05
#define MDMA_Read_Command 0x06
#define MDMA_Write_Command 0x07
#define PIO_Write_Complete 0x08
#define PIO_Read_Complete  0x09
#define READ_1F0 0x0A
#define POLL_BUSY 0x0B
#define UPDATE_TASKFILE 0x0C
#define  DMA_VALIDATE_BUFFER        0x0E
#define  DMA_CLEAR_BUFFER           0x0F
#define  DMA_BUFFER_RESTART         0x10
#define  DMA_RESET                  0x11
#define  DMA_FLUSH                  0x12
//for FPGA and old board
/*
#define DMA_Start  P16
#define DMA_Wr_Rd P17
#define DMA_Reset P14
#define ISP1581_CS P15
#define  ISP1581_RESET              P27
#define  RESET_IDE                  P26
#define HS_FS_LED P35
*/
// for new Evalkit
/*
#define DMA_Start  P10
#define DMA_Reset P11
#define DMA_Wr_Rd P12
#define  VBus_Sense     P21
#define ISP1581_CS P15
#define  ISP1581_RESET  P16
#define  RESET_IDE      P17
#define HS_FS_LED P20
*/
//sbit DMA_Start = P1 ^ 4;
//sbit DMA_Reset = P1 ^ 4;
//sbit DMA_Wr_Rd = P1 ^ 4;
//sbit VBus_Sense = P1 ^ 2;
//sbit RESET_IDE = P1 ^ 4;
//sbit HS_FS_LED = P1 ^ 0;
//sbit ISP1581_CS = P1 ^ 2;
//sbit ISP1581_RESET = P3 ^ 4;

#define EA_DISABLE ( AT91C_BASE_AIC->AIC_IDCR = 1 )
#define EA_ENABLE ( AT91C_BASE_AIC->AIC_IECR = 1 )

// for old Evalkit
/*
#define DMA_Start  P34
#define DMA_Wr_Rd P10
#define DMA_Reset P35
#define ISP1581_CS P26
#define  ISP1581_RESET              P25
#define  RESET_IDE                  P24
#define HS_FS_LED P23
*/
#define GDMA_Test 1
#define UDMA_Test 2
#define MDMA_Test 3
#define PIO_Test    4
#define HIGH_SPEED 0
#define FULL_SPEED 1
#define HIGH_SPEED_LED          0
#define FULL_SPEED_LED 1
#define  TEST_MODE            0x0200
#define  ENDPOINT_HALT        0
#define  DEVICE_REMOTE_WAKEUP 1
#define  TEST_J            1
#define  TEST_K            2
#define  TEST_SE0_NAK      3
#define  TEST_PACKET       4
#define  TEST_FORCE_ENABLE 5
//***********************************************************************
//* *
//*                  Structure Variable Definition   *
//* *
//***********************************************************************
typedef union
{
	struct
	{
		BYTE TIMER_ONOFF : 1,											// 8051 timer running state
		STATE : 3,																// kernel current state
		MASTER_ATAPI_DRIVE : 1,
		MASTER_ATA_DRIVE : 1,
		MASTER_PRESENT : 1,
		SLAVE_PRESENT : 1;

		BYTE Manual_PIO : 1,
		Auto_PIO : 1,
		UDMA_Mode : 1,
		MDMA_Mode : 1,
		GDMA_Mode : 1,
		Timer_Expired : 1,
		Bus_Reset : 1,
		Error_Occur1 : 1;

//		BYTE Error_Occur : 2;
		BYTE Error_Occur2 : 1,
		Init_Done : 1,
		In_Reset_Done : 1,
		Out_Reset_Done : 1,
		Tx_Done : 1,
		Dummy_Byte : 1,
		Endpt_Stall : 1,
		HS_FS_State : 1;

		BYTE MPIO_Out_Reset_Done : 1,
		MPIO_In_Reset_Done : 1,
		Register_Test : 1,
		Transfer_Error : 1,
		Transfer_Start : 1,
		ATAPI_DMA_END : 1;

	} BITS;

	DWORD VALUE;

} KERNEL;

typedef struct DRIVE_CONFIG
{
	WORD General_Config; //word 0
	WORD PIO_Mode; //word 51
	WORD MDMA_Mode; //word 63
	WORD Advanced_PIO; //word 64
	WORD UDMA_Mode; //word 88

} DRIVE_CONFIG;

typedef struct TIMER
{
	WORD mSEC_Scale;
	WORD Hundred_mSEC_Scale;

}TIMER;

typedef struct FEATURE
{
	BYTE SubCommand_Count;
	BYTE SubCommand_Number;
	BYTE SubCommand_Low;
	BYTE SubCommand_High;

}FEATURE;

typedef union DMA_INT_FLAG
{
	struct DMA_FLAG_BITS
	{
		BYTE CMD_AUTO_COMPLETE : 1;
		BYTE CMD_INTRQ_OK : 1;
		BYTE TASKFILE_READ_COMPLETE : 1;
		BYTE BSY_DRQ_POLL_DONE : 1;
		BYTE START_READ_1F0_RD_FIFO : 1;
		BYTE RD_1F0_FIFO_EMPTY : 1;
		BYTE WR_1F0_FIFO_FULL : 1;
		BYTE WR_1F0_FIFO_EMPTY : 1;
		BYTE DMA_DONE : 1;
		BYTE INTRQ_SEEN : 1;
		BYTE       INT_EOT : 1;
		BYTE       EXT_EOT : 1;
		BYTE RES1 : 4;
	}BITS;

	WORD VALUE;
}DMA_INT_FLAG;

typedef union DATA_SWAP
{
	BYTE byte[2];
	WORD word;
}DATA_SWAP;

typedef union ATAPI_BYTECOUNT
{
	BYTE Byte[4];
	ULI VALUE;
}ATAPI_BYTECOUNT;

typedef union BYTECOUNT
{
	BYTE Byte[2];
	ULI VALUE;
}BYTECOUNT;
/*
typedef union USB_ELEMENT
{
struct USB_ELEMENT_BITS
{
WORD SOF_Count : 11,
uSOF_Count : 3;
}BITS;
// WORD  Value;
}USB_ELEMENT;
*/
/*
typedef union
{
	struct
	{
		BYTE RESET : 1;
		BYTE SOF : 1;
		BYTE PSOF : 1;
		BYTE SUSP : 1;
		BYTE RESUME : 1;
		BYTE HS_STAT : 1;
		BYTE DMA : 1;
		BYTE RESERVED3 : 1;

		BYTE EP0SETUP : 1;
		BYTE RESERVED2 : 1;
		BYTE EP0RX : 1;
		BYTE EP0TX : 1;
		BYTE EP1RX : 1;
		BYTE EP1TX : 1;
		BYTE EP2RX : 1;
		BYTE EP2TX : 1;

		BYTE EP3RX : 1;
		BYTE EP3TX : 1;
		BYTE EP4RX : 1;
		BYTE EP4TX : 1;
		BYTE EP5RX : 1;
		BYTE EP5TX : 1;
		BYTE EP6RX : 1;
		BYTE EP6TX : 1;

		BYTE EP7RX : 1;
		BYTE EP7TX : 1;
		BYTE RESERVED1 : 6;

	} BITS;

	DWORD VALUE;

} USB_INT_FLAG;
*/
typedef struct
{
	BYTE bmRequestType;
	BYTE bRequest;
	WORD wValue;
	WORD wIndex;
	WORD wLength;

} USB_DEVICE_REQ;

typedef union
{
	struct
	{
		BYTE Remote_Wakeup : 1,
		Halt : 1,
		Self_Powered : 1,
		Endpt_Halt : 1,
		Test_Mode : 1,
		State : 3;

		BYTE Dummy0 : 8;

//		BYTE CBI_Detected : 1,
//		BYTE DMA_Test_Mode : 8;

		BYTE CBI_Detected : 1,
		Alter_Interface : 1,
		CBW_Rx : 1,
		CBW_Data : 1,
		DMA_State : 3,
		Big_Endian : 1;

	} BITS;
	
	DWORD VALUE;

} USB_DEVICE;

typedef struct USBCBW
{
	union Signature
	{
		BYTE dCBWSignature[4];
		ULI VALUE;

	}Signature;

	BYTE  dCBWTag[4];
	union Length
	{

		BYTE dCBWDataTransferLength[4];
		ULI VALUE;

	}Length;

	BYTE  dCBWFlags;
	BYTE  bCBWLUN;
	BYTE  bCDBLength;
	BYTE  CBWCDB[0x10];

} USBCBW;

typedef struct USB_CSW
{
	union Residue
	{
		BYTE dCSWDataResidue[4];
		ULI VALUE;

	} Residue;

	BYTE bCSWStatus;

} USBCSW;

typedef union FILESIZE
{
	struct filesize
	{
		BYTE       Address[3];
		BYTE       FileSize[4];
		BYTE       DIR;

	} Size;

	UDI      Value;

} FILESIZE;

