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

typedef struct SICODevice* SICODevice;
typedef struct SICOKernel* SICOKernel;
typedef void* SICOHandle;
typedef void* SICOCommanQueue;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum SICOState
{
    SICO_GeneralFail,
    SICO_Ok,
    SICO_NoDevice,
    SICO_UnableToBuildKernel,
    SICO_UnableToExecuteKernel,
} SICOState;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum SICOMemoryPolicy
{
    SICO_AutoAllocate,
    SICO_UserSuppliedData,

} SICOMemoryPolicy;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SICO_MEM_READ_WRITE CL_MEM_READ_WRITE
#define SICO_MEM_WRITE_ONLY CL_MEM_WRITE_ONLY
#define SICO_MEM_READ_ONLY CL_MEM_READ_ONLY
#define SICO_PARAMETER (1 << 20)    // not a real memory type

#define SICO_SIZEOF_ARRAY(array) (int)(sizeof(array) / sizeof(array[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SICOParam
{
    uintptr_t data;
    unsigned int type;   // memory type (use defines above)
    SICOMemoryPolicy policy;
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

SICODevice* scGetAllDevices(int* count);

/*
 * Get the number of CPU devices we have in the machine.
 * \@param count = returns back how many CPU devices there are
 * Return device(s) pointer
 */

SICODevice* scGetCPUDevices(int* count);

/*
 * Get the number of GPU devices we have in the machine.
 * \@param count = returns back how many GPU devices there are
 * Return device(s) pointer
 */

SICODevice* scGetGPUDevices(int* count);

/*
 * Get the number of other OpenCL devices we have in the machine.
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer
 */

SICODevice* scGetOtherDevices(int* count);

/*
 * Get the "best" devices in the system, It will check number of compute units
 * on each device and return the best one in the category
 * Meaning if the GPU(s) is the best device it will return all of them
 * \@param count = returns back how many other devices there are.
 * Return device(s) pointer
 */

SICODevice* scGetBestDevices(int* count);

/*
 * Get the "best" device in the system, It will check number of compute units
 * on each device and return the best one in the category
 * Return device(s) pointer
 */

SICODevice scGetBestDevice();

/*
 *
 */

int scCompileFromFile(struct SICODevice* device, const char* filename, const char* buildOpts);

/*
 * Allocate memory from a device. The memory is uninitialized so user is responsible for filling this memory
 * \@param device Device to allocate the memory from
 * \@param flags cl_mem_flags (see OpenCL docs for a list of all values)
 * \@param size Number of bytes to allocate
 * \@param hostPtr (used with CL_MEM_USE_HOST_PTR, etc flags)
 * Return handle to memory, otherwise 0
 */

SICOHandle scAlloc(struct SICODevice* device, int flags, size_t size, void* hostPtr);

/*
 * Frees memory alloced using scAlloc
 * \@param handle Handle to memory to free
 * Return true if memory freed correct, otherwise false
 */

bool scFree(SICOHandle handle);



SICOCommanQueue scCreateCommandQueue(struct SICODevice* device);


/*

   SICOHandle scAllocSyncCopy(struct SICODevice* device, const void* memory, int size);


   SICOHandle scAsycCopyToDevice(SICOHandle handle, const void* memory, int size);


   SICOHandle scAsycCopyFromDevice(void* dest, const SICOHandle handle, int size);

 */


/*
 * TODO Document
 */

int scSetupParameters(SICODevice device, SICOKernel kernel, SICOCommanQueue queue, SICOParam* params, int paramCount);

/*
 * TODO Document
 *
 */

SICOState scAddKernel1D(SICOCommanQueue queue, SICOKernel kernel, size_t count);

/*
 * TODO Document
 *
 */

SICOState scAddKernel2D(SICOCommanQueue queue, SICODevice device, SICOKernel kernel, size_t sizeX, size_t sizeY, SICOParam* params, int paramCount);

/*
 * TODO Document
 *
 */

SICOState scAddKernel(SICOCommanQueue queue, SICOKernel kernel, int workDim,
                      const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize,
                      int eventListCount, void* waitEventList, void* event);


/*
 * TODO Document
 *
 */

SICOState scFlushMemory(SICOCommanQueue* queue, SICOKernel kernel, size_t sizeX, size_t sizeY);

/*
 * TODO Document
 *
 */

void scFreeParams(SICOParam* paramaters, int count);

/*
 * TODO Document
 */

SICOState scWriteMemoryParams(struct SICODevice* device, SICOCommanQueue queue, SICOParam* params, uint32_t paramCount);

/*
 * TODO Document
 */


SICOState scCommandQueueFlush(SICOCommanQueue queue);

/*
 * scDestroyCommandQueue
 */

SICOState scDestroyCommandQueue(SICOCommanQueue queue);


/*
 * TODO Document
 */

struct SICOKernel* scCompileKernelFromSourceFile(struct SICODevice* device, const char* filename, const char* kernelName, const char* buildOpts);

/*
 * TODO Document
 */

SICOState scRunKernel1DArraySimple(void* dest, void* sourceA, void* sourceB, const char* filename, size_t elementCount, size_t sizeInBytes);

#ifdef __cplusplus
}
#endif

#endif

