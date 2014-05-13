#ifndef _OPENCL_WRAPPER_H_
#define _OPENCL_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct OCLWDevice;
typedef uintptr_t OCLWHandle;
struct OCLWKernel;

/*
 * Needs to be called before any other function is called. This function won't do that much except make sure that there
 * are some OpenCL capable device on your machine.
 * Return non-zero on success otherwise 0
 */

int wclInitialize();

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer 
 */

OCLWDevice** wclGetCPUDevices(int* count);

/*
 * Get the number of GPU devices we have in the machine.
 * \@param count = returns back how many GPU devices there are
 * Return device(s) pointer 
 */

OCLWDevice** wclGetGPUDevices(int* count);

/*
 * Get the number of other OpenCL devices we have in the machine.
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

OCLWDevice** wclGetOtherDevices(int* count);

/*
 * Get the "best" devices in the system, It will check number of compute units 
 * on each device and return the best one in the category
 * Meaning if the GPU(s) is the best device it will return all of them
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

OCLWDevice* wclGetBestDevices(int* count);

/*
 *
 */

OCLWKernel* wclGetKernelFromFile(OCLWDevice* device, const char* filename);

/*
 *
 */

OCLWHandle wclAlloc(OCLDevice* device, int size);

/*
 *
 */

OCLWHandle wclFree(OCLWHandle handle);

/*
 *
 */

OCLWHandle wclAllocSyncCopy(OCLDevice* device, const void* memory, int size);

/*
 *
 */

OCLWHandle wclAsycCopyToDevice(OCLWHandle handle, const void* memory, int size);

/*
 *
 */

OCLWHandle wclAsycCopyFromDevice(void* dest, const OCLWHandle handle, int size);

#ifdef __cplusplus
}
#endif

#endif

