// x64 Native Tools Command Prompt for VS 2022 > cl.exe user_echo.c /I .

#include <windows.h>
#include <stdio.h>
#include <echo_driver_win64.h>

int main (char argc, char ** argv) {
    HANDLE device = INVALID_HANDLE_VALUE;
    BOOL status = FALSE;
    DWORD bytesReturned = 0;
    CHAR inBuffer[128] = {0};
    CHAR outBuffer[128] = {0};

    if (argc != 2) {
        printf ("Need a parameter\n");
        return 2600;
    }

    RtlCopyMemory (inBuffer, argv[1], strlen (argv[1]));

    device = CreateFileW (L"\\\\.\\EchoDeviceLink", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

    if (device == INVALID_HANDLE_VALUE) {
        printf ("> Could not open device: 0x%x\n", GetLastError ());
        return 2600;
    }

    printf ("> Issuing IOCTL_ECHO: 0x%x\n", IOCTL_ECHO);
    status = DeviceIoControl (device, IOCTL_ECHO, inBuffer, sizeof (inBuffer), outBuffer, sizeof (outBuffer), &bytesReturned, (LPOVERLAPPED) NULL);
    printf ("> IOCTL_ECHO 0x%x issued\n", IOCTL_ECHO);
    printf ("> Received from the KernelLand: %s. Received buffer size: %d\n", outBuffer, bytesReturned);
    CloseHandle (device);
}