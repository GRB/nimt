#include "pch.h"
#include "operations.h"

/// <summary>
/// Function to actually read the memory using the kernel functions
/// </summary>
/// <param name="process_id">INT</param>
/// <param name="address">POID</param>
/// <param name="buffer">PVOID</param>
/// <param name="buffer_size">SIZE_T</param>
/// <returns>NTSTATUS</returns>
NTSTATUS Nimt::ReadMemory(_In_ INT process_id,_In_ PVOID address, _In_ PVOID buffer, _In_ SIZE_T buffer_size) {
	/*Bellow we initialize the status as a success, and work it down the function*/
	NTSTATUS status = STATUS_SUCCESS;
	
	/*Bellow we check to see if the buffer is empty and exit, couse theres nothing to be done since we cant report back the read*/
	if (buffer == 0) {
		return STATUS_INVALID_PARAMETER;
	}

	/*Bellow we start two processes, source and target */
	PEPROCESS source = nullptr;
	PEPROCESS target = IoGetCurrentProcess();

	do
	{
		/*Bellow we look for the process id provided by the user application*/
		status = PsLookupProcessByProcessId((HANDLE*)process_id, &source);

		/*Bellow we check to see if we actually found the process id specified, and if fail return immediatly*/
		if (!NT_SUCCESS(status)) {
			status = STATUS_NOT_FOUND;
			break;
		}

		/*Bellow we use a try except couse if this fail we dont BSOD*/
		__try {
			/*Bellow we copy the virtual memory to a readable space. We pass nullptr in the end couse here we dont care about the size of the bytes copied. We are passing the structure outside the function*/
			status = MmCopyVirtualMemory(source, address, target, buffer, buffer_size, UserMode, nullptr);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			status = STATUS_UNSUCCESSFUL;
		}

		/*Bellow we check the the status of the mmcopy. if it fails, we exit.*/
		if (!NT_SUCCESS(status)) {
			break;
		}

	} while (false);

	/*Bellow we decrement the object count to prevent leaks*/
	ObDereferenceObject(source);

	/*Finally we return from this function with the status of the operation*/
	return status;
}

/// <summary>
/// Function to actually write to memory using the kernel functions
/// </summary>
/// <param name="process_id">INT</param>
/// <param name="address">POID</param>
/// <param name="buffer">PVOID</param>
/// <param name="buffer_size">SIZE_T</param>
/// <returns>NTSTATUS</returns>
NTSTATUS Nimt::WriteMemory(_In_ INT process_id, _In_ PVOID address, _In_ PVOID buffer, _In_ SIZE_T buffer_size) {
	/*Bellow we initialize the status as a success, and work it down the function*/
	NTSTATUS status = STATUS_SUCCESS;

	/*Bellow we start two processes, source and target */
	PEPROCESS source = nullptr;
	PEPROCESS target = IoGetCurrentProcess();

	do
	{
		/*Bellow we look for the process id provided by the user application*/
		status = PsLookupProcessByProcessId((HANDLE*)process_id, &source);

		/*Bellow we check to see if we actually found the process id specified, and if fail return immediatly*/
		if (!NT_SUCCESS(status)) {
			status = STATUS_NOT_FOUND;
			break;
		}

		/*Bellow we use a try except couse if this fail we dont BSOD*/
		__try {
			/*Bellow we copy the buffer passed from user application to the address in memory. We pass nullptr in the end couse here we dont care about the size of the bytes copied. We are passing the structure outside the function*/
			status = MmCopyVirtualMemory(target, buffer, source, address, buffer_size, UserMode, nullptr);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			status = STATUS_UNSUCCESSFUL;
		}

		/*Bellow we check the the status of the mmcopy. if it fails, we exit.*/
		if (!NT_SUCCESS(status)) {
			break;
		}

	} while (false);

	/*Bellow we decrement the object count to prevent leaks*/
	ObDereferenceObject(source);

	/*Finally we return from this function with the status of the operation*/
	return status;
}
