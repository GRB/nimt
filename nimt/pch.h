#pragma once

/*Bellow we set the device name and the symbolic link name. both strings should have the format "\\Device\\device name" and "\\??\\symbolic link name"*/
#define devicename RTL_CONSTANT_STRING(L"\\Device\\nimt")
#define symlinkname RTL_CONSTANT_STRING(L"\\??\\nimt")

/*Bellow we set the CTL_CODES to be used on the communication
* Params:
* 1- DeviceType. Microsoft state that 3rd party should start with 0x8000
* 2- Function. A incremental number, never to be repeated on same driver. Microsoft state that 3rd party should start on 0x800
* 3- Method. This is where we set the kind of cumunication we want with the driver.
* 4- Access. Type of access.
* https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes
*/
#define READ_MEMORY CTL_CODE(0x8069, 0x869, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define WRITE_MEMORY CTL_CODE(0x8069, 0x870, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*Bellow we set just a inicial string for when we kdprint*/
#define DEBUG_TO_PRINT "Nimt: "

/*Bellow are the includes needed for the project in their respective order*/
#include <ntifs.h>
#include <ntddk.h>