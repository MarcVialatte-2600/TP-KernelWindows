#ifndef __PROCTRACE_ROOTKIT_DRIVER_WIN64_H
#define __PROCTRACE_ROOTKIT_DRIVER_WIN64_H

// Attention: utilisable en UserLand après un include de windows.h
// Attention: utilisable en KernelLand après un include de wdm.h

#define IOCTL_ROOTKIT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif