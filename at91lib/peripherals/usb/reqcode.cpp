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
#include "common.h"
#include "standard.h"
#include "Usb.h"
#include "kernel.h"
#include "reqcode.h"


//***********************************************************************
//*                *
//*                      Variable Definition        *
//*                *
//***********************************************************************
//***********************************************************************
//*                *
//*                      Routine Definition    *
//*                *
//***********************************************************************
//***********************************************************************
//*                     *
//* Routine  :  Standard Device Descriptor                        *
//* Input :  None                        *
//* Output   :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const DEVICE_DESCRIPTOR USB_Standard_Device_Descriptor[] =
{
	sizeof( DEVICE_DESCRIPTOR ),								// length 
	0x01,																				//    1. bDescriptorType
	0x0110,															// 2.-3. Version 1.1 USB spec Word
	0x00,																				//    4. Class
	0x00,																				//    5. bDeviceSubClass
	0x00,																				//    6. bDeviceProtocol
	0x40,																				//    7. bMaxPacketSize0
	0x6022,															// 8.-9. idVendor
	0x5000,    /* a.-b.  idProduct for no Filter Driver      */
	0x0000,    /* c.-d.  bcdDevice                  */
	0x00,    /* e.    iManufacturer             */
	0x00,     /* f.    iProduct      */
	0x00,     /* 10.    iSerialNumber              */
	0x01      /* 11.    bNumConfigurations  */
};

alignas(2)
const DEVICE_DESCRIPTOR USB_HS_Device_Descriptor[] =
{

	sizeof( DEVICE_DESCRIPTOR ), /*        length  */
	0x01,     /*  1.    bDescriptorType            */
	0x0200,    /*  2.-3. Version 2.0 USB spec Word  */
	0x00,       /*  4.    Class                      */
	0x00,     /*  5.    bDeviceSubClass            */
	0x00,     /*  6.    bDeviceProtocol            */
	0x40, /*  7.    bMaxPacketSize0            */
	0x6022, /* 8.-9.  idVendor                   */
	0x5000,    /* a.-b.  idProduct                  */
	0x0000,    /* c.-d.  bcdDevice                  */
	0x00,    /* e.    iManufacturer             */
	0x00,     /* f.    iProduct      */
	0x00,     /* 10.    iSerialNumber              */
	0x01      /* 11.    bNumConfigurations  */
};

//***********************************************************************
//*                     *
//* Routine  :  HS Device Qualifier                               *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const HS_DEVICE_QUALIFIER USB_HS_Device_Qualifier[] =
{
	sizeof( HS_DEVICE_QUALIFIER ), //length of HS Device Descriptor
	0x06, //HS Device Qualifier Type
	0x0200, // USB 2.0 version
	0x00, //Device class
	0x00, //Device SubClass
	0x00,  //Device Protocol Code
	0x40, //Maximum Packet SIze for other speed
	0x01, //Number of Other speed configurations
	0x00 //Reserved
};

//***********************************************************************
//*                     *
//* Routine  :  Standard Configuration Descriptor                 *
//* Input :  None                        *
//* Output   :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const CONFIGURATION_DESCRIPTOR USB_Standard_Config_Descriptor[] =
{
	sizeof( CONFIGURATION_DESCRIPTOR ), //Length of the configuration Descriptor
	0x02,   //Configuration Descriptor
	CONFIG_LENGTH,      //Total length of the configurations,interface and class/endpoint
	0x01,   //No. of interface supported
	0x01,   //Configuration Value
	0x00,   //Index of string descriptor
	0xC0,   //Configurations Characteristic
	0x32   //Maximun Power
};

//***********************************************************************
//*                     *
//* Routine  :  Other speed Configuration Descriptor              *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const OTHER_SPEED_CONFIG_DESCRIPTOR USB_Other_Speed_Config_Descriptor[] =
{
	sizeof( OTHER_SPEED_CONFIG_DESCRIPTOR ), //length of other speed configuration descriptor
	0x07, //Other speed configuration Type
	CONFIG_LENGTH, //Total length
	0x01, //No of interface supported
	0x01, //Configuration Value
	0x00, //Index of string descriptor
	0xC0, //Configurations Characteristic
	0x32 //Maximun Power
};

//***********************************************************************
//*                     *
//* Routine  :  Standard Interface Descriptor                     *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const INTERFACE_DESCRIPTOR USB_Bulk_Interface_Descriptor[] =
{
	sizeof( INTERFACE_DESCRIPTOR ), //Length of standard interface descriptor
	0x04, //Standard Interface Type
	0x00, //No of Interface
	0x00, //Alternate Setting Selector
	0x06, //No of Endpoints
	0x00, //Interface Class
	0x00, //ATAPI Interface Sub Class
	0x00, //Bulk Only Interface Protocol
	0x00 //Index of interface string desriptor
};

alignas(2)
const INTERFACE_DESCRIPTOR USB_HS_Bulk_Interface_Descriptor[] =
{
	sizeof( INTERFACE_DESCRIPTOR ), //Length of standard interface descriptor
	0x04, //Standard Interface Type
	0x00, //No of Interface
	0x00, //Alternate Setting Selector
	0x06, //No of Endpoints
	0x00, //Interface Class
	0x00, //ATAPI Interface Sub Class
	0x00, //Bulk Only Interface Protocol
	0x00 //Index of interface string desriptor
};

//***********************************************************************
//*                     *
//* Routine  :  Standard Endpoint Descriptor                     *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const ENDPOINT_DESCRIPTOR USB_Bulk_Out_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x81,  //Set Bulk endpoint to 1 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_Bulk_In_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x01,  //Set Bulk endpoint to 1 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_HS_Bulk_Out_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x81,  //Set Bulk endpoint to 1 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0200, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_HS_Bulk_In_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x01,  //Set Bulk endpoint to 1 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0200, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_Int_Out_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x82,  //Set Int endpoint to 2 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_Int_In_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x02,  //Set Int endpoint to 2 for ATAPI mode
	0x02,   //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_HS_Int_Out_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x82,  //Set Int endpoint to 2 for ATAPI mode
	0x02, //Endpoint Characteristic
	0x0200, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_HS_Int_In_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x02,  //Set Int endpoint to 2 for ATAPI mode
	0x02,   //Endpoint Characteristic
	0x0200, //Endpoint Max Packet Size
	0x00 //Interval for Polling Endpoint for data transfer
};

//***********************************************************************
//*                     *
//* Routine  :  Standard Interface Descriptor                     *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
/*
alignas(2)
const INTERFACE_DESCRIPTOR Iso_Interface_Descriptor[] =
{
	sizeof( INTERFACE_DESCRIPTOR ), //Length of standard interface descriptor
	0x04, //Standard Interface Type
	0x00, //No of Interface
	0x01, //Alternate Setting Selector
	0x02, //No of Endpoints
	0x00, //Interface Class
	0x00, //Interface Sub Class
	0x00, //Interface Protocol
	0x00 //Index of interface string desriptor
};
*/
alignas(2)
const ENDPOINT_DESCRIPTOR USB_Iso_Out_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x03,  //Enndpoint Address
	0x03, //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x01 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR USB_Iso_In_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x83,  //Enndpoint Address
	0x03, //Endpoint Characteristic
	0x0040, //Endpoint Max Packet Size
	0x01 //Interval for Polling Endpoint for data transfer
};
/*
alignas(2)
const ENDPOINT_DESCRIPTOR Iso_Out_Zero_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x03,  //Enndpoint Address
	0x01, //Endpoint Characteristic
	0x0000, //Endpoint Max Packet Size
	0x01 //Interval for Polling Endpoint for data transfer
};

alignas(2)
const ENDPOINT_DESCRIPTOR Iso_In_Zero_Descriptor[] =
{
	sizeof( ENDPOINT_DESCRIPTOR ), //Length of Standard Endpoint Descriptor
	0x05, //Standard Endpoint Type
	0x83,  //Enndpoint Address
	0x01, //Endpoint Characteristic
	0x0000, //Endpoint Max Packet Size
	0x01 //Interval for Polling Endpoint for data transfer
};
*/
//***********************************************************************
//*                     *
//* Routine  :  Standard String Descriptor                        *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
/*
alignas(2)
const STRING_DESCRIPTOR Standard_Product_String[] =
{
	0x30,    // length
	0x03,     // descriptor type = string
	'P' , 0x00,
	'h' , 0x00,
	'i' , 0x00,
	'l' , 0x00,
	'i' , 0x00,
	'p' , 0x00,
	's' , 0x00,
	' ' , 0x00,
	'I' , 0x00,
	'S' , 0x00,
	'P' , 0x00,
	'1' , 0x00,
	'5' , 0x00,
	'8' , 0x00,
	'1' , 0x00,
	' ' , 0x00,
	'V' , 0x00,
	'e' , 0x00,
	'r' , 0x00,
	' ' , 0x00,
	'1' , 0x00,
	'.' , 0x00,
	'0' , 0x00
};
*/
//***********************************************************************
//*                     *
//* Routine  :  Standard String Descriptor                        *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
/*
alignas(2)
const STRING_DESCRIPTOR Standard_Manufacturer_String[] =
{
	0x1C,    // length
	0x03,     // descriptor type = string
	'P' , 0x00,
	'h' , 0x00,
	'i' , 0x00,
	'l' , 0x00,
	'i' , 0x00,
	'p' , 0x00,
	' ' , 0x00,
	'-' , 0x00,
	' ' , 0x00,
	'A' , 0x00,
	'P' , 0x00,
	'I' , 0x00,
	'C' , 0x00
};
*/
//***********************************************************************
//*                     *
//* Routine  :  Standard String Identifications                   *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
alignas(2)
const STRING_ID USB_Standard_String_ID[] =
{
	0x04,
	0x03,
	0x09,
	0x04
};

//***********************************************************************
//*                     *
//* Routine  :  Standard Mass Storage Identifications                 *
//* Input :  None                        *
//* Output :  None                                    *
//* Function :  None                                          *
//*                *
//***********************************************************************
/*
alignas(2)
const BYTE Test_Packet[] =
{
	// 0x00, 0x00,
	// 0x00, 0x80, // syn pattern
	0xc3,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xee,  //aa*4
	0xee, 0xee,
	0xee, 0xee,
	0xee, 0xee,
	0xee, 0xfe,  //ee*4
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,  //FF*6
	0xff, 0x7f,
	0xbf, 0xdf,
	0xef, 0xf7,
	0xfb, 0xfd,
	0xfc, 0x7e,
	0xbf, 0xdf,
	0xef, 0xf7,
	0xfb, 0xfd,
	0x7e,
	0xb6, 0xce  // crc
							// 0xff, 0xf7
};
*/
