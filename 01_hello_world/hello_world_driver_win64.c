#include <wdm.h>

_Use_decl_annotations_
VOID MyUnload(struct _DRIVER_OBJECT *DriverObject)
{
    (void)DriverObject;
    // Function body
    DbgPrint("[HelloWorld] Unloaded\n");
}

_Use_decl_annotations_
NTSTATUS
DriverEntry(struct _DRIVER_OBJECT *DriverObject, PUNICODE_STRING RegistryPath)
{
    (void)RegistryPath;
    // Function body
    DriverObject->DriverUnload = MyUnload;
    DbgPrint("[HelloWorld] Loaded\n");
    return STATUS_SUCCESS;
}
