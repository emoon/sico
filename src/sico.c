
#include "sico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__clang__) || defined(__gcc__)
static void sico_log_internal(const char* format, ...) __attribute__((format(printf, 1, 2)));
#else
static void sico_log_internal(const char* format, ...);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_log_internal(const char* format, ...)
{
    va_list ap;

    va_start(ap, format);
#if defined(_WIN32)
    {
        char buffer[2048];
        vsprintf(buffer, format, ap);
        OutputDebugStringA(buffer);
    }
#else
    vprintf(format, ap);
#endif
    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define sico_log(format, ...) sico_log_internal("%s(%d) : %s " format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static cl_platform_id s_platformId = 0;
static struct SICODevice** s_devices = 0;
static int s_deviceCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SICODevice
{
    cl_device_id deviceId;
    cl_device_type deviceType;
    cl_context context;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SICOKernel
{
    cl_program program;
    cl_kernel kern;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
struct SICOCommanQueue
{
	int dumm

};
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mallocZero(size_t size)
{
    void* t = malloc(size);
    assert(t);
    memset(t, 0, size);
    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getErrorString(cl_int errorCode)
{
    switch (errorCode)
    {
        case CL_DEVICE_NOT_FOUND:
            return "CL_INVALID_PLATFORM";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES : There is a failure to allocate resources required by the OpenCL implementation on the device.";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY : There is a failure to allocate resources required by the OpenCL implementation on the host.";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case CL_INVALID_VALUE:
            return "CL_INVALID_VALUE : Values specified in properties are not valid.";
        case CL_INVALID_DEVICE_TYPE:
            return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:
            return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE : Device is not a valid device or is not associated with context.";
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT : Context is not a valid context.";
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES : Values specified in properties are valid but are not supported by the device.";
        case CL_INVALID_COMMAND_QUEUE:
            return "CL_INVALID_COMMAND_QUEUE : Command queue is not a valid host command-queue.";
        case CL_INVALID_HOST_PTR:
            return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:
            return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:
            return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:
            return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:
            return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:
            return "CL_INVALID_PROGRAM_EXECUTABLE : There is no successfully built program executable available for device associated with command_queue.";
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:
            return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:
            return "CL_INVALID_KERNEL : Kernel is not a valid kernel object.";
        case CL_INVALID_ARG_INDEX:
            return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:
            return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:
            return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:
            return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:
            return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:
            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:
            return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:
            return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:
            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:
            return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:
            return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:
            return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:
            return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:
            return "CL_INVALID_GLOBAL_WORK_SIZE";
    }

    return "UNKNOWN_ERROR_CODE";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* readFileFromDisk(const char* file, size_t* size)
{
    size_t fileSize;
    void* data;
    FILE* f = fopen(file, "rb");

    if (!f)
    {
        sico_log("Unable to open file %s\n", file);
        return 0;
    }

    fseek(f, 0, SEEK_END);
    fileSize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    // pad the size a bit so we make sure to have the data null terminated
    data = mallocZero(fileSize + 16);

    if ((fread((void*)data, 1, fileSize, f)) != fileSize)
    {
        free(data);
        fclose(f);
        sico_log("SICO: Unable to read the whole file %s to memory\n", file);
    }

    *size = fileSize;

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printOrAppendString(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, size_t* len)
{
    char tempBuffer[2048];

    size_t valueSize;
    char* value;

    clGetDeviceInfo(dev, param, 0, 0, &valueSize);
    value = (char*)malloc(valueSize);
    clGetDeviceInfo(dev, param, valueSize, value, 0);

    if (output && (*len > 0))
    {
        size_t tempLen;

        sprintf(tempBuffer, fmt, id, index, value);
        tempLen = strlen(tempBuffer);

        if (tempLen < *len)
        {
            strcat(output, tempBuffer);
            *len -= tempLen;
        }
    }
    else
    {
        printf(fmt, id, index, value);
    }

    free(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printOrAppendInt(char* output, cl_device_id dev, cl_device_info param, int id, int index, const char* fmt, size_t* len)
{
    char tempBuffer[2048];
    size_t value;

    clGetDeviceInfo(dev, param, sizeof(value), &value, 0);

    if (output && (*len > 0))
    {
        size_t tempLen;

        sprintf(tempBuffer, fmt, id, index, value);
        tempLen = strlen(tempBuffer);

        if (tempLen < *len)
        {
            strcat(output, tempBuffer);
            *len -= tempLen;
        }
    }
    else
    {
        printf(fmt, id, index, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cl_context createSingleContext(cl_device_id deviceId)
{
    cl_context context;
    cl_int err;

    if ((context = clCreateContext(0, 1, &deviceId, NULL, NULL, &err)))
        return context;

    // TODO: Include more detailed error messages

    sico_log("Unable to create context. Error %s\n", getErrorString(err));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICODevice* scGetAllDevices(int* count)
{
    cl_uint i, j, deviceIter = 0;
    cl_uint platformCount;
    cl_uint deviceCount;
    cl_platform_id* platforms;
    cl_device_id* devices;
    int totalDeviceCount = 0;

    if (s_platformId == 0)
        return 0;

    // Check if we have already fetched all devices then we just return them here

    if (s_deviceCount > 0)
    {
        *count = s_deviceCount;
        return s_devices;
    }

    clGetPlatformIDs(0, 0, &platformCount);
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, 0);

    for (i = 0; i < platformCount; ++i)
    {
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);
        totalDeviceCount += deviceCount;
    }

    // Allocate space for the devices and then set them up

    s_devices = malloc((sizeof(SICODevice*) * (size_t)totalDeviceCount));

    // setup the device ids

    for (i = 0; i < platformCount; ++i)
    {
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);
        devices = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, 0);

        for (j = 0; j < deviceCount; ++j, ++deviceIter)
        {
            s_devices[deviceIter] = mallocZero(sizeof(SICODevice));
            s_devices[deviceIter]->deviceId = devices[j];
            s_devices[deviceIter]->context = createSingleContext(devices[j]);
            clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &s_devices[deviceIter]->deviceType, 0);
        }

        free(devices);
    }

    free(platforms);

    *count = totalDeviceCount;
    s_deviceCount = totalDeviceCount;

    return s_devices;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int scSetupParameters(SICODevice device, SICOKernel kernel, SICOCommanQueue queue, SICOParam* params, int paramCount)
{
    uint8_t needsUpload[256] = { 0 };
    cl_int error;
    cl_mem mem;

    if (!device)
        return SICO_NoDevice;

    if (!kernel || !queue || !params || params == 0)
        return SICO_GeneralFail;

    if (!device->context)
        return SICO_GeneralFail;

    assert(paramCount < (int)sizeof(needsUpload));

    // Create memory objects

    for (int i = 0; i < paramCount; ++i)
    {
        SICOParam* param = &params[i];

        if (param->type == SICO_PARAMETER || param->policy == SICO_UserSuppliedData)
        {
            param->privData = 0;
            continue;
        }

        if (device->deviceType == CL_DEVICE_TYPE_CPU)
        {
            if (!(mem = clCreateBuffer(device->context, param->type | CL_MEM_USE_HOST_PTR, param->size, (void*)param->data, &error)))
            {
                sico_log("CPU clCreateBuffer failed (param %d), error %s\n", i, getErrorString(error));
                return SICO_GeneralFail;
            }
        }
        else
        {
            if (!(mem = clCreateBuffer(device->context, CL_MEM_READ_WRITE, param->size, NULL, &error)))
            {
                sico_log("GPU clCreateBuffer failed (param %d), error %s\n", i, getErrorString(error));
                return SICO_GeneralFail;
            }

            needsUpload[i] = 1;
        }

        param->privData = (void*)mem;
    }

    // Setup the memory objects that needs to be transfered

    for (int i = 0; i < paramCount; ++i)
    {
        SICOParam* param = &params[i];

        if (needsUpload[i] == 0)
            continue;

        if ((error = clEnqueueWriteBuffer((cl_command_queue)queue, (cl_mem)param->privData, CL_TRUE, 0, param->size, (void*)param->data, 0, NULL, NULL)) != CL_SUCCESS)
        {
            sico_log("clEnqueueWriteBuffer failed (param %d), error %s\n", i, getErrorString(error));
            return SICO_GeneralFail;
        }
    }

    // Setup kernel parameters

    for (int i = 0; i < paramCount; ++i)
    {
        SICOParam* param = &params[i];

        if (param->type == SICO_PARAMETER)
            error = clSetKernelArg(kernel->kern, (cl_uint)i, param->size, (void*)param->data);
        else
            error = clSetKernelArg(kernel->kern, (cl_uint)i, sizeof(cl_mem), (cl_mem) & param->privData);

        if (error != CL_SUCCESS)
        {
            sico_log("Unable to clSetKernelArg (param %d), error %s\n", i, getErrorString(error));
            return SICO_GeneralFail;
        }
    }

    return SICO_Ok;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scWriteMemoryParams(SICODevice device, SICOCommanQueue queue, SICOParam* params, uint32_t paramCount)
{
    uint32_t i;
    cl_int error;

    // If the device is CPU we have nothing that needs to be copied back

    if (device->deviceType == CL_DEVICE_TYPE_CPU)
        return SICO_Ok;

    for (i = 0; i < paramCount; ++i)
    {
        SICOParam* param = &params[i];

        if (param->type == SICO_MEM_READ_ONLY || param->type == SICO_PARAMETER)
            continue;

        if ((error = clEnqueueReadBuffer((cl_command_queue)queue, (cl_mem)param->privData, CL_TRUE, 0, param->size, (void*)param->data, 0, NULL, NULL)) != CL_SUCCESS)
        {
            sico_log("clEnqueueReadBuffer failed (param %d), error %s\n", i, getErrorString(error));
            return SICO_GeneralFail;
        }
    }

    return SICO_Ok;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int scInitialize()
{
    if (clGetPlatformIDs(1, &s_platformId, 0) != CL_SUCCESS)
    {
        s_platformId = 0;
        return 0;
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scClose()
{
    for (int i = 0; i < s_deviceCount; ++i)
    {
        SICODevice device = s_devices[i];

        if (!device->context)
            continue;

        int error = clReleaseContext(device->context);

        if (error != CL_SUCCESS)
            sico_log("%s ", getErrorString(error));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int scListDevices(char* output, size_t size)
{
    int i, count;
    SICODevice* devices;

    if (s_platformId == 0)
        return 0;

    if (!(devices = scGetAllDevices(&count)))
        return 0;

    for (i = 0; i < count; ++i)
    {
        cl_device_id id = 0;

        if (!devices[i])
            continue;

        id = devices[i]->deviceId;

        printOrAppendString(output, id, CL_DEVICE_NAME, i + 1, 0, "%d.%d Device: %s\n", &size);
        printOrAppendString(output, id, CL_DEVICE_VERSION, i + 1, 1, " %d.%d Hardware version: %s\n", &size);
        printOrAppendString(output, id, CL_DEVICE_VERSION, i + 1, 2, " %d.%d Software version: %s\n", &size);
        printOrAppendString(output, id, CL_DEVICE_OPENCL_C_VERSION, i + 1, 3, " %d.%d OpenCL C version: %s\n", &size);
        printOrAppendInt(output, id, CL_DEVICE_MAX_COMPUTE_UNITS, i + 1, 4, " %d.%d Parallel compute units: %d\n", &size);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SICODevice* scGetBestDevice()
{
    int i, count;
    SICODevice* devices;

    // this algorithm can be quite improved. Right now it will just pick the first GPU

    if (!(devices = scGetAllDevices(&count)))
    {
        sico_log("%s", "Unable to find any OpenCL devices in the system\n");
        return 0;
    }

    for (i = 0; i < count; ++i)
    {
        if (!devices[i])
            continue;

        if (devices[i]->deviceType == CL_DEVICE_TYPE_GPU)
        {
            if (!createSingleContext(devices[i]->deviceId))
                return 0;

            return devices[i];
        }
    }

    // if no found at this spot we just use the first one

    if (!createSingleContext(devices[0]->deviceId))
        return 0;

    return devices[0];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SICOKernel* scCompileKernelFromSourceFile(struct SICODevice* device, const char* filename, const char* kernelName, const char* buildOpts)
{
    const char* data;
    size_t fileSize;
    cl_kernel kern;
    cl_int error;

    cl_program program;
    SICOKernel kernel;

    (void)buildOpts;

    // First create a single context if we have none

    if (!device->context)
    {
        if (!(device->context = createSingleContext(device->deviceId)))
            return 0;
    }

    if (!(data = readFileFromDisk(filename, &fileSize)))
        return 0;

    if (!(program = clCreateProgramWithSource(device->context, 1, &data, &fileSize, &error)))
    {
        free((void*)data);
        sico_log("clCreateProgramWithSource failed, error: %s\n", getErrorString(error));
        return 0;
    }

    free((void*)data);

    if ((error = clBuildProgram(program, 0, 0, 0, 0, 0)) != CL_SUCCESS)
    {
        char* errorBuffer;
        size_t size;

        clGetProgramBuildInfo(program, device->deviceId, CL_PROGRAM_BUILD_LOG, 0, 0, &size);
        errorBuffer = malloc(size + 1);
        clGetProgramBuildInfo(program, device->deviceId, CL_PROGRAM_BUILD_LOG, size, errorBuffer, 0);
        errorBuffer[size] = 0;

        // TODO: Support writing the error log to a buffer

        sico_log("unable to build %s (error %s)\n\n%s\n", filename, getErrorString(error), errorBuffer);
        free(errorBuffer);

        return 0;
    }

    if (!(kern = clCreateKernel(program, kernelName, &error)))
    {
        sico_log("Unable to create kernel for %s (%s), error %s\n", filename, kernelName, getErrorString(error));
        return 0;
    }

    kernel = mallocZero(sizeof(SICOKernel));
    kernel->program = program;
    kernel->kern = kern;

    return kernel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Use this code in the function above

int scCompileFromFile(struct SICODevice* device, const char* filename, const char* buildOpts)
{
    const char* data;
    size_t fileSize;
    cl_int error;
    cl_program program;

    (void)buildOpts;

    // First create a single context if we have none

    if (!device->context)
    {
        if (!(device->context = createSingleContext(device->deviceId)))
            return 0;
    }

    if (!(data = readFileFromDisk(filename, &fileSize)))
        return 0;

    if (!(program = clCreateProgramWithSource(device->context, 1, &data, &fileSize, &error)))
    {
        free((void*)data);
        sico_log("clCreateProgramWithSource failed, error: %s\n", getErrorString(error));
        return 0;
    }

    free((void*)data);

    if ((error = clBuildProgram(program, 0, 0, 0, 0, 0)) != CL_SUCCESS)
    {
        char* errorBuffer;
        size_t size;

        clGetProgramBuildInfo(program, device->deviceId, CL_PROGRAM_BUILD_LOG, 0, 0, &size);
        errorBuffer = malloc(size + 1);
        clGetProgramBuildInfo(program, device->deviceId, CL_PROGRAM_BUILD_LOG, size, errorBuffer, 0);
        errorBuffer[size] = 0;

        // TODO: Support writing the error log to a buffer

        sico_log("unable to build %s (error %s)\n\n%s\n", filename, getErrorString(error), errorBuffer);
        free(errorBuffer);

        return 0;
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOHandle scAlloc(struct SICODevice* device, int flags, size_t size, void* hostPtr)
{
    cl_int errorCode;

    cl_mem mem = clCreateBuffer(device->context, (cl_mem_flags)flags, size, hostPtr, &errorCode);

    if (errorCode != CL_SUCCESS)
    {
        sico_log("%s\n", getErrorString(errorCode));
        mem = 0;
    }

    return (SICOHandle)mem;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool scFree(SICOHandle handle)
{
    int errorCode = clReleaseMemObject((cl_mem)handle);

    if (errorCode == CL_SUCCESS)
        return true;

    sico_log("%s\n", getErrorString(errorCode));

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOCommanQueue scCreateCommandQueue(struct SICODevice* device)
{
    cl_int error;
    cl_command_queue queue;

    queue = clCreateCommandQueue(device->context, device->deviceId, 0, &error);

    if (error == CL_SUCCESS)
        return (SICOCommanQueue)queue;

    sico_log("%s", getErrorString(error));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scAddKernel(SICOCommanQueue queue, SICOKernel kernel, int workDim,
                      const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize,
                      int eventListCount, void* waitEventList, void* event)
{
    cl_int error = clEnqueueNDRangeKernel((cl_command_queue)queue, kernel->kern,
                                          (unsigned int)workDim, globalWorkOffset, globalWorkSize, localWorkSize,
                                          (cl_uint)eventListCount, waitEventList, event);

    if (error == CL_SUCCESS)
        return SICO_Ok;

    switch (error)
    {
        case CL_INVALID_PROGRAM_EXECUTABLE:
            sico_log("%s ", "CL_INVALID_PROGRAM_EXECUTABLE if there is no successfully built program executable available for device associated with command_queue.\n"); break;
        case CL_INVALID_COMMAND_QUEUE:
            sico_log("%s ", " CL_INVALID_COMMAND_QUEUE : if command_queue is not a valid host command-queue.\n"); break;
        case CL_INVALID_KERNEL:
            sico_log("%s ", " CL_INVALID_KERNEL : if kernel is not a valid kernel object.\n"); break;
        case CL_INVALID_CONTEXT:
            sico_log("%s ", " CL_INVALID_CONTEXT : if context associated with command_queue and kernel is not the same or if the context associated with command_queue and events in event_wait_list are not the same.\n"); break;
        case CL_INVALID_KERNEL_ARGS:
            sico_log("%s ", " CL_INVALID_KERNEL_ARGS : if the kernel argument values have not been specified or if a kernel argument declared to be a pointer to a type does not point to a named address space.\n"); break;
        case CL_INVALID_WORK_DIMENSION:
            sico_log("%s ", " CL_INVALID_WORK_DIMENSION : if work_dim is not a valid value (i.e. a value between 1 and 3).\n"); break;
        case CL_INVALID_GLOBAL_WORK_SIZE:
            sico_log("%s ", " CL_INVALID_GLOBAL_WORK_SIZE : if global_work_size is NULL, or if any of the values specified in global_work_size[0], ...global_work_size [work_dim - 1] are 0 or exceed the range given by the sizeof(size_t) for the device on which the kernel execution will be enqueued.\n"); break;
        case CL_INVALID_GLOBAL_OFFSET:
            sico_log("%s ", " CL_INVALID_GLOBAL_OFFSET : if the value specified in global_work_size + the corresponding values in global_work_offset for any dimensions is greater than the sizeof(size_t) for the device on which the kernel execution will be enqueued.\n"); break;
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            sico_log("%s ", " CL_MISALIGNED_SUB_BUFFER_OFFSET : if a sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for device associated with queue.\n"); break;
        case CL_INVALID_IMAGE_SIZE:
            sico_log("%s ", " CL_INVALID_IMAGE_SIZE : if an image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with queue.\n"); break;
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            sico_log("%s ", " CL_IMAGE_FORMAT_NOT_SUPPORTED : if an image object is specified as an argument value and the image format (image channel order and data type) is not supported by device associated with queue.\n"); break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            sico_log("%s ", " CL_MEM_OBJECT_ALLOCATION_FAILURE : if there is a failure to allocate memory for data store associated with image or buffer objects specified as arguments to kernel.\n"); break;
        case CL_INVALID_EVENT_WAIT_LIST:
            sico_log("%s ", " CL_INVALID_EVENT_WAIT_LIST : if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.\n"); break;
        case CL_INVALID_OPERATION:
            sico_log("%s ", " CL_INVALID_OPERATION : if SVM pointers are passed as arguments to a kernel and the device does not support SVM or if system pointers are passed as arguments to a kernel and/or stored inside SVM allocations passed as kernel arguments and the device does not support fine grain system SVM allocations.\n"); break;
        case CL_OUT_OF_HOST_MEMORY:
            sico_log("%s ", " CL_OUT_OF_HOST_MEMORY : if there is a failure to allocate resources required by the OpenCL implementation on the host.\n"); break;
        case CL_OUT_OF_RESOURCES:
        {
            sico_log("%s ", " CL_OUT_OF_RESOURCES : if there is a failure to queue the execution instance of kernel on the command-queue because of insufficient resources needed to execute the kernel. For example, the explicitly specified local_work_size causes a failure to execute the kernel because of insufficient resources such as registers or local memory. Another example would be the number of read-only image args used in kernel exceed the CL_DEVICE_MAX_READ_IMAGE_ARGS value for device or the number of write-only image args used in kernel exceed the CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS value for device or the number of samplers used in kernel exceed CL_DEVICE_MAX_SAMPLERS for device.\n");
            sico_log("%s ", " CL_OUT_OF_RESOURCES : if there is a failure to allocate resources required by the OpenCL implementation on the device.\n");
            break;
        }
        case CL_INVALID_WORK_ITEM_SIZE:
        {
            sico_log("%s ", " CL_INVALID_WORK_ITEM_SIZE : if the number of work-items specified in any of local_work_size[0], ... local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], .... CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim - 1].\n");
            sico_log("%s ", " CL_INVALID_WORK_ITEM_SIZE : if the number of work-items specified in any of local_work_size[0], ... local_work_size[work_dim – 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], ... CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim – 1].\n");
            break;
        }
        case CL_INVALID_WORK_GROUP_SIZE:
        {
            sico_log("%s ", " CL_INVALID_WORK_GROUP_SIZE : if local_work_size is specified and does not match the work-group size for kernel in the program source given by the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier.\n"); break;
            sico_log("%s ", " CL_INVALID_WORK_GROUP_SIZE : if local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] * … local_work_size[work_dim – 1] is greater than the value specified by CL_DEVICE_MAX_WORK_GROUP_SIZE in the table of OpenCL Device Queries for clGetDeviceInfo.\n");
            sico_log("%s ", " CL_INVALID_WORK_GROUP_SIZE : if local_work_size is NULL and the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier is used to declare the work-group size for kernel in the program source.\n");
            sico_log("%s ", " CL_INVALID_WORK_GROUP_SIZE : if the program was compiled with –cl-uniform-work-group-size and the number of work-items specified by global_work_size is not evenly divisible by size of work-group given by local_work_size.\n");
            break;
        }
    }

    return SICO_GeneralFail;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scAddKernel1D(SICOCommanQueue queue, SICOKernel kernel, size_t count)
{
    return scAddKernel(queue, kernel, 1, 0, &count, 0, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scAddKernel2D(SICOCommanQueue queue, SICODevice device, SICOKernel kernel, size_t sizeX, size_t sizeY, SICOParam* params, int paramCount)
{
    size_t sizes[] = { sizeX, sizeY };

	if (scSetupParameters(device, kernel, queue, params, paramCount) != SICO_Ok)
		return SICO_GeneralFail;

    if (scAddKernel(queue, kernel, 2, 0, (size_t*)&sizes, 0, 0, 0, 0) != SICO_Ok)
    	return SICO_GeneralFail;

    return scWriteMemoryParams(device, queue, params, SICO_SIZEOF_ARRAY(params));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*

   SICOHandle scAllocSyncCopy(struct SICODevice* device, const void* memory, int size)
   {

   }


   SICOHandle scAsycCopyToDevice(SICOHandle handle, const void* memory, int size);


   SICOHandle scAsycCopyFromDevice(void* dest, const SICOHandle handle, int size);

 */


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scFreeParams(SICOParam* params, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (params[i].privData)
            clReleaseMemObject(params[i].privData);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scCommandQueueFinish(SICOCommanQueue queue)
{
    cl_int errorCode = clFinish(queue);

    if (errorCode == CL_SUCCESS)
        return SICO_Ok;

    sico_log("%s ", getErrorString(errorCode));

    return SICO_GeneralFail;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scDestroyCommandQueue(SICOCommanQueue queue)
{
    cl_int errorCode = clReleaseCommandQueue(queue);

    if (errorCode == CL_SUCCESS)
        return SICO_Ok;

    sico_log("%s ", getErrorString(errorCode));

    return SICO_GeneralFail;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICOState scRunKernel1DArraySimple(void* dest, void* sourceA, void* sourceB, const char* filename, size_t elementCount, size_t sizeInBytes)
{
    SICODevice device;
    SICOKernel kernel;
    SICOCommanQueue queue;

    SICOParam params[] =
    {
        { (uintptr_t)dest, SICO_MEM_READ_WRITE, SICO_AutoAllocate, sizeInBytes, 0 },
        { (uintptr_t)sourceA, SICO_MEM_READ_ONLY, SICO_AutoAllocate, sizeInBytes, 0 },
        { (uintptr_t)sourceB, SICO_MEM_READ_ONLY, SICO_AutoAllocate, sizeInBytes, 0 },
    };

    scInitialize();

    if (!(device = scGetBestDevice()))
    {
        sico_log("%s ", "Unable to find OpenCL device\n");
        return SICO_NoDevice;
    }

    if (!(kernel = scCompileKernelFromSourceFile(device, filename, "kern", "")))
        return SICO_UnableToBuildKernel;

    if (!(queue = scCreateCommandQueue(device)))
        return SICO_GeneralFail;

    if ((scSetupParameters(device, kernel, queue, params, SICO_SIZEOF_ARRAY(params))) != SICO_Ok)
        return SICO_GeneralFail;

    if ((scAddKernel1D(queue, kernel, elementCount)) != SICO_Ok)
        return SICO_GeneralFail;

    if ((scWriteMemoryParams(device, queue, params, SICO_SIZEOF_ARRAY(params))) != SICO_Ok)
        return SICO_GeneralFail;

    scCommandQueueFinish(queue);

    scFreeParams(params, SICO_SIZEOF_ARRAY(params));

    scDestroyCommandQueue(queue);

    return SICO_Ok;
}
