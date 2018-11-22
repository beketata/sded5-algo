#pragma once


#include "standard.h"


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
//#define ENDPT_NUMBER 8
//#define INTERFACE_NO 2
#define ENDPT_NUMBER 6
#define INTERFACE_NO 1
#define SWAP(x)    ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
#define CONFIG_LENGTH ( sizeof(CONFIGURATION_DESCRIPTOR) + ( INTERFACE_NO * sizeof( INTERFACE_DESCRIPTOR ) ) + ( ENDPT_NUMBER * sizeof( ENDPOINT_DESCRIPTOR ) ) )

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdUSB;
	BYTE bDeviceClass;
	BYTE bDeviceSubClass;
	BYTE bDeviceProtocol;
	BYTE bMaxPacketSize0;
	WORD idVendor;
	WORD idProduct;
	WORD bcdDevice;
	BYTE iManufacturer;
	BYTE iProduct;
	BYTE iSerialNumber;
	BYTE bNumConfigurations;

} DEVICE_DESCRIPTOR;

typedef struct
{
	BYTE bLength; //length of HS Device Descriptor
	BYTE bQualifier;  //HS Device Qualifier Type
	WORD wVersion; // USB 2.0 version
	BYTE bDeviceClass; //Device class
	BYTE bDeviceSubClasss; //Device SubClass
	BYTE bProtocol; //Device Protocol Code
	BYTE MaxPktSize; //Maximum Packet SIze for other speed
	BYTE bOther_Config; //Number of Other speed configurations
	BYTE Reserved; //Reserved

} HS_DEVICE_QUALIFIER;

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	WORD wTotalLength;
	BYTE bNumInterfaces;
	BYTE bConfigurationValue;
	BYTE iConfiguration;
	BYTE bmAttributes;
	BYTE MaxPower;

} CONFIGURATION_DESCRIPTOR;

typedef struct
{
	BYTE bLength; //length of other speed configuration descriptor
	BYTE bDescriptorType; //Other speed configuration Type
	WORD bTotalLength; //Total length
	BYTE No_Interface; //No of interface supported
	BYTE Configuration_Value; //Configuration Value
	BYTE String_Index_Descriptor; //Index of string descriptor
	BYTE Config_Characteristic; //Configurations Characteristic
	BYTE Max_Power; //Maximun Power

}OTHER_SPEED_CONFIG_DESCRIPTOR;

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	BYTE bInterfaceNumber;
	BYTE bAlternateSetting;
	BYTE bNumEndpoints;
	BYTE bInterfaceClass;
	BYTE bInterfaceSubClass;
	BYTE bInterfaceProtocol;
	BYTE iInterface;
} INTERFACE_DESCRIPTOR;

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	BYTE bEndpointAddress;
	BYTE bmAttributes;
	WORD wMaxPacketSize;
	BYTE bInterval;

} ENDPOINT_DESCRIPTOR;

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	BYTE bString[2];

} STRING_ID;

typedef struct
{
	BYTE bLength;
	BYTE bDescriptorType;
	BYTE bString[30];

} STRING_DESCRIPTOR;
