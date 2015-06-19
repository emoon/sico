#ifndef _OPENCL_WRAPPER_H_
#define _OPENCL_WRAPPER_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __APPLE__ 
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct SICODevice;
typedef uintptr_t SICOHandle;
struct SICOKernel;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum SCIOState
{
    SCIO_Ok,
    SCIO_GeneralFail,
    SCIO_NoDevice,
    SCIO_UnableToBuildKernel,
    SCIO_UnableToExecuteKernel,
} SCIOState;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SICO_MEM_READ_WRITE CL_MEM_READ_WRITE
#define SICO_MEM_WRITE_ONLY CL_MEM_WRITE_ONLY
#define SICO_MEM_READ_ONLY CL_MEM_READ_ONLY
#define SICO_PARAMETER (1 << 20)	// not a real memory type

#define SICO_SIZEOF_ARRAY(array) (int)(sizeof(array) / sizeof(array[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SICOParam
{
	void* data;
	unsigned int type;   // memory type (use defines above) 
	size_t size;
	void* privData; // private data
} SICOParam;

/*
 * Needs to be called before any other function is called. This function won't do that much except make sure that there
 * are some OpenCL capable device on your machine.
 * Return non-zero on success otherwise 0
 */

int scInitialize();

/*
 * Closes the devices etc 
 */

void scClose();

/*
 * List the devices in the system. This function can optionally write the text to an output buffer instead if
 * suplied. If the parameter is NULL or the size is 0 printf will be used instead
 */

int scListDevices(char* output, size_t length);

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer 
 */

struct SICODevice** scGetAllDevices(int* count);

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer 
 */

struct SICODevice** scGetCPUDevices(int* count);

/*
 * Get the number of GPU devices we have in the machine.
 * \@param count = returns back how many GPU devices there are
 * Return device(s) pointer 
 */

struct SICODevice** scGetGPUDevices(int* count);

/*
 * Get the number of other OpenCL devices we have in the machine.
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

struct SICODevice** scGetOtherDevices(int* count);

/*
 * Get the "best" devices in the system, It will check number of compute units 
 * on each device and return the best one in the category
 * Meaning if the GPU(s) is the best device it will return all of them
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer 
 */

struct SICODevice** scGetBestDevices(int* count);

/*
 * Get the "best" device in the system, It will check number of compute units 
 * on each device and return the best one in the category
 * Return device(s) pointer 
 */

struct SICODevice* scGetBestDevice();

/*
 *
 */

int scCompileFromFile(struct SICODevice* device, const char* filename, const char* buildOpts);

/*

SICOHandle scAlloc(struct SICODevice* device, size_t size);


SICOHandle scFree(SICOHandle handle);


SICOHandle scAllocSyncCopy(struct SICODevice* device, const void* memory, int size);


SICOHandle scAsycCopyToDevice(SICOHandle handle, const void* memory, int size);


SICOHandle scAsycCopyFromDevice(void* dest, const SICOHandle handle, int size);

*/


SCIOState scRunKernel1DArray(void* dest, void* sourceA, void* sourceB, const char* filename, size_t elementCount, size_t sizeInBytes);

#ifdef __cplusplus
}
#endif

#endif

