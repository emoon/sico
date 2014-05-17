#ifndef _OPENCL_WRAPPER_H_
#define _OPENCL_WRAPPER_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OCLWDevice;
typedef uintptr_t OCLWHandle;
struct OCLWKernel;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum OpenCLWState
{
    OpenCLW_Ok,
    OpenCLW_NoDevice,
    OpenCLW_UnableToFindKernel,
    OpenCLW_UnableToExecuteKernel,
    OpenCLW_CompileFailed,
} OpenCLWState;

/*
 * Needs to be called before any other function is called. This function won't do that much except make sure that there
 * are some OpenCL capable device on your machine.
 * Return non-zero on success otherwise 0
 */

int wclInitialize();

/*
 * List the devices in the system. This function can optionally write the text to an output buffer instead if
 * suplied. If the parameter is NULL or the size is 0 printf will be used instead
 */

int wclListDevices(char* output, size_t length);

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer 
 */

struct OCLWDevice** wclGetAllDevices(int* count);

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer 
 */

struct OCLWDevice** wclGetCPUDevices(int* count);

/*
 * Get the number of GPU devices we have in the machine.
 * \@param count = returns back how many GPU devices there are
 * Return device(s) pointer 
 */

struct OCLWDevice** wclGetGPUDevices(int* count);

/*
 * Get the number of other OpenCL devices we have in the machine.
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

struct OCLWDevice** wclGetOtherDevices(int* count);

/*
 * Get the "best" devices in the system, It will check number of compute units 
 * on each device and return the best one in the category
 * Meaning if the GPU(s) is the best device it will return all of them
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

struct OCLWDevice** wclGetBestDevices(int* count);

/*
 *
 */

struct OCLWKernel* wclGetKernelFromFile(struct OCLWDevice* device, const char* filename);

/*
 *
 */

OCLWHandle wclAlloc(struct OCLWDevice* device, size_t size);

/*
 *
 */

OCLWHandle wclFree(OCLWHandle handle);

/*
 *
 */

OCLWHandle wclAllocSyncCopy(struct OCLWDevice* device, const void* memory, int size);

/*
 *
 */

OCLWHandle wclAsycCopyToDevice(OCLWHandle handle, const void* memory, int size);

/*
 *
 */

OCLWHandle wclAsycCopyFromDevice(void* dest, const OCLWHandle handle, int size);

/*
 *
 *
 */

OpenCLWState wclRunKernel1DArray(void* dest, void* source, const char* filename, int elementCount, int sizeInBytes);

#ifdef __cplusplus
}
#endif

#endif

