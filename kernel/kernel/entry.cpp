#include "defs.h"
#include "ioctl.h"
#include "utils.h"
#include "pte/pte_protect.h"

PEPROCESS AttachedProcess;
DWORD AttachedPid;

NTSTATUS manage_memory(_memory* copy) 
{
	NTSTATUS status = STATUS_SUCCESS;
	if (NT_SUCCESS(status)) 
	{
		PEPROCESS sourceProcess, targetProcess;
		PVOID sourcePtr, targetPtr;
		if (copy->is_write == FALSE) 
		{
			sourceProcess = AttachedProcess;
			targetProcess = PsGetCurrentProcess();
			sourcePtr = (PVOID)copy->dst;
			targetPtr = (PVOID)copy->src;
		}
		else {
			sourceProcess = PsGetCurrentProcess();
			targetProcess = AttachedProcess;
			sourcePtr = (PVOID)copy->src;
			targetPtr = (PVOID)copy->dst;
		}
		SIZE_T bytes;
		if (copy->dst < 0x7FFFFFFFFFFF)
		{
			if (copy->is_forcewrite)
			{
				PMDL mdl = IoAllocateMdl(targetPtr, (ULONG)copy->size, FALSE, FALSE, NULL);

				MmProbeAndLockProcessPages(mdl, targetProcess, KernelMode, IoReadAccess);
				void* map = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
				MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);

				RtlCopyMemory(map, sourcePtr, copy->size);

				MmUnmapLockedPages(map, mdl);
				MmUnlockPages(mdl);
				IoFreeMdl(mdl);
			}
			else
				status = MmCopyVirtualMemory(sourceProcess, sourcePtr, targetProcess, targetPtr, copy->size, KernelMode, &bytes);

		}
	}
	return status;
}

NTSTATUS ctl_io(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp) 
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
	PVOID ioBuffer = Irp->AssociatedIrp.SystemBuffer;

	if (irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL) 
	{
		ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
		if (ioControlCode == IOCTL_DRIVER_INIT)
		{
			_initalize* ReadInput = (_initalize*)Irp->AssociatedIrp.SystemBuffer;
			PsLookupProcessByProcessId((HANDLE)ReadInput->pid, &AttachedProcess);
			AttachedPid = ReadInput->pid;

			Irp->IoStatus.Status = STATUS_SUCCESS;
		}

		if (ioControlCode == IOCTL_DRIVER_MANAGE_MEMORY)
		{
			Irp->IoStatus.Status = manage_memory((_memory*)ioBuffer);
			Irp->IoStatus.Information = sizeof(_memory);
		}

		if (ioControlCode == IOCTL_DRIVER_GET_BASE_ADDRESS)
		{
			_baseaddress* ReadInput = (_baseaddress*)Irp->AssociatedIrp.SystemBuffer;
			*ReadInput->out = (ULONGLONG)PsGetProcessSectionBaseAddress(AttachedProcess);

			Irp->IoStatus.Status = STATUS_SUCCESS;
		}

		if (ioControlCode == IOCTL_DRIVER_PROTECT)
		{
			_protect data = *(_protect*)Irp->AssociatedIrp.SystemBuffer;

			KAPC_STATE apc;
			KeStackAttachProcess(AttachedProcess, &apc);

			DWORD old;
			Irp->IoStatus.Status = ZwProtectVirtualMemory(ZwCurrentProcess(), (PVOID*)&data.addr, &data.size, data.protect, &old);

			KeUnstackDetachProcess(&apc);
		}
	}

	NTSTATUS status = Irp->IoStatus.Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS real_main(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) 
{
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING dev_name, sym_link;
	PDEVICE_OBJECT dev_obj;

	RtlInitUnicodeString(&dev_name, DRIVER_DEVICE_NAME); //die lit
	auto status = IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);
	if (status != STATUS_SUCCESS) return status;

	RtlInitUnicodeString(&sym_link, DRIVER_DOS_DEVICE_NAME);
	status = IoCreateSymbolicLink(&sym_link, &dev_name);
	if (status != STATUS_SUCCESS) return status;

	SetFlag(dev_obj->Flags, DO_BUFFERED_IO); //set DO_BUFFERED_IO bit to 1

	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++) //set all MajorFunction's to unsupported
		driver_obj->MajorFunction[t] = unsupported_io;

	//then set supported functions to appropriate handlers
	driver_obj->MajorFunction[IRP_MJ_CREATE] = create_io; //link our io create function
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = close_io; //link our io close function
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ctl_io; //link our control code handler
	driver_obj->DriverUnload = NULL; //add later

	ClearFlag(dev_obj->Flags, DO_DEVICE_INITIALIZING); //set DO_DEVICE_INITIALIZING bit to 0 (we are done initializing)
	return status;
}

NTSTATUS entry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path)
{
	UNREFERENCED_PARAMETER(driver_obj);
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING  drv_name;
	RtlInitUnicodeString(&drv_name, DRIVER_NAME);
	return IoCreateDriver(&drv_name, &real_main); //so it's kdmapper-able
}