#pragma once

#include "common.h"

/***********************/
/*	SDK Version Number */
/***********************/
#define DochSDKVersion     "0151"
#define DochSDKSubVersion  "Pre2"

/********************/
/*	Partitions		*/
/********************/

/*	Pattern to indicate a boolean global variable was set
	(to deal with "naughty" compilers...)*/
#define DOCH_GLOBAL_BOOL_PATTERN 0xCA

/*	Max number of paritions	*/
#define DOCH_MAX_PARTITIONS 16

/*IPL partition*/
#define DOCH_IPL_PARTITION				0
#define IPL_PARTITION_SIZE_SECTORS		0x200	/*256KB in sectors*/
#define IPL_PARTITION_SIZE_V_SECTORS	0x10   /* 8K KB sectors*/


/********************/
/*	Various Defines	*/
/********************/

/* On/Off definitions */
#define DOCH_OFF	0
#define DOCH_ON		1

/* size of DOCH data block in FLBytes */
#define	DOCH_SECTOR_SIZE_BITS	9
#define DOCH_SECTOR_SIZE		(1<<DOCH_SECTOR_SIZE_BITS)

/* Operations on IOreq structure */
#define DOCH_GET_SOCKET_FROM_IOREQ_HANDLE(ioreq)			(FLByte)((ioreq)->irHandle & 0x0f)
#define DOCH_SET_SOCKET_TO_IOREQ_HANDLE(ioreq, socket)		((ioreq)->irHandle |= (socket))

#define DOCH_GET_PARTITION_FROM_IOREQ_HANDLE(ioreq)			(FLByte)(((ioreq)->irHandle & 0xf0) >> 4)
#define DOCH_SET_PARTITION_TO_IOREQ_HANDLE(ioreq, part)		((ioreq)->irHandle = \
															( ((part & 0xF) << 4) | ((ioreq)->irHandle & 0xFFFFFF0F)) )

/*	Operation type (for long operations)	*/
#define DOCH_NORMAL_OPERATION		0x00
#define DOCH_IMMEDIATE_OPERATION	0x80

/*	Data transfer modes	*/
typedef enum { DOCH_DATA_XFER_MODE_SINGLE		= 0x00,
			   DOCH_DATA_XFER_MODE_MULT			= 0x01
} DOCH_DataTransferMode;
/*Max supported DRQ*/
#define DOCH_MAX_DRQ_SUPPORTED	4

/*ETFFS Alert Levels*/
typedef enum {
	DOCH_ALERT_LEVEL_CRASH		= 0,	/*Prints only errors that H3 can not recover from*/
	DOCH_ALERT_LEVEL_EMERGENCY	= 1,	/*Prints only important, but not lethal errors*/
	DOCH_ALERT_LEVEL_REPORT		= 2,	/*Prints basic report on H3 operations*/
	DOCH_ALERT_LEVEL_DEBUG		= 3,	/*Prints full report on H3 progress*/
	DOCH_ALERT_LEVEL_BORING		= 4		/*Prints every operation done by H3*/
} DOCH_AlertLevel;

/*	Power mode constants */
typedef enum {
	DOCH_PM_SET_NONE			= 0x00,
	DOCH_PM_SET_WORK_MODE		= 0x01,
	DOCH_PM_SET_INACTIVE_MODE	= 0x02,
	DOCH_PM_SET_BOTH_MODES		= 0x03
} DOCH_SetPowerMode;

typedef enum {
	DOCH_PM_WORK_MODE		= 0x00,
	DOCH_PM_INACTIVE_MODE	= 0x04
} DOCH_SelectedPowerMode;

typedef enum {
	DOCH_WM_NORMAL					= 0x00,	/*Same as ATA ACTIVE Mode*/
	DOCH_WM_LOW_FREQ				= 0x01,	/*Same as ATA IDLE Mode*/
	DOCH_WM_NORMAL_AND_AUTO_STBY	= 0x02,
	DOCH_WM_LOW_FREQ_AND_AUTO_STBY	= 0x03
} DOCH_WorkMode;

typedef enum {
	DOCH_IM_IDLE			= 0x00,			/*Same as ATA STANDBY Mode*/
	DOCH_IM_DPD				= 0x10,			/*Same as ATA SLEEP Mode*/
	DOCH_IM_IDLE_2_DPD      = 0x20			/*No timeout IDLE ==> DPD */
} DOCH_InactiveMode;

#define DOCH_PM_SAVE_DEFAULT	0x10		/*Save power mode settings as device default*/

/*	Custom Parameters Table	*/
typedef enum { 
	DOCH_CP_BOOT_MODE		= 0x00,	/*Boot Mode (See DOCH_IPLModes)*/
	DOCH_CP_XIP_TEST_MODE	= 0x01,	/*XIP Test Modes (See DOCH_XIPTestModes)*/
	DOCH_CP_XIP_MAX_SIZE	= 0x02,	/*Max XIP Size*/
	DOCH_CP_XIP_ETFFS_SIZE	= 0x03,	/*Returns the size of the currently saved ETFFS (required for uploading the ETFFS)*/
	DOCH_CP_WORK_MODES		= 0x04,	/*Work Modes (Bit Field, see enum DOCH_CPWorkModes)*/
	DOCH_CP_SLOCK_CTRL		= 0x05,	/*Sticky Lock control (See DOCH_SlockCtrl)*/
	DOCH_CP_BYTE_SWAP_CTRL	= 0x06,	/*Byte Swap control (See DOCH_ByteSwapCtrl)*/
	DOCH_CP_FLASH_TEST_MODE	= 0x07,	/*Enable/Disable flash test mode (See DOCH_FlashTestModeCtrl)*/
	DOCH_CP_REPORT_ECC_MODE  =0x08  /*Enable/Disable report of ECC errors (See DOCH_EccErrorCtrl)*/
} DOCH_CustomParams;

#define DOCH_CUSTOM_PARAM_TEMP		1
#define DOCH_CUSTOM_PARAM_DEFAULT	0

/* IPL Modes */
typedef enum {
	DOCH_IPL_MODE_NORMAL_RAM				= 0x0000,
	DOCH_IPL_MODE_PAGED_RAM					= 0x0001,
	DOCH_IPL_MODE_VIRTUAL_RAM				= 0x0002,
	DOCH_IPL_MODE_CS_DELAY					= 0x1000,	/*Should be Or`ed with 0x0XX0, where bits 4-11 indicating delay*/
	DOCH_IPL_MODE_ADDRESS_SHIFT_IN_AFFECT	= 0x2000,
	DOCH_IPL_MODE_ACTIVE_SWAP_BYTES			= 0x4000,
	DOCH_IPL_MODE_8KB_WINDOW				= 0x8000
} DOCH_IPLModes;

#define	DOCH_IPL_WRITE_FIRST_CHUNK			0x80000000 /*Defines first chunk when writing IPL in chunks*/

/* XIP Test Modes */ /*No default value, will be cleared upon reset*/
typedef enum {
	DOCH_XIP_NORMAL			= 0x0000,
	DOCH_XIP_WRITEABLE		= 0x0001,
	DOCH_XIP_FILL_PATTERN	= 0x0002,	/*"M-Systems" and the rest with consecutive 16bit numbers*/
	DOCH_XIP_PIPELINE		= 0x0003	/*Places the device in pipeline access mode*/
} DOCH_XIPTestModes;

typedef enum {
	DOCH_WORK_MODES_BLOCK_SPI = 0x0002	/*Block SPI*/
} DOCH_CPWorkModes;

typedef enum {
	DOCH_STICKY_LOCK_UNSET = 0x00,
	DOCH_STICKY_LOCK_SET   = 0x01
} DOCH_SlockCtrl;

typedef enum {
	DOCH_BYTE_SWAP_UNSET = 0x00,
	DOCH_BYTE_SWAP_SET   = 0x01
} DOCH_ByteSwapCtrl;

typedef enum {
	DOCH_DISABLE_FLASH_TEST_MODE	= 0x00,
	DOCH_ENABLE_FLASH_TEST_MODE		= 0x01
} DOCH_FlashTestModeCtrl;

typedef enum {
	DOCH_DISABLE_REPORT_ECC_ERRORS	= 0x00,
	DOCH_ENABLE_REPORT_ECC_ERRORS	= 0x01
} DOCH_EccErrorCtrl;


typedef enum {
	DOCH_DLMCODE_DOWNLOAD						= 0x01,	/*Download is for immediate, temporary use*/
	DOCH_DLMCODE_DOWNLOAD_AND_SAVE				= 0x07,	/*Save downloaded code for immediate & future use*/
	DOCH_DLMCODE_DOWNLOAD_AND_LAST_SAVE			= 0x87,	/*Same as code 0x07, but marking this as the last permanent
														  download accepted by the device*/
	DOCH_DLMCODE_UPLOAD							= 0xFF
} DOCH_DLMcodeModes;

/* Set of access options for DOCH_EN_ACCESS_WPWD	*/
typedef enum { DOCH_ACCESS_USER_PASSWORD	= 0x00,
			   DOCH_ACCESS_MASTER_PASSWORD	= 0x01,
			   DOCH_ACCESS_GATE_KEEPER		= 0x02,

			   DOCH_ACCESS_ALL_PARTITIONS	= 0xFF
} DOCH_Access_Op;

/*Defines for DOCHConfigHW*/
/*------------------------*/
typedef enum {
	DOCH_BURST_WRITE_MODE_CTRL	= 0x0,
	DOCH_BURST_READ_MODE_CTRL	= 0x1,
	DOCH_IPL_CTRL				= 0x2,
	DOCH_WARM_BOOT_CTRL			= 0x3,
	DOCH_POWER_DOWN				= 0x4,
	DOCH_DMA_CTRL				= 0x5,
	DOCH_DMA_ENABLE				= 0x6,
	DOCH_DMA_NEGATION_CTRL		= 0x7,
	DOCH_SLOCK					= 0x8,
	DOCH_ENDIAN_CTRL			= 0x9,
	DOCH_OPERATION_MODE_CTRL	= 0xA,
	DOCH_POWER_MODE_CTRL		= 0xB
} DOCH_HwConfigType;

/* DOCH_NUM_OF_DCONFIGHW_ITEMS should be set to number of items in DOCH_HwConfigType*/
#define DOCH_NUM_OF_DCONFIGHW_ITEMS 0xC

/*IPL Control Register*/
#define DOCH_IPL_WRITE_DISABLE       0x0000
#define DOCH_IPL_WRITE_READY         0x0001
#define DOCH_IPL_WRITE_ENABLE        0x0002

#define DOCH_IPL_ALL_CS_ENABLED		 0x0000
#define DOCH_IPL_CLOSE_2LOW_IPLS	 0x0010
#define DOCH_IPL_CLOSE_2HIGH_IPLS	 0x0020
#define DOCH_IPL_CLOSE_ALL_IPLS		 0x0030

/*Warm Boot Register*/
#define DOCH_WARM_RST_POLARITY_LOW   0x0000
#define DOCH_WARM_RST_POLARITY_HIGH  0x0001

#define DOCH_WARM_RST_BURST_ON       0x0000
#define DOCH_WARM_RST_BURST_NO_CHNG  0x0002

/*Deep Power Down Mode Register*/
#define DOCH_DPD_PIN_POL_LOW		 0x0000
#define DOCH_DPD_PIN_POL_HIGH		 0x0001

#define DOCH_DPD_PIN_DISABLED		 0x0000
#define DOCH_DPD_PIN_ENABLED		 0x0002

#define DOCH_DPD_WAKEUP_HOST_CE		 0x0000
#define DOCH_DPD_WAKEUP_ASYNC_CLK	 0x0100

/*DMA Control Register*/
#define DOCH_DMA_REQ_DISABLE         0x0000
#define DOCH_DMA_REQ_ENABLE          0x0001

#define DOCH_DMA_REQ_POL_HIGH        0x0000
#define DOCH_DMA_REQ_POL_LOW         0x0002

#define DOCH_DMA_REQ_LEVEL           0x0000
#define DOCH_DMA_REQ_EDGE            0x0004

/*SLOCK Control Register*/
#define DOCH_SLOCK_ACTIVE			 0x0001
#define DOCH_SLOCK_OFF				 0x0000

/*Burst write/read mode control*/
#define DOCH_BURST_DISABLE           0x0000
#define DOCH_BURST_ENABLE            0x0002

#define DOCH_BURST_HOLD_1_CLK        0x0000
#define DOCH_BURST_HOLD_2_CLK        0x2000

#define DOCH_BURST_LEN_4_CYC         0x0000
#define DOCH_BURST_LEN_8_CYC         0x0800
#define DOCH_BURST_LEN_16_CYC        0x1000
#define DOCH_BURST_LEN_32_CYC        0x1800

/*Endian Control Register*/
#define DOCH_END_SWAP_OFF            0x0000
#define DOCH_END_SWAP_ON	         0x0101

/*Operation Mode Register*/
#define DOCH_NON_PIPE_ACCESS         0x00
#define DOCH_PIPE_ACCESS             0x01

#define DOCH_NO_ADDR_SHIFT           0x00
#define DOCH_ADDR_SHIFT              0x02

/*Power Mode Register*/
#define DOCH_POWER_MODE_DPD          0x0001

/*Environment variables*/
typedef enum {
	DOCH_ENV_VERIFY_WRITE		= 0x01,	/* Verifies every written sector by reading it back from device */
	DOCH_ENV_ATA_DEBUG			= 0x02,	/* Retrieve debug buffer from device after completion of ATA command */
	DOCH_ENV_BLOCK_SPI			= 0x03,	/* Block SPI */
	DOCH_ENV_NO_ATA_TIMEOUT		= 0x04,	/* Disable ATA command timeout */
	DOCH_ENV_AUTO_DPD_BY_HOST	= 0x05  /* Host automatically enters device to DPD after every command*/
} DOCH_EnVars;

/*Flags for read/write operations using DMA/Burst*/
#define DOCH_USE_DMA		0x01
#define DOCH_USE_BURST		0x02

/* Strings lengths */
#define DOCH_PASS_KEY_LEN			0x80
#define DOCH_PASS_KEY_LEN_NO_NULL	(H3_PASS_KEY_LEN-1)
#define DOCH_SER_NO_LEN				0x14
#define DOCH_MODEL_NO_LEN           0x28
#define DOCH_PROG_NAME_LEN          0x10
#define DOCH_PROG_VER_LEN           0x8

/* DOCHRecoverFromPowerLoss flags */
#define DOCH_DO_NOT_REINIT_MASTER   0x1


/* DOCH error codes */
/*------------------*/
#define DOCH_ATA_ERROR_BASE	0x0
#define DOCH_SDK_ERROR_BASE	0x100
#define DOCH_FS_ERROR_BASE  0x200

typedef enum { DOCH_OK                  = 0,

				/*ATA Error codes*/

				DOCH_ATA_NO_ERROR                     = DOCH_ATA_ERROR_BASE + 0x00,
				DOCH_ATA_ERROR_MEDIA_ERROR_DETECTED   = DOCH_ATA_ERROR_BASE + 0x01,  /*ATA-4*/
				DOCH_ATA_ERROR_ADDRESS_MARK_NOT_FOUND = DOCH_ATA_ERROR_BASE + 0x01,  /*Repeat last operation*/
				DOCH_ATA_ERROR_TRACK_0_NOT_FOUND      = DOCH_ATA_ERROR_BASE + 0x02,  /*Protection violation*/
				DOCH_ATA_ERROR_NO_MEDIA               = DOCH_ATA_ERROR_BASE + 0x02,  /*ATA-4*/
				DOCH_ATA_ERROR_ABORT                  = DOCH_ATA_ERROR_BASE + 0x04,  /*Always need to be supplied*/
				DOCH_ATA_ERROR_MEDIA_CHANGE_REQUESTED = DOCH_ATA_ERROR_BASE + 0x08,  /*Not to use for now*/
				DOCH_ATA_ERROR_ID_NOT_FOUND           = DOCH_ATA_ERROR_BASE + 0x10,  /*Passing partition boundary*/
				DOCH_ATA_ERROR_MEDIA_CHANGE_DETECTED  = DOCH_ATA_ERROR_BASE + 0x20,  /*Not to use for now*/
				DOCH_ATA_ERROR_WRITE_PROTECTED        = DOCH_ATA_ERROR_BASE + 0x40,  /*on writes       - only if we run out of space*/
				DOCH_ATA_ERROR_UNCORRECTABLE_DATA	  = DOCH_ATA_ERROR_BASE + 0x40,  /*on reads        - possible for unrecoverable ECC - should not be returned by write routines*/
				DOCH_ATA_ERROR_ICRC					  = DOCH_ATA_ERROR_BASE + 0x80,  /*on DMA xfers    - to leave for now*/
	
				/*SDK Error Codes*/
				DOCH_TimedOut			 = DOCH_SDK_ERROR_BASE,
				DOCH_GeneralFailure      = DOCH_SDK_ERROR_BASE + 5,
				DOCH_AdapterNotFound     = DOCH_SDK_ERROR_BASE + 6,
				DOCH_DeviceTurnedOff     = DOCH_SDK_ERROR_BASE + 7,
				DOCH_BadParameter        = DOCH_SDK_ERROR_BASE + 10,
				DOCH_DriveNotAvailable   = DOCH_SDK_ERROR_BASE + 15,
				DOCH_DriveNotReady       = DOCH_SDK_ERROR_BASE + 20,
				DOCH_NotEnoughMemory     = DOCH_SDK_ERROR_BASE + 25,
				DOCH_ReadFault           = DOCH_SDK_ERROR_BASE + 30,
				DOCH_WriteFault          = DOCH_SDK_ERROR_BASE + 31,
				DOCH_VerifyFault         = DOCH_SDK_ERROR_BASE + 32,
				DOCH_ProtectionFault     = DOCH_SDK_ERROR_BASE + 33,
				DOCH_UnknownCmd          = DOCH_SDK_ERROR_BASE + 35,
				DOCH_DiskNotFound        = DOCH_SDK_ERROR_BASE + 40,
				DOCH_PartitionNotFound	 = DOCH_SDK_ERROR_BASE + 45,
				DOCH_PartitionLimitExceeded = DOCH_SDK_ERROR_BASE + 50,
				DOCH_FeatureNotSupported = DOCH_SDK_ERROR_BASE + 55,
				DOCH_ATABusyNotCleared	 = DOCH_SDK_ERROR_BASE + 60,
				DOCH_ATANotReady		 = DOCH_SDK_ERROR_BASE + 61,
				DOCH_ATAErrorDetected	 = DOCH_SDK_ERROR_BASE + 65

} DOCH_Error;


/*	IOCTL Codes	*/
typedef enum {
	/*Get Info*/
	SDK_IDENTIFY_DISKONCHIP_DEVICE	= 0x200,
	SDK_GET_RESET_STATUS			= 0x201,
	SDK_NOTIFY_RESET				= 0x202,
	SDK_NOTIFY_PLATFORM_RESUMED		= 0x203,
	SDK_GET_PARTITION_INFO			= 0x204,
	SDK_SET_DISK_USER_ATTR			= 0x205,
	SDK_GET_DISK_USER_ATTR			= 0x206,
	SDK_GET_CONFIG_DATA				= 0x207,
	SDK_SET_CONFIG_DATA				= 0x208,

	/*Media*/
	SDK_SET_DEFAULT_PARTITION	= 0x210,
	SDK_SET_DATA_XFER_MODE		= 0x211,
	SDK_OPTIMIZE_MEDIA			= 0x212,
	SDK_DELETE_PARTITIONS		= 0x213,
	SDK_ADD_PARTITION			= 0x214,
	SDK_UNFORMAT_DEVICE			= 0x215,
	SDK_WRITE_IPL				= 0x216,
	SDK_READ_IPL				= 0x217,

	/*IO*/
	SDK_READ_PARTITION_SECTORS	= 0x220,
	SDK_WRITE_PARTITION_SECTORS	= 0x221,
	SDK_WRITE_AND_LOCK			= 0x222,
	SDK_WIPE_SECTORS			= 0x223,
	SDK_FREE_SECTORS			= 0x224,
	SDK_PREPARE_FOR_WRITE		= 0x225,
	SDK_WIPE_SECTORS_BY_WRITING = 0x226,

	/*Flexi-Flash*/
	SDK_WRITE_FLEXI_FAST		= 0x230,
	SDK_WRITE_FLEXI_NORMAL		= 0x231,
	SDK_REWRITE_FLEXI_NORMAL	= 0x232,

	/*Hash*/
	SDK_READ_WITH_HASH_VERIFY	= 0x240,
	SDK_READ_HASH				= 0x241,
	SDK_WRITE_HASH				= 0x242,

	/*Protection*/
	SDK_ACCESS_PART_WITH_PWD	= 0x250,
	SDK_DISABLE_PART_ACCESS		= 0x251,
	SDK_SET_PART_PROTECTION		= 0x252,
	SDK_SET_PART_USER_ATTR		= 0x253,
	SDK_GET_PART_USER_ATTR		= 0x254,

	/*PKI*/
	SDK_PKI_HOST				= 0x260,
	SDK_PKI_DOCH				= 0x261,
	SDK_PKI_VERIFY_HOST_KEY		= 0x262,

	/*Custom Parameters*/
	SDK_GET_CUSTOM_PARAM		= 0x270,
	SDK_SET_CUSTOM_PARAM		= 0x271,

	/*Atomic Read/Write Sequence*/
	SDK_ATOMIC_WRITE_SEQ		= 0x280,

	/*Algorithms*/
	SDK_REPORT_SUPP_ALG			= 0x290,
	SDK_REPORT_ALG_CAPAB		= 0x291,
	SDK_SET_ALG_MODE			= 0x292,

	/*Hash*/
	SDK_AUTO_HASH_CTRL			= 0x2A0,
	SDK_READ_CALC_HASH			= 0x2A1,
	SDK_WRITE_CALC_HASH			= 0x2A2,
	SDK_READ_ORIG_HASH			= 0x2A3,
	SDK_WRITE_GIVEN_HASH		= 0x2A4,
	SDK_START_HASH_STREAM_CALC	= 0x2A5,
	SDK_READ_HASH_STREAM_CALC	= 0x2A6,
	SDK_RETURN_RAND_NUMS		= 0x2A7,
	SDK_SET_HASH_KEY			= 0x2A8,

	/*General (Control/Configuration)*/
	SDK_SET_POWER_MODE			= 0x2B0,
	SDK_GET_POWER_MODE			= 0x2B1,
	SDK_HW_CONFIG				= 0x2B2,
	SDK_RECOVER_FROM_POWER_LOSS	= 0x2B3,
	SDK_RESET_SOCKET			= 0x2B4,
	SDK_SET_ENV_VAR				= 0x2B5,
	SDK_CLEAR_ATA_INTERRUPT		= 0x2B6,
#ifndef FL_SLPP
	SDK_GET_PHYSICAL_ADDRESS	= 0x2B7
#else /*FL_SLPP*/
	SDK_GET_PHYSICAL_ADDRESS	= 0x2B7,		
	/*SLPP Specific*/
	SDK_SLPP_UNLOCK_RANGE			= 0x2C0,
	SDK_SLPP_UNLOCK_ENTIRE_PARTITION	= 0x2C1,
	SDK_SLPP_LOCK_RANGE				= 0x2C2,
	SDK_SLPP_STICKY_LOCK_RANGE		= 0x2C3,
	SDK_SLPP_REPORT_LOCKED_RANGES	= 0x2C4
#endif /*FL_SLPP*/

} SDKFunctionNo;

/* runtime socket init data */
typedef struct {
	unsigned int nDeviceAddress;
} DOCH_InitSocket;

/* Access layer routines registration structure */
typedef struct {
	unsigned char (*hal_get_ata_reg_user)	(volatile unsigned char *base, int reg);
	void		  (*hal_set_ata_reg_user)	(volatile unsigned char *base, int reg, unsigned int val);
	unsigned short(*hal_get_ctrl_reg_user)	(volatile unsigned char *base, int reg);
	void		  (*hal_set_ctrl_reg_user)	(volatile unsigned char *base, int reg, unsigned int val);
	int			  (*hal_blk_read_user)		(volatile unsigned char *base, unsigned char *buf, int sectors);
	int			  (*hal_blk_write_user)		(volatile unsigned char *base, unsigned char *buf, int sectors);
	int			  (*hal_release_user)		(int socketNo);
} DOCH_BusAccess_routines;

/********************/
/*	API Defines		*/
/********************/

/* set of DOCH ATA registers */
typedef enum
{ 
	DOCH_DATA_REG          = 0x00,
	DOCH_ERROR_REG         = 0x01,
	DOCH_FEATURES_REG      = 0x01,
	DOCH_SECTOR_CNT_REG    = 0x02,
	DOCH_SECTOR_NO_REG     = 0x03,
	DOCH_CYLINDER_LOW_REG  = 0x04,
	DOCH_CYLINDER_HIGH_REG = 0x05,
	DOCH_DRIVE_HEAD_REG    = 0x06,
	DOCH_STATUS_REG        = 0x07,
	DOCH_COMMAND_REG       = 0x07,
	DOCH_ALT_STATUS_REG    = 0x0E,
	DOCH_CONTROL_REG		   = 0x0E

} DOCH_Reg;

/* ATA registers structure */
typedef struct
{
	FLByte bFeaturesError;	/* DOCH_FEATURES_REG or DOCH_ERROR_REG	*/
	FLByte bSectorCount;		/* DOCH_SECTOR_CNT_REG	*/
	FLByte bSectorNumber;		/* DOCH_SECTOR_NO_REG	*/
	FLByte bCylLow;			/* DOCH_CYLINDER_LOW_REG	*/
	FLByte bCylHigh;			/* DOCH_CYLINDER_HIGH_REG	*/
	FLByte bDriveHead;		/* DOCH_DRIVE_HEAD_REG	*/
	FLByte bCommandStatus;	/* DOCH_COMMAND_REG	or DOCH_STATUS_REG*/
	FLByte bContorlAltStatus;	/* DOCH_CONTROL_REG	or DOCH_ALT_STATUS_REG*/

} DOCH_Registers;

/* Set of operation codes for PassThru */
typedef enum { DOCH_PASSTHRU_NO_DATA		= 0x00,
               DOCH_PASSTHRU_DATA_IN		= 0x01,
               DOCH_PASSTHRU_DATA_OUT		= 0x02
} DOCH_PassThru_Op;

typedef enum { DOCH_PASSTHRU_NO_INT		= 0x00,
               DOCH_PASSTHRU_USE_INT	= 0x04

} DOCH_PassThru_Int;


/*OPTIMIZE_MEDIA*/
#define DOCH_OPTIMIZE_BY_PREPARE_FOR_WRITE	0
#define DOCH_OPTIMIZE_DEFAULT				1

typedef enum {
	SET_DEFAULT_PARTITION_TEMP = 0x0,
	SET_DEFAULT_PARTITION_PERM = 0x1
} DOCH_DefaultPartSet;

typedef enum {
	DOCH_FAST_PARTITOIN_DELETE		= 0x0,
	DOCH_COMPLETE_PARTITION_DELETE	= 0x8
} DOCH_DeletePartitionsMode;


/******************************************************************************/
/*
 *	Partition Info structure, sub-structures and ENUMS
 */
/******************************************************************************/

/*Partition dwCommandFlagsOrStatuses Offsets*/
#define	DOCH_CFSO_FAST_AREA_SIZE_TYPE	8
#define	DOCH_CFSO_USER_AUTHENTICATED	16
#define	DOCH_CFSO_PERM_LOCKED			17
#define	DOCH_CFSO_MASTER_AUTHENTICATED	18
#ifdef FL_SOTP
#define DOCH_CFSO_SOTP_LOCKED			19
#endif /*FL_SOTP*/
#define	DOCH_CFSO_HW_LOCK_ASSERTED		30
#define	DOCH_CFSO_SW_LOCK_ASSERTED		31

/*Partition dwCommandFlagsOrStatuses Bits*/
#define	DOCH_CFSB_FAST_AREA_SIZE_TYPE	0x00000100
#define	DOCH_CFSB_USER_AUTHENTICATED	0x00010000
#define	DOCH_CFSB_PERM_LOCKED			0x00020000
#define	DOCH_CFSB_MASTER_AUTHENTICATED	0x00040000
#ifdef FL_SOTP
#define	DOCH_CFSB_SOTP_LOCKED			0x00080000
#endif /*FL_SOTP*/
#define	DOCH_CFSB_HW_LOCK_ASSERTED		0x40000000
#define	DOCH_CFSB_SW_LOCK_ASSERTED		0x80000000

/*Partition Attribute1 Offsets*/
#define	DOCH_PA1O_PARTITION_TYPE	0
#define	DOCH_PA1O_PERFORMANCE_CTRL	8
#define	DOCH_PA1O_PAGE_SIZE_EXP		16
#define	DOCH_PA1O_HASH_TYPE			24

/*Partition Attribute1 Bits*/
#define	DOCH_PA1B_PARTITION_TYPE	0x000000FF
#define	DOCH_PA1B_PERFORMANCE_CTRL	0x00000F00
#define	DOCH_PA1B_PAGE_SIZE_EXP		0x000F0000
#define	DOCH_PA1B_HASH_TYPE			0x3F000000

/*Partition Attribute2 Offsets*/
#define	DOCH_PA2O_PROTECTION_TYPE	0
#define	DOCH_PA2O_USER_MODE			6
#define	DOCH_PA2O_GUEST_MODE		9
#define	DOCH_PA2O_MASTER_CTRL		12
#define	DOCH_PA2O_ENCRYPT_TYPE		15
#define	DOCH_PA2O_OTP_BIT			21
#define	DOCH_PA2O_LOCK_CTRL			22
#define	DOCH_PA2O_MAX_AUTH_ATTEMPTS 24

/*Partition Attribute2 Bits*/
#define	DOCH_PA2B_PROTECTION_TYPE	0x00000007		/*DOCH_ProtectionType*/
#define	DOCH_PA2B_USER_MODE			0x000001C0		/*DOCH_PartitionAccessMode*/
#define	DOCH_PA2B_GUEST_MODE		0x00000E00		/*DOCH_PartitionAccessMode*/
#define	DOCH_PA2B_MASTER_CTRL		0x00007000		/*DOCH_PartitionAccessMode*/
#define	DOCH_PA2B_ENCRYPT_TYPE		0x00038000		/*DOCH_EncryptType*/
#define	DOCH_PA2B_OTP_BIT			0x00200000
#define	DOCH_PA2B_LOCK_CTRL			0x00C00000		/*DOCH_LockControl*/
#define	DOCH_PA2B_MAX_AUTH_ATTEMPTS	0x0F000000

/*	Structure that holds various partition data*/
typedef struct {
	FLByte  bDevice;		/*On which device (0/1) the partition reside*/
} DOCH_PartitionAttr;

/*Partition formatting info*/

/*API exported for partition preporties*/
typedef struct {
	FLDword		dwProtectionType;		/*DOCH_ProtectionType*/
	FLDword		dwUserAccessMode;		/*DOCH_PartitionAccessMode*/
	FLDword		dwGuestAccessMode;		/*DOCH_PartitionAccessMode*/
	FLDword		dwMasterControl;		/*DOCH_PartitionAccessMode*/
	FLDword		dwEncryptionType;		/*DOCH_EncryptType*/
	FLDword		dwLockControl;			/*DOCH_LockControl*/
	FLDword		dwMaxNumOfAuthAttempts;	/*0 = Unlimited number*/
	FLByte		bPasskey[0x80];			/*ASCII*/
} DOCH_PartitionProtectionAPI;

typedef enum {
	DOCH_PASSKEY_VALID		= 0x01,		/* Passkey in DOCH_PartitionProtectionAPI is valid */
	DOCH_ATTRIBUTES_VALID	= 0x02,		/* All attributes in DOCH_PartitionProtectionAPI besides passkey are valid */
	DOCH_LOCK_AS_OTP		= 0x04,		/* Lock the partition for writing */
	DOCH_SEQUENTIAL_OTP		= 0x08		/* Enable sequential OTP partition, must be always set with DOCH_LOCK_AS_OTP*/
} DOCH_PartitionProtectioValidity;

/*Partition Info structure*/

typedef struct {
	FLByte			bReserved1[0x4];
	FLDword			dwCommandFlagsOrStatuses;			/*Dynamic info*/
	FLDword			partitionAttributes1;
	FLDword			partitionAttributes2;
	FLDword			nPartitionSize;						/*In Sectors*/
	FLDword			nFastAreaSize;						/*In Sectors or Percent. for IPL partition: size reported in XIP*/
	FLWord			wFastAreaFactor;					/*Exponent*/
	FLByte			bReserved2[0x2];
	FLDword			wPartitionStartSector;
	FLWord			wCurrentSectorsPerTrack;
	FLWord			wDefaultSectorsPerTrack;
	FLWord			wCurrentCylinders;
	FLWord			wDefaultCylinders;
	FLWord			wCurrentHeads;
	FLWord			wDefaultHeads;
	FLByte			bReserved3[0x4];					/*44-48*/
	FLDword			dwSOTPoffset;
	FLByte			bReserved4[0xC];
	FLWord			wFastAreaSectorsInErasableUnit;		/*Exponent*/
	FLWord			wNormalAreaSectorsInErasableUnit;	/*Exponent*/
	FLWord			wRecommendedSectorsPerCluster;
	FLWord			wFastMaxRelatedSectors;
	FLWord			wNormalMaxRelatedSectors;
	FLByte			bReserved5[0xB6];
	FLByte			bPartitionKey[0x80];
	FLByte			bReserved6[0x80];
} DOCH_PartitionInfo;

typedef struct {
	FLDword		dwPartitionType;		/*DCOH_PartitionType*/
	FLDword		dwPerformanceControl;	/*DOCH_PerformanceControl*/
	FLDword		dwPageSizeExp;			/*DOCH_PageSizeExp*/
	FLDword		dwHashType;				/*DOCH_HashType*/
	FLDword		dwFastAreaSizeType;		/*DOCH_FastAreaType*/
	FLDword		dwProtectionType;		/*DOCH_ProtectionType*/
	FLDword		dwUserAccessMode;		/*DOCH_PartitionAccessMode*/
	FLDword		dwGuestAccessMode;		/*DOCH_PartitionAccessMode*/
	FLDword		dwMasterControl;		/*DOCH_PartitionAccessMode*/
	FLDword		dwEncryptionType;		/*DOCH_EncryptType*/
	FLDword		dwLockControl;			/*DOCH_LockControl*/
	FLDword		dwOtpEnabled;			/*DOCH_OTPBit*/
	FLDword		dwMaxNumOfAuthAttempts;	/*0 = Unlimited number*/
	FLNative	nPartitionSize;			/*In Sectors*/
	FLNative	nFastAreaSize;			/*In Sectors or Percent**/
	FLWord		wFastAreaFactor;		/*Exponent*/
	FLByte		bPasskey[0x80];			/*ASCII*/
#ifdef FL_SOTP
	FLByte		bSOTP;					/*SOTP partition*/
#endif /*FL_SOTP*/
} DOCH_PartitionFormatInfoAPI;

/*Partition formatting info*/
typedef struct {
	FLByte			bReserved1[0x4];
	FLDword			dwCommandFlagsOrStatuses;
	FLDword			partitionAttributes1;
	FLDword			partitionAttributes2;
	FLDword			nPartitionSize;					/*In Sectors*/
	FLDword			nFastAreaSize;					/*In Sectors or Percent*/
	FLWord			wFastAreaFactor;				/*Exponent*/
	FLByte			bReserved2[0x12];
	FLDword			dwValidity;						/*DOCH_PartitionProtectioValidity*/
	FLByte			bReserved3[0xD0];
	FLByte			bPasskey[0x80];
	FLByte			bReserved4[0x80];
} DOCH_PartitionFormatInfo;

/*Device dwCommandFlagsOrStatuses Offsets*/
#define	DOCH_DCFSO_CURRENT_XFER_MODE	0
#define	DOCH_DCFSO_CURRENT_MULTI_SECTOR	8
#define	DOCH_DCFSO_HW_LOCK_ASSERTED		30
#define	DOCH_DCFSO_SW_LOCK_ASSERTED		31

/*Device dwCommandFlagsOrStatuses Bits*/
#define	DOCH_DCFSB_CURRENT_XFER_MODE	0x00000007
#define	DOCH_DCFSB_CURRENT_MULTI_SECTOR	0x00000F00
#define	DOCH_DCFSB_HW_LOCK_ASSERTED		0x40000000
#define	DOCH_DCFSB_SW_LOCK_ASSERTED		0x80000000

/*Device Attributes1 Offsets*/
#define	DOCH_DA1O_SUPPORTED_XFER_MODES_BMP	0
#define	DOCH_DA1O_MAX_MULTI_SEC_XFER_SIZE	8
#define	DOCH_DA1O_SUPPORT_48_BITS			31

/*Device Attributes1 Bits*/
#define	DOCH_DA1B_SUPPORTED_XFER_MODES_BMP	0x000000FF
#define	DOCH_DA1B_MAX_MULTI_SEC_XFER_SIZE	0x00000F00
#define	DOCH_DA1B_SUPPORT_48_BITS			0x80000000

/*Gate Keeper Attributes Offsets*/
#define	DOCH_DA2O_AUTH_TYPE			0		/*DOCH_DeviceAuthType*/
#define	DOCH_DA2O_GUEST_ACCESS_MODE	9		/*DOCH_DeviceAccessMode*/
#define	DOCH_DA2O_MASTER_CONTROL	12		/*DOCH_DeviceAccessMode*/
#define	DOCH_DA2O_MAX_AUTH_ATTEMPTS	24

/*Gate Keeper Attributes Bits*/
#define	DOCH_DA2B_AUTH_TYPE			0x00000003
#define	DOCH_DA2B_GUEST_ACCESS_MODE	0x00000E00
#define	DOCH_DA2B_MASTER_CONTROL	0x00007000
#define	DOCH_DA2B_MAX_AUTH_ATTEMPTS	0x0F000000

/*Authentication types*/
typedef enum {
	DOCH_DEVICE_AUTH_NONE	= 0,
	DOCH_DEVICE_AUTH_PWD	= 1,
	DOCH_DEVICE_AUTH_RSA	= 2
} DOCH_DeviceAuthType;
/*Access modes*/
typedef enum {
	DOCH_DEVICE_ACCESS_MODE_FULL	= 0,
	DOCH_DEVICE_ACCESS_MODE_RO		= 1,
	DOCH_DEVICE_ACCESS_MODE_NONE	= 2
} DOCH_DeviceAccessMode;

#define DOCH_CONFIGURATION_EXISTS_SIGN	0x1F2E3D4C

typedef struct {
	FLWord		wVersion;
	FLWord		wVersionCompatability;
	FLDword		dwCommandFlagsOrStatuses;
	FLDword		dwDiskAttributes1;
	FLDword		dwRservedGateKeeperAttr;
	FLDword		dwMiscFlags;
	FLByte		bReserved2[0xC];
	FLByte		bUniqueID[0x10];
	FLByte		bReserved3[0x10];
	FLByte		bSerialNumber[0x14];
	FLByte		bModelNumber[0x28];
	FLWord		wTotalNumOfPartitions;			/*0=IPL not present, 1=Only IPL Present, ... 16=IPL partition + 15 user partitions (MAX)*/
	FLWord		wDefaultPartitionNumber;
	FLDword		dwUnformattedCapacity;			/*In Sectors*/
	FLDword		dwConfigurationPartitionExistsSign;
	FLDword		dwETFFSVER;
	FLDword		dwReservedConfigPartitionSize;	/*In Sectors*/
	FLByte		bProgrammerName[0x10];
	FLByte		bProgrammerVersion[0x8];
	FLDword		dwUnitSize;						/* In Sectors */
	FLByte		bReserved5[0x54];				/*In Sectors*/
	FLByte		bReservedGateKeeperKey[0x80];
	FLByte		bReserved6[0x80];
} DOCH_DeviceInfo;

/*Enums*/
/*=====*/
typedef enum { DOCH_PRT_TYPE_NORMAL					= 0,	/*normal*/
			   DOCH_PRT_TYPE_PAGED					= 1,	/*paged*/
			   DOCH_PRT_TYPE_PAGED_DIGEST			= 2,	/*paged + keeps digest information*/
			   DOCH_PRT_TYPE_PAGED_COMPRESSED		= 3,	/*paged + keeps data in compressed form */
			   DOCH_PRT_TYPE_PAGED_DIGEST_COMPRESSED= 4		/*paged + keeps data in compressed form + digest*/
} DCOH_PartitionType;


/*Performance Control*/
typedef enum { DOCH_NORMAL_PERFORMANCE	= 0,	/*MLC*/
			   DOCH_FIRM_FLASH_OPTIMIZE	= 1,	/*Rigid MLC/SLC division, fast area field defines division*/
			   DOCH_FLEXI_FLASH_OPTIMIZE= 2		/*Adaptive MLC/SLC division, fast area field defines size of spare capacity for performance optimization*/
} DOCH_PerformanceControl;

/*Page size exponent (for paged partitions)*/
typedef enum { DOCH_PAGE_SIZE_512B	= 0,
			   DOCH_PAGE_SIZE_1KB	= 1,
			   DOCH_PAGE_SIZE_2KB	= 2,
			   DOCH_PAGE_SIZE_4KB	= 3,
			   DOCH_PAGE_SIZE_8KB	= 4,
			   DOCH_PAGE_SIZE_16KB	= 5,
			   DOCH_PAGE_SIZE_32KB	= 6
} DOCH_PageSizeExp;

/*Page size exponent (for paged partitions)*/
typedef enum { DOCH_HASH_TYPE_SHA1	= 0
} DOCH_HashType;

/*Fast Area Type*/
typedef enum { DOCH_FAST_AREA_TYPE_SECTORS	= 0,
			   DOCH_FAST_AREA_TYPE_PERCENT	= 1
} DOCH_FastAreaType;

/*Protection Type*/
typedef enum { DOCH_PARTITION_NOT_PROTECTED		= 0,
			   DOCH_PARTITION_PWD_PROTECTED		= 1,
			   DOCH_PARTITION_RSA_PROTECTED		= 2,
			   DOCH_PARTITION_SLPP_PROTECTED	= 3
} DOCH_ProtectionType;

#define DOCH_MAX_PWD_ATTEMPTS	5	/*"0" for Unlimited # of attempts*/

/*Partition access mode*/
typedef enum {
	DOCH_PART_ACCESS_MODE_FULL	= 0,
	DOCH_PART_ACCESS_MODE_RO	= 1,
	DOCH_PART_ACCESS_MODE_NONE	= 2
} DOCH_PartitionAccessMode;

/*Encryption Type*/
typedef enum { DOCH_ENCRYPT_NONE	= 0,	/*Partition is not encrypted*/
			   DOCH_ENCRYPT_AES_CTR	= 1,	/*Partition is AES-encrypted, CTR mode*/
			   DOCH_ENCRYPT_RC4		= 2		/*Partition is RC4-encrypted*/
} DOCH_EncryptType;


/*Lock Control*/
typedef enum { DOCH_LOCK_NOT_ACTIVE	= 0,/*LOCK# signal or soft-lock do not affect the access to this partition*/
			   DOCH_LOCK_ACTIVE		= 1 /*After LOCK# signal is active or soft-lock is written, access to partition*/
										/*can not be authenticated. If access has already been authenticated, it remains*/
} DOCH_LockControl;

/*OTP Bit*/
typedef enum { DOCH_OTP_BIT_OFF	= 0,
			   DOCH_OTP_BIT_ON	= 1
} DOCH_OTPBit;

typedef enum
{
	PARTITION_TYPE_IPL		= 0x00,
	PARTITION_TYPE_BDTL		= 0x01,
	PARTITION_TYPE_BINARY	= 0x02,
	PARTITION_TYPE_OTP		= 0x03

} DOCH_PartitionType;

/******************************************************************************/
/*
 *	Partition Access structure
 */
/******************************************************************************/

/*Structure for DOCH_VSCMD_EXT_DEVICE_CTRL::DOCH_SET_PARTITION_PROTECTION
  and DOCH_VSCMD_EXT_SECURITY_CTRL::DOCH_EN_ACCESS_WPWD*/
typedef struct {
	FLByte bReserved1[0x100];
	FLByte bPassword[0x80];
	FLByte bReserved2[0x80];
} DOCH_PartitionAcessPassword;

/******************************************************************************/
/*
 *	PKI structures
 */
/******************************************************************************/

#define DOCH_CHLNG_SIZE		0x40
#define DOCH_PKI_KEY_SIZE	0x80

/*Structure for DOCH_VSCMD_EXT_SECURITY_CTRL::DOCH_TX_HOST_PUBLICKEY*/
typedef struct {
	FLWord wVersion;
	FLByte bReserved1[0xBE];
	FLByte bHostRandomChallange[DOCH_CHLNG_SIZE];
	FLByte bHostPublicKey[DOCH_PKI_KEY_SIZE];
	FLByte bReservedForLargerKey[0x80];
} DOCH_HostPublicKey;

/*Structure for DOCH_VSCMD_EXT_SECURITY_CTRL::DOCH_RX_DOCH_PUBLICKEY*/
typedef struct {
	FLWord wVersion;
	FLByte bReserved1[0x3E];
	FLByte bHostRandomChallangeByDochPrivateKey[DOCH_CHLNG_SIZE];
	FLByte bReserved2[0x40];
	FLByte bDochRandomChallange[DOCH_CHLNG_SIZE];
	FLByte bDochPublicKey[DOCH_PKI_KEY_SIZE];
	FLByte bReservedForLargerKey[0x80];
} DOCH_DochPublicKey;

/*Structure for DOCH_VSCMD_EXT_SECURITY_CTRL::DOCH_VERIFY_HOST_KEY*/
typedef struct {
	FLWord	wVersion;
	FLByte	bReserved1[0x3E];
	FLByte	bDochRandomChallangeByHostPrivateKey[DOCH_CHLNG_SIZE];
	FLByte	bReserved2[0x180];
} DOCH_VerifyHostKey;

/******************************************************************************/
/*
 *	Algorithm structures
 */
/******************************************************************************/

/*Supported algorithms retrieval structure*/
/*----------------------------------------*/
typedef struct {
	FLWord	wVersion;
	FLByte	bReserved1[0x2];
	FLDword	wSymmetricAlgorithmsSupported;		/*Bitmap: AES...*/
	FLDword	wAsymmetricAlgorithmsSupported;		/*Bitmap: RSA...*/
	FLDword	wHashSchemesSupported;				/*Bitmap: SHA-1...*/
	FLDword	wRNGSchemesSupported;				/*Bitmap: TrueRandom...*/
	FLDword	wDecompressionSchemesSupported;		/*Bitmap: LZ77...*/
	FLDword	wCompressionSchemesSupported;		/*Bitmap: LZ77...*/
	FLByte	bReserved2[0x1E4];
} DOCH_supportedAlgorithms;

/*Specific algorithm capabilities retrieval structure and sub-structures*/
/*----------------------------------------------------------------------*/
typedef struct {
	FLDword	dwSize;
	FLDword	dwCalcTime;	/*For the key, in nano*/
} KeyAttr;

typedef struct {
	FLByte	bName[8];		/*In ASCII*/
	FLWord	wKeySizes;		/*Bitmap of key sizes supported by this mode*/
	FLByte	bReserved[0x6];
} ModeAttr;

typedef struct {
	FLWord		wVersion;
	FLByte		bReserved1[0x2];
	FLByte		bAlgorithmName[0x10];	/*In ASCII*/
	FLWord		wNumOfKeySizesSupported;
	FLWord		wNumOfModesSupported;
	FLByte		bReserved2[0x8];
	KeyAttr		keysAttributes[0x8];
	ModeAttr	modesAttributes[0x18];
} DOCH_AlgorithmCapabilities;

/*Enums for HASH control*/
/*----------------------*/
typedef enum {
	DOCH_HASH_REINITIALIZE			= 0x00,
	DOCH_HASH_CONTINUE				= 0x01,
	DOCH_HASH_CONTINUE_ACCUM		= 0x00,
	DOCH_HASH_STOP_ACCUM			= 0x01,
	DOCH_HASH_DONT_RETURN_DATA		= 0x00,
	DOCH_HASH_RETURN_ACCUM_DIGEST	= 0x02
} DOCH_HashStartStream;

typedef enum {
	DOCH_HASH_DECRYPTION_KEY		= 0x00,
	DOCH_HASH_ENCRYPTION_KEY		= 0x01,

	/*For Decryption*/
	DOCH_HASH_USE_DOCH_PUBLIC_KEY		= 0x00,
	DOCH_HASH_USE_HOST_PUBLIC_KEY		= 0x01,
	DOCH_HASH_USE_PARTITION_PUBLIC_KEY	= 0x02,

	/*For Encryption*/
	DOCH_HASH_USE_DOCH_PRIVATE_KEY		= 0x00,
	DOCH_HASH_USE_HOST_PRIVATE_KEY		= 0x01
} DOCH_HashSetKey;

#ifdef FL_SLPP
/*SLPP Specific*/
/* should be replaced by the real values/structures once they are defined!!! */

#define DOCH_SLPP_MAX_LOCK_RANGES 16

typedef enum {
	DOCH_SLPP_UNLOCKED_RANGE		= 1,
	DOCH_SLPP_PERM_LOCKED_RANGE		= 2
} DOCH_SLPPRangeType;

typedef struct {
	FLDword dwRangeStartSector;
	FLDword dwRangeSizeInSectors;
	FLDword lockType;
} DOCH_SLPPRange;

typedef struct {
	FLDword numOfActiveRanges;
	DOCH_SLPPRange rangeParams[DOCH_SLPP_MAX_LOCK_RANGES];
	FLByte spare[0x200 - (DOCH_SLPP_MAX_LOCK_RANGES*sizeof(DOCH_SLPPRange)) - sizeof(FLDword)];
} DOCH_SLPPReport;

typedef struct {
	FLDword dwRangeStartSector;
	FLDword dwRangeSizeInSectors;
	FLByte passkey[0x80];
	FLByte spare[0x178];
} DOCH_SLPPRangeRequestSector;
#endif /*FL_SLPP*/
