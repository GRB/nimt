#pragma once

namespace Nimt {
	/*Bellow we create a structure with the type of information we need to be passing for the comunication
	Since this is a showcase for READ and WRITE only one struct is enough*/
	struct MemoryStruct {
		INT process_id = 0;		// the process id aka PID of the preocess we gona mess with
		PVOID address;			// the address to be read/write
		PVOID buffer;			// the buffer
		SIZE_T buffer_size;		// the size of the buffer
		SIZE_T size_copied;		// the size of bytes copied
	};


	NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT device_object, _In_ PIRP irp);
}