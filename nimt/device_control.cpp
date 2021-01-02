#include "pch.h"
#include "device_control.h"
#include "operations.h"

/// <summary>
/// Function where we control the information passed by the CTL_CODE
/// inside the stack.Parameters.DeviceIoControl
/// </summary>
/// <param name="device_object">PDEVICE_OBJECT</param>
/// <param name="irp">PIRP</param>
/// <returns>NTSTATUS</returns>
NTSTATUS Nimt::DeviceControl(_In_ PDEVICE_OBJECT device_object, _In_ PIRP irp) {
    /*
    * UNREFERENCED_PARAMETER so that the compiler dont complain we dont do nothing with
    * that variable the function needs
    * We could just pass the structure and not pass the variable
    */
    UNREFERENCED_PARAMETER(device_object);

    /*Bellow we initialize the status as invalid device request and change it if we actually suceed into doing any operation down bellow and initialize the number or worked bytes.*/
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    SIZE_T bytes_worked = 0;

    /*Bellow we get the current stack*/
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

    /*Bellow we check that we actually got the stack. If not we return imediatly becouse we cant anything else here*/
    if (!stack) {
        return status;
    }

    /*Bellow we get the current ctl_code that was sent from the user application*/
    INT ctl_code = stack->Parameters.DeviceIoControl.IoControlCode;

    /*Bellow we run the specified operation*/
    switch (ctl_code) {
        case READ_MEMORY: {
            /*Bellow we get the input buffer size and the output buffer size*/
            ULONG inputbufferlength = stack->Parameters.DeviceIoControl.InputBufferLength;
            ULONG outputbufferlength = stack->Parameters.DeviceIoControl.OutputBufferLength;

            /*Bellow we check if both buffers have the correct size for the information and to return it*/
            if (inputbufferlength < sizeof(MemoryStruct) || outputbufferlength < sizeof(MemoryStruct)) {
                /*We set the status as a fail becouse we could not read all the information or store it and exit becouse we cant do anything else*/
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            /*Bellow we set our buffer with the same structure of the information that was sent from the user application*/
            MemoryStruct* buffer = (MemoryStruct*)irp->AssociatedIrp.SystemBuffer;

            /*Bellow we check to see if we actually managed to set the buffer with our structure*/
            if (!buffer) {
                /*We set the status to invalid buffer and exit becouse we cant do anything else*/
                status = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /*Bellow we call our read function and pass the correct parameters*/
            Nimt::ReadMemory(buffer->process_id, buffer->address, buffer->buffer, buffer->buffer_size, buffer->size_copied);

            /*Bellow we set the number of bytes worked to the same size as the struct of the buffers*/
            bytes_worked = sizeof(MemoryStruct);
            break;
        }
        case WRITE_MEMORY: {
            /*Bellow we get the input buffer size and the output buffer size*/
            ULONG inputbufferlength = stack->Parameters.DeviceIoControl.InputBufferLength;
            ULONG outputbufferlength = stack->Parameters.DeviceIoControl.OutputBufferLength;

            /*Bellow we check if both buffers have the correct size for the information and to return it*/
            if (inputbufferlength < sizeof(MemoryStruct) || outputbufferlength < sizeof(MemoryStruct)) {
                /*We set the status as a fail becouse we could not read all the information or store it and exit becouse we cant do anything else*/
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            /*Bellow we set our buffer with the same structure of the information that was sent from the user application*/
            MemoryStruct* buffer = (MemoryStruct*)irp->AssociatedIrp.SystemBuffer;

            /*Bellow we check to see if we actually managed to set the buffer with our structure*/
            if (!buffer) {
                /*We set the status to invalid buffer and exit becouse we cant do anything else*/
                status = STATUS_INVALID_BUFFER_SIZE;
                break;
            }

            /*Bellow we call our read function and pass the correct parameters*/
            Nimt::WriteMemory(buffer->process_id, buffer->address, buffer->buffer, buffer->buffer_size, buffer->size_copied);

            /*Bellow we set the number of bytes worked to the same size as the struct of the buffers*/
            bytes_worked = sizeof(MemoryStruct);
            break;
        }
    }

    /*Bellow we set the size of the bytes worked. This will mostly be the size of the structure used to comunicate and pass information. Its the size of the copied buffer*/
    irp->IoStatus.Information = (UINT32)bytes_worked;

    /*Bellow we set the status of the returning operation. This should be always almost the same status that originate the IRP*/
    irp->IoStatus.Status = status;

    /*Bellow finally we complete the request made. IO_NO_INCREMENT used bellow if we dont want to boost the thread*/
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    /*Bellow we return the status of the operation*/
    return status;
}
