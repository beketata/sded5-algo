#pragma once

#include "common.h"
#include "PROGPROC.h"
#include "_dochapi.h"


#define DOCH_CHIP_ID_H3	0x4833
#define DOCH_CHIP_ID_H4	0x4834
#define DOCH_CHIP_ID_H5	0x4835

#define DOCH_SHORT_WAIT 0x00080000
#define DOCH_LONG_WAIT  0x000F0000


/*8KB Offsets*/ 
#define DOCH_CONT_REG_AREA_8KB_OFFSET		0x0800 
#define DOCH_DATA_PORT_AREA_8KB_OFFSET		0x1000 
#define DOCH_CONFIG_REG_AREA_8KB_OFFSET		0x1400 
 
/*128KB Offsets*/ 
#define DOCH_CONT_REG_AREA_128KB_OFFSET				0 
#define DOCH_DATA_PORT_AREA_128KB_OFFSET			0x0800 
#define DOCH_CONFIG_REG_AREA_128KB_OFFSET			0x1400 
#define DOCH_PRIMARY_SECONDARY_AREA_128KB_OFFSET	0x1800 

#define DOCH_CONFIG_REG_AREA_OFFSET DOCH_CONT_REG_AREA_8KB_OFFSET

#define DOCHREAD_ATA_REG( reg ) (BYTE)IC_Read( DOCH_CONFIG_REG_AREA_OFFSET+(((DWORD)(reg)) << 1) )
#define DOCHWRITE_ATA_REG( reg, data ) IC_Write( DOCH_CONFIG_REG_AREA_OFFSET+(((DWORD)(reg)) << 1), (WORD)data )
#define DOCHBLK_READ( buf, num ) tffs_readw( (Word*)(buf), ((DWORD)(num))<<8 )


/* maximum number of sectors which could be read from / written to DOCH at once */
#define DOCH_MAX_SECTORS  256

/* set of operation codes for DOCH_VSCMD_EXT_DEVICE_CTRL command */
typedef enum : BYTE
{ 
	DOCH_IDENTIFY_DISKONCHIP_DEVICE	= 0x00,
	DOCH_GET_EXTENDED_DEVICE_INFO		= 0x01,
	DOCH_SET_DATA_XFER_MODE					= 0x10,
	DOCH_ATOMIC_WRITE_SEQUENCE			= 0x20,
	DOCH_OPTIMIZE_MEDIA							= 0x30,
	DOCH_GET_CUSTOM_PARAM						= 0x40,
	DOCH_SET_CUSTOM_PARAM						= 0x41,
	DOCH_CALIBRATE_CLOCK						= 0x50,
	DOCH_GET_POWER_MODE							= 0x60,
	DOCH_SET_POWER_MODE							= 0x61,
	DOCH_GET_DISK_USER_ATTR					= 0x70,
	DOCH_SET_DISK_USER_ATTR					= 0x71,
	DOCH_GET_CONFIGURATION_DATA			= 0x72,
	DOCH_SET_CONFIGURATION_DATA			= 0x73,
	DOCH_ACTIVATE_DEBUG_MODE				= 0x7C,
	DOCH_RETRIEVE_DBG_MSG						= 0x7E,
	DOCH_SET_ALERT_LEVEL						= 0x7F,
	DOCH_GET_RESET_STATUS						= 0x80,
	DOCH_NOTIFY_PLATFORM_RESUMED		= 0x8E,
	DOCH_NOTIFY_RESET								= 0x8F

} DOCH_DeviceCtrlOp;


// Set of DOCH Vendor-Specific ATA Commands
//
typedef enum : BYTE
{ 
	// I/O
	DOCH_VSCMD_READ_PARTITION							= 0x82,
	DOCH_VSCMD_WRITE_PARTITION						= 0x83,
	DOCH_VSCMD_WRITE_FLEXI								= 0x84,

	// Sectors Operations
	DOCH_VSCMD_OPTIMIZE_PARTITION_SECTORS	= 0x86,
	DOCH_VSCMD_ERASE_PARTITION_SECTORS		= 0x88,

	// Hash
	DOCH_VSCMD_READ_CALCULATED_HASH				= 0x8A,
	DOCH_VSCMD_WRITE_CALCULATED_HASH			= 0x8B,
	DOCH_VSCMD_READ_ORIGINAL_HASH					= 0x8C,
	DOCH_VSCMD_WRITE_GIVEN_HASH						= 0x8D,

	// Sub-commanded commands
	DOCH_VSCMD_PARTITION_MANAGEMENT				= 0xFA,
	DOCH_VSCMD_ACCESS_CONTROL							= 0xFB,
	DOCH_VSCMD_EXT_DEVICE_CTRL						= 0xFC,
	DOCH_VSCMD_EXT_SECURITY_CTRL					= 0xFD,

	// Supported standard ATA commands
	DOCH_CMD_IDENTIFY_DEV									= 0xEC,
	DOCH_DOWNLOAD_MICROCODE								= 0x92

} DOCH_Command;


/* bits in DOCH_STATUS_REG and DOCH_ALT_STATUS_REG registers */
#define DOCH_ERROR      0x01
#define DOCH_DRQ        0x08
#define DOCH_DSC        0x10
#define DOCH_READY      0x40
#define DOCH_BUSY       0x80

/* bits in DOCH_STATUS_REG and DOCH_ALT_STATUS_REG registers */
#define DOCH_ERR_REG_BIT0	0x01;
#define DOCH_ERR_REG_BIT1	0x02;
#define DOCH_ERR_REG_ABRT	0x04;
#define DOCH_ERR_REG_BIT3	0x08;
#define DOCH_ERR_REG_BIT4	0x10;
#define DOCH_ERR_REG_BIT5	0x20;
#define DOCH_ERR_REG_BIT6	0x40;
#define DOCH_ERR_REG_BIT7	0x80;

/* bits for DOCH_DRIVE_HEAD_REG register */
#define DOCH_DEVICE     0x10
#define DOCH_LBA        0x40
#define DOCH_DRIVE_HEAD 0xA0

#define DOCH_SELECT_DEV0 DOCH_LBA
#define DOCH_SELECT_DEV1 (DOCH_DEVICE|DOCH_LBA)
#define DOCH_SELECT_DEV(devNum) ((devNum*DOCH_DEVICE)|DOCH_LBA)

/* DOCH configuration bits (IDENTIFY_DEVICE Word#0)	*/
#define DOCH_ATAPI      0x8000
#define DOCH_REMOVABLE  0x0080
#define DOCH_FIXED      0x0040


// DOCH error codes
//
#define DOCH_ATA_ERROR_BASE	0x0
#define DOCH_SDK_ERROR_BASE	0x100
#define DOCH_FS_ERROR_BASE  0x200

/* DOCH configuration bits (IDENTIFY_DEVICE Word#0)	*/
#define DOCH_ATAPI      0x8000
#define DOCH_REMOVABLE  0x0080
#define DOCH_FIXED      0x0040

/*	IDENTIFY DEVICE data	*/
/*==========================*/
__packed
typedef struct
{
	Word	wGeneralConfiguration;
	Word	wDefaultNumberOfCylinders;	/*1*/
	Word	wSpecificConfiguration;		/*ATA-5*/
	Word	wDefaultNumberOfHeads;		/*3*/
	Dword	dwReserved2;
	Word	wDefaultSectorsPerTrack;	/*6*/
	Word	wReserved1[3];
	Byte	bSerialNumber[20];			/*10*/
	Word	wReserved4[2];
	Word	wVsFLBytesInReadWriteLONG;	/*obsolete in ata-5*/
	Byte	bFirmwareRevision[8];		/*23*/
	Byte	bModelNumber[40];			/*27*/
	
	Byte	bReserved3;					/*47	ata-4+ : must be 0x80*/
	Byte	bMaxSectorsPerMulti;
	
	Word	wReserved5;
	Word	wCapabilities;				/*49*/
	Word	wCapabilities2;				/*ata-4*/
	
	Byte	bPIOTransferTiming;			/*51	obsolete in ATA-5*/
	Byte	bReserved6a;
	
	Word	wReserved7;					/*was (single-word?) DMA Timing in ATA-2*/
	Word	wWordSupported;				/*53*/
	Word	wCurrentNumberOfCylinders;	/*54*/
	Word	wCurrentNumberOfHeads;		/*55*/
	Word	wCurrentSectorPerTrack;		/*56*/
	
	Word	wCurrentCapacityInSectorsLow;	/*57*/
	Word	wCurrentCapacityInSectorsHigh;	/*58*/
	
	Byte	bMultipleSectorSettingValid;	/*59*/
	Byte	bMultipleSectorCurrentNumber;
	
	Word	wTotalNumberOfSectorsLow;	/*60*/
	Word	wTotalNumberOfSectorsHigh;	/*61*/
	
	Word	wReserved8;	/*was singlewordDMASelect+Support in ATA-2*/
	
	Byte	bMultiwordDMASelected;		/*63*/
	Byte	bMultiwordDMASupported;
	
	Byte	bReserved9;					/*64*/
	Byte	bAdvancedPIOsupported;
	
	Word	wMinDMACycleTimePerWord;
	Word	wRecommndedMinDMACycleTimePerWord;
	Word	wMinPIOCycleTimeWithoutFlowControl;
	Word	wMinPIOCycleTimeWithIORDY;
	Word	wReserved10[6];
	
	Word	wQueueDepth;				/*75	ata-4*/
	Word	wReserved11[4];
	
	Word	wMajorVersionNumber;		/*80*/
	Word	wMinorVersionNumber;
	Word	wCommandSetsupported1;		/*82*/
	Word	wCommandSetsupported2;
	
	Word	wCommandSetFeatureExtension;	/*84	ata-4*/
	Word	wCommandSetFeatureEnabled1;		/*ata-4*/
	Word	wCommandSetFeatureEnabled2;		/*86	ata-4*/
	Word	wCommandSetFeatureDefault;		/*ata-4*/
	
	Byte	bUltraDMASelected;				/*88	ata-4*/
	Byte	bUltraDMASupported;
	
	Word	wTimeForSecurityEraseCompletion;			/*ata-4*/
	Word	wTimeForEnhancedSecurityEraseCompletion;	/*90	ata-4*/
	
	Word	wCurrentAdvancedPowerManagementValue;		/*ata-4*/
	
	Word	wMasterPasswordRevision;					/*92	ata-5*/
	Word	wHardwareResetResult;						/*ata-5*/
	
	Word	wReserved12[33];
	
	Word	wRemovableMediaNotificationSupport;	/*127	ata-4*/
	
	Word	wSecurityStatus;					/*128, i.e. 127 words left*/
	Word	wVendorSpecific[31];				/*129-159*/
	Word	wCFApowermode1;
	Word	wCFAreserved[15];
	Word	wCurrentMediaSerialNumber[30];
	
	Word	wReserved13[49];
	
	Word	wIntegrityWord;

} DOCH_DriveParameters;


/* set of operation codes for DOCH_VSCMD_PARTITION_MANAGEMENT command */
typedef enum
{ 
	DOCH_GET_PARTITION_INFO			= 0x00,
	DOCH_SET_DEFAULT_PARTITION		= 0x11,
	DOCH_SET_PARTITION_PROTECTION	= 0x18,
	DOCH_GET_PARTITION_USER_ATTR		= 0x70,
	DOCH_SET_PARTITION_USER_ATTR		= 0x71,
	DOCH_DELETE_PARTITIONS			= 0xB0,
	DOCH_ADD_PARTITION				= 0xB4,
	DOCH_SECURE_ERASE				= 0xB8

} DOCH_PartitionManageOp;


//	structure that holds addressing data (CHS/LBA)
//
typedef struct
{
	Byte  bSecNum;
	Byte  bCylLow;
	Byte  bCylHi;
	Byte  bDevHead;

} Addressing_Values_s;


#define DOCH_SET_ATA_ADDRESS_VALUES(addr_vals, sectorNumber)	\
	addr_vals.bSecNum  = (Byte)(sectorNumber);									\
	addr_vals.bCylLow  = (Byte)(sectorNumber >>  8);						\
	addr_vals.bCylHi   = (Byte)(sectorNumber >> 16);						\
	addr_vals.bDevHead = (Byte)(((sectorNumber >> 24) & 0x0f) | DOCH_LBA);


DOCH_Error ready( DOCH_Reg reg, Byte mask, Byte on_bits, Dword milliseconds );
