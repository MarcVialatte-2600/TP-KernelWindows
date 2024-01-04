#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

void PcreateProcessNotifyRoutine (HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create) {
    if (Create) {
        DbgPrint ("[ProcTrace] Create process %d form %d", ProcessId, ParentId);
        PEPROCESS proc;
        PsLookupProcessByProcessId (ProcessId, &proc);
        DbgPrint ("[ProcTrace] PEPROCESS %p\n", proc);
        ObDereferenceObject (proc);
    }
    else {
        DbgPrint ("[ProcTrace] Delete process %d form %d", ProcessId, ParentId);
    }
}

void PcreateProcessNotifyRoutineEx (PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
    (void) Process;
    (void) CreateInfo;

    if (CreateInfo) {
        DbgPrint ("[ProcTrace] Create process %d\n", ProcessId);
    }
    else {
        DbgPrint ("[ProcTrace] Delete process %d\n", ProcessId);
    }
}

void PloadImageNotifyRoutine (PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo) {
    DbgPrint ("[ProcTrace] Load %wZ from %d - at %p of %d bytes", FullImageName, ProcessId, ImageInfo->ImageBase, ImageInfo->ImageSize);
}

void PcreateThreadNotifyRoutine(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create) {
    if (Create) {
        DbgPrint ("[ProcTrace] Create thread %d form %d\n", ThreadId, ProcessId);
    }
    else {
        DbgPrint ("[ProcTrace] Delete thread %d form %d\n", ThreadId, ProcessId);
    }
}

_Use_decl_annotations_
VOID MyUnload (struct _DRIVER_OBJECT* DriverObject) {
    (void) DriverObject;
    // Function Body
    DbgPrint ("[ProcTrace] Unloaded\n");
    // Unregister routines
    PsSetCreateProcessNotifyRoutine (PcreateProcessNotifyRoutine, TRUE);
    PsSetCreateProcessNotifyRoutineEx (PcreateProcessNotifyRoutineEx, TRUE);

    PsRemoveLoadImageNotifyRoutine (PloadImageNotifyRoutine);
    PsRemoveCreateThreadNotifyRoutine (PcreateThreadNotifyRoutine);
}

_Use_decl_annotations_
NTSTATUS
DriverEntry (struct _DRIVER_OBJECT *DriverObject, PUNICODE_STRING RegistryPath) {
    DriverObject->DriverUnload = MyUnload;
    // Fucntion body
    DbgPrint ("[ProcTrace] Loaded: %wZ", RegistryPath);

    // Register routines
    PsSetCreateProcessNotifyRoutine (PcreateProcessNotifyRoutine, FALSE);
    PsSetCreateProcessNotifyRoutineEx (PcreateProcessNotifyRoutineEx, FALSE);

    PsSetLoadImageNotifyRoutine (PloadImageNotifyRoutine);
    PsSetCreateThreadNotifyRoutine (PcreateThreadNotifyRoutine);

    return STATUS_SUCCESS;
}