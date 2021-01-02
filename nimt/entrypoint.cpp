#include "pch.h"
#include "device_control.h"

/// <summary>
/// Fake Function to pass to DriverObject->Major so its not needed to declare all
/// the IRP_MJ properties. We set them all at once bellow
/// </summary>
/// <param name="device_object">PDEVICE_OBJECT</param>
/// <param name="irp">PIRP</param>
/// <returns>NTSTATUS</returns>
/// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-major-function-codes
NTSTATUS FakeMajorFunction(_In_ PDEVICE_OBJECT device_object, _In_ PIRP irp) {
	/*
	* UNREFERENCED_PARAMETER so that the compiler dont complain we dont do nothing with
	* those 2 variables the function needs
	* We could just pass the structures and not pass the variables
	*/
	UNREFERENCED_PARAMETER(device_object);
	UNREFERENCED_PARAMETER(irp);
	return STATUS_SUCCESS;
}

/// <summary>
/// Function to be called when driver is unloaded
/// </summary>
/// <param name="driver_object"></param>
/// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
void Nimt_Unload(_In_ PDRIVER_OBJECT driver_object) {
	/*Bellow we create a unicode string that is the symlink name*/
	UNICODE_STRING symlink = symlinkname;

	/* https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletesymboliclink */
	/*Bellow we delete the symbolic link based on the symbolic link unicode string we just created*/
	IoDeleteSymbolicLink(&symlink);

	/* https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice */
	/*Bellow we delete the device that was created before*/
	IoDeleteDevice(driver_object->DeviceObject);
}

/// <summary>
/// DriverEntry point. Where all begins
/// </summary>
/// <param name="driver_object"></param>
/// <param name="registry_path"></param>
/// <returns>NTSTATUS</returns>
/// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_initialize
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driver_object, _In_ PUNICODE_STRING registry_path) {
	/*
	* UNREFERENCED_PARAMETER so that the compiler dont complain we dont do nothing with
	* that variable the function needs
	* We could just pass the structure and not pass the variable
	*/
	UNREFERENCED_PARAMETER(registry_path);

	/*Bellow we set the function to be called when the driver is unloaded*/
	driver_object->DriverUnload = Nimt_Unload;

	/*Bellow we iterate over all the IRP_MJ functions and set it the FakeMajorFunction that is only a return STATUS_SUCCESS*/
	for (unsigned int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i) {
		driver_object->MajorFunction[i] = FakeMajorFunction;
	}

	/*Bellow we set the only IRP_MJ function we care about that is the IRP_MJ_DEVICE_CONTROL*/
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Nimt::DeviceControl;

	/*Bellow we create the device name and the symbolic link as unicode strings to be passed to the creation of the device*/
	UNICODE_STRING device_name = devicename;
	UNICODE_STRING symlink = symlinkname;

	/*Bellow we create an empty device object*/
	PDEVICE_OBJECT device_object = nullptr;

	/*Bellow we set the returning status of DriverEntry function to always be SUCCESS. We change it to other state when needed more down the road*/
	NTSTATUS status = STATUS_SUCCESS;

	/*Bellow we initialize the status of the simbolic link to false, becouse is not yet linked*/
	BOOLEAN symlink_status = false;

	/*Bellow we gona create the device, set the wanted method of passing the information and initialize the symbolic link.*/
	do {
		/* https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice */
		/*Bellow we create the device that will allow us to comunicate with the driver 
		* Params:
		* 1- our driver name
		* 2- 0 becouse we dont need extra bytes
		* 3- the device name
		* 4- device type
		* 5- characteristic flags
		* 6- False becouse we not work exclusively
		* 7- the result pointer to the device */
		status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, 0, FALSE, &device_object);

		/*Bellow we check if we successfully created the device*/
		if (!NT_SUCCESS(status)) {
			/*KdPrint only work with debug build*/
			/*Bellow we print a mensage that we fail to create a device with status*/
			KdPrint((DEBUG_TO_PRINT "failed to create device (0x%08X)\n", status));
			/*Bellow we break out of the loop becouse we cant do anything else*/
			break;
		}

		/* https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/using-direct-i-o */
		/*Bellow we flag the device to work with buffered io method instead of using the direct or using the neither.*/
		device_object->Flags |= DO_BUFFERED_IO;

		/* https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatesymboliclink */
		/*Bellow we create the symbolic link*/
		status = IoCreateSymbolicLink(&symlink, &device_name);

		/*Bellow we check if we actually created the symbolic link*/
		if (!NT_SUCCESS(status)) {
			/*KdPrint only work with debug build*/
			/*Bellow we print a mensage that we fail to create the symbolic link with status*/
			KdPrint((DEBUG_TO_PRINT "failed to create symbolic link (0x%08X)\n", status));

			/*Bellow we break out of the loop becouse we cant proceed*/
			break;
		}

		/*Bellow we set the symbolic link status to true becouse we just created the symbolic link*/
		symlink_status = true;

		/* https://docs.microsoft.com/en-us/windows-hardware/drivers/ifs/clearing-the-do-device-initializing-flag */
		/*Bellow we clear te device object. Not needed on the driver entry, but better to get used to it*/
		device_object->Flags &= ~DO_DEVICE_INITIALIZING;
	} while (false);

	/*Bellow we check if the status is SUCCESS, if its not and we already did some of the work, we clear it here
	in the reverse order that we created before*/
	if (!NT_SUCCESS(status)) {

		/*Bellow we check if we created the symbolic link*/
		if (symlink_status) {
			/*If we actually created the symbolic link we delete it on next line*/
			IoDeleteSymbolicLink(&symlink);
		}

		/*Bellow we check if we created the device object*/
		if (device_object) {
			/*If we actually created the device object we delete it on next line*/
			IoDeleteDevice(device_object);
		}
	}

	/*Bellow we return the status of this operation*/
	return status;
}