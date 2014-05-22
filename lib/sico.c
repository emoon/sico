
#include "sico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static cl_platform_id s_platformId = 0;
static struct SICODevice** s_devices = 0;
static int s_deviceCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SICODevice
{
	cl_device_id deviceId;
	cl_device_type deviceType;
	cl_context context;
} SICODevice;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SICOKernel
{
	cl_program program;
	cl_kernel kern;

} SICOKernel;

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
		case CL_DEVICE_NOT_FOUND : return "CL_INVALID_PLATFORM"; 
		case CL_DEVICE_NOT_AVAILABLE : return "CL_DEVICE_NOT_AVAILABLE"; 
		case CL_COMPILER_NOT_AVAILABLE : return "CL_COMPILER_NOT_AVAILABLE"; 
		case CL_MEM_OBJECT_ALLOCATION_FAILURE : return "CL_MEM_OBJECT_ALLOCATION_FAILURE"; 
		case CL_OUT_OF_RESOURCES : return "CL_OUT_OF_RESOURCES"; 
		case CL_OUT_OF_HOST_MEMORY : return "CL_OUT_OF_HOST_MEMORY"; 
		case CL_PROFILING_INFO_NOT_AVAILABLE : return "CL_PROFILING_INFO_NOT_AVAILABLE"; 
		case CL_MEM_COPY_OVERLAP : return "CL_MEM_COPY_OVERLAP"; 
		case CL_IMAGE_FORMAT_MISMATCH : return "CL_IMAGE_FORMAT_MISMATCH"; 
		case CL_IMAGE_FORMAT_NOT_SUPPORTED : return "CL_IMAGE_FORMAT_NOT_SUPPORTED"; 
		case CL_BUILD_PROGRAM_FAILURE : return "CL_BUILD_PROGRAM_FAILURE"; 
		case CL_MAP_FAILURE : return "CL_MAP_FAILURE"; 
		case CL_MISALIGNED_SUB_BUFFER_OFFSET : return "CL_MISALIGNED_SUB_BUFFER_OFFSET"; 
		case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST : return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"; 
		case CL_INVALID_VALUE : return "CL_INVALID_VALUE"; 
		case CL_INVALID_DEVICE_TYPE : return "CL_INVALID_DEVICE_TYPE"; 
		case CL_INVALID_PLATFORM : return "CL_INVALID_PLATFORM"; 
		case CL_INVALID_DEVICE : return "CL_INVALID_DEVICE"; 
		case CL_INVALID_CONTEXT : return "CL_INVALID_CONTEXT"; 
		case CL_INVALID_QUEUE_PROPERTIES : return "CL_INVALID_QUEUE_PROPERTIES"; 
		case CL_INVALID_COMMAND_QUEUE : return "CL_INVALID_COMMAND_QUEUE"; 
		case CL_INVALID_HOST_PTR : return "CL_INVALID_HOST_PTR"; 
		case CL_INVALID_MEM_OBJECT : return "CL_INVALID_MEM_OBJECT"; 
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR : return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"; 
		case CL_INVALID_IMAGE_SIZE : return "CL_INVALID_IMAGE_SIZE"; 
		case CL_INVALID_SAMPLER : return "CL_INVALID_SAMPLER"; 
		case CL_INVALID_BINARY : return "CL_INVALID_BINARY"; 
		case CL_INVALID_BUILD_OPTIONS : return "CL_INVALID_BUILD_OPTIONS"; 
		case CL_INVALID_PROGRAM : return "CL_INVALID_PROGRAM"; 
		case CL_INVALID_PROGRAM_EXECUTABLE : return "CL_INVALID_PROGRAM_EXECUTABLE"; 
		case CL_INVALID_KERNEL_NAME : return "CL_INVALID_KERNEL_NAME"; 
		case CL_INVALID_KERNEL_DEFINITION : return "CL_INVALID_KERNEL_DEFINITION"; 
		case CL_INVALID_KERNEL : return "CL_INVALID_KERNEL"; 
		case CL_INVALID_ARG_INDEX : return "CL_INVALID_ARG_INDEX"; 
		case CL_INVALID_ARG_VALUE : return "CL_INVALID_ARG_VALUE"; 
		case CL_INVALID_ARG_SIZE : return "CL_INVALID_ARG_SIZE"; 
		case CL_INVALID_KERNEL_ARGS : return "CL_INVALID_KERNEL_ARGS"; 
		case CL_INVALID_WORK_DIMENSION : return "CL_INVALID_WORK_DIMENSION"; 
		case CL_INVALID_WORK_GROUP_SIZE : return "CL_INVALID_WORK_GROUP_SIZE"; 
		case CL_INVALID_WORK_ITEM_SIZE : return "CL_INVALID_WORK_ITEM_SIZE"; 
		case CL_INVALID_GLOBAL_OFFSET : return "CL_INVALID_GLOBAL_OFFSET"; 
		case CL_INVALID_EVENT_WAIT_LIST : return "CL_INVALID_EVENT_WAIT_LIST"; 
		case CL_INVALID_EVENT : return "CL_INVALID_EVENT"; 
		case CL_INVALID_OPERATION : return "CL_INVALID_OPERATION"; 
		case CL_INVALID_GL_OBJECT : return "CL_INVALID_GL_OBJECT"; 
		case CL_INVALID_BUFFER_SIZE : return "CL_INVALID_BUFFER_SIZE"; 
		case CL_INVALID_MIP_LEVEL : return "CL_INVALID_MIP_LEVEL"; 
		case CL_INVALID_GLOBAL_WORK_SIZE : return "CL_INVALID_GLOBAL_WORK_SIZE"; 
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
		printf("CLW: Unable to open file %s\n", file);
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
		printf("CLW: Unable to read the whole file %s to memory\n", file);

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
	value = (char*) malloc(valueSize);
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

	printf("CLW: Unable to create context. Error %s\n", getErrorString(err));

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SICODevice** scGetAllDevices(int* count)
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
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
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
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, 0);

        for (j = 0; j < deviceCount; ++j, ++deviceIter) 
        {
			s_devices[deviceIter] = mallocZero(sizeof(SICODevice));
			s_devices[deviceIter]->deviceId = devices[j];
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

static int setupParameters(SICODevice* device, SICOKernel* kernel, cl_command_queue queue, SICOParam* params, uint32_t paramCount)
{
	uint8_t needsUpload[256] = { 0 };
	cl_int error;
	cl_mem mem;
	uint32_t i;

	assert(paramCount < sizeof(needsUpload));

	// Create memory objects 

	for (i = 0; i < paramCount; ++i)
	{
		SICOParam* param = &params[i];

		if (param->type == SICO_PARAMETER)
		{
			param->privData = 0;
			continue;
		}

		if (device->deviceType == CL_DEVICE_TYPE_CPU)
		{
			if (!(mem = clCreateBuffer(device->context, param->type | CL_MEM_USE_HOST_PTR, param->size, param->data, &error)))
			{
				printf("CLW: CPU clCreateBuffer failed (param %d), error %s\n", i, getErrorString(error));
				return SCIO_GeneralFail;
			}
		}
		else
		{
			if (!(mem = clCreateBuffer(device->context, CL_MEM_READ_WRITE, param->size, NULL, &error)))
			{
				printf("CLW: GPU clCreateBuffer failed (param %d), error %s\n", i, getErrorString(error));
				return SCIO_GeneralFail;
			}

			needsUpload[i] = 1;
		}

		param->privData = (void*)mem;
	}

	// Setup the memory objects that needs to be transfered 

	for (i = 0; i < paramCount; ++i)
	{
		SICOParam* param = &params[i]; 

		if (needsUpload[i] == 0)
			continue;

    	if ((error = clEnqueueWriteBuffer(queue, (cl_mem)param->privData, CL_TRUE, 0, param->size, param->data, 0, NULL, NULL)) != CL_SUCCESS)
    	{
			printf("CLW: clEnqueueWriteBuffer failed (param %d), error %s\n", i, getErrorString(error));
			return SCIO_GeneralFail;
    	}
	}

	// Setup kernel parameters

 	for (i = 0; i < paramCount; ++i)
	{
		SICOParam* param = &params[i]; 

		if (param->type == SICO_PARAMETER)
			error = clSetKernelArg(kernel->kern, (cl_uint)i, param->size, param->data);
		else
			error = clSetKernelArg(kernel->kern, (cl_uint)i, sizeof(cl_mem), (cl_mem)&param->privData);
		
		if (error != CL_SUCCESS)
		{
			printf("Unable to clSetKernelArg (param %d), error %s\n", i, getErrorString(error));
			return SCIO_GeneralFail;
		}
	}

	return SCIO_Ok;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int writeMemoryParams(SICODevice* device, cl_command_queue queue, SICOParam* params, uint32_t paramCount)
{
	uint32_t i;
	cl_int error;

	// If the device is CPU we have nothing that needs to be copied back

	if (device->deviceType == CL_DEVICE_TYPE_CPU)
		return SCIO_Ok;

	for (i = 0; i < paramCount; ++i)
	{
		SICOParam* param = &params[i]; 

		if (param->type == SICO_MEM_READ_ONLY || param->type == SICO_PARAMETER)
			continue;

    	if ((error = clEnqueueReadBuffer(queue, (cl_mem)param->privData, CL_TRUE, 0, param->size, param->data, 0, NULL, NULL)) != CL_SUCCESS)
    	{
			printf("CLW: clEnqueueReadBuffer failed (param %d), error %s\n", i, getErrorString(error));
			return SCIO_GeneralFail;
    	}
	}

	return SCIO_Ok;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int scInitialize()
{
	if (clGetPlatformIDs(1, &s_platformId , 0) != CL_SUCCESS)
	{
		s_platformId = 0;
		return 0;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int scListDevices(char* output, size_t size)
{
	int i, count;
	SICODevice** devices;

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
	SICODevice** devices;

	// this algorithm can be quite improved. Right now it will just pick the first GPU

	if (!(devices = scGetAllDevices(&count)))
	{
		printf("CLW: Unable to find any OpenCL devices in the system\n");
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

struct SICOKernel* scCompileKernelFromSourceFile(
	struct SICODevice* device, 
	const char* filename, 
	const char* kernelName, 
	const char* buildOpts)
{
	const char* data;
	size_t fileSize;
	cl_kernel kern;
	cl_int error;

	cl_program program;
	SICOKernel* kernel;

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
		printf("ERROR: clCreateProgramWithSource failed, error: %s\n", getErrorString(error));
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

		printf("CLW: unable to build %s (error %s)\n\n%s\n", filename, getErrorString(error), errorBuffer);
		free(errorBuffer);
		
		return 0;
	}

    if (!(kern = clCreateKernel(program, kernelName, &error)))
    {
    	printf("CLW: Unable to create kernel for %s (%s), error %s\n", filename, kernelName, getErrorString(error)); 
    	return 0;
    }

	kernel = mallocZero(sizeof(SICOKernel));
	kernel->program = program;
	kernel->kern = kern;

	return kernel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SCIOState scRunKernel1DArray(void* dest, void* sourceA, void* sourceB, const char* filename, size_t elementCount, size_t sizeInBytes)
{
	SICODevice* device;
	SICOKernel* kernel;
	uint32_t i;
	cl_int error;
	cl_command_queue queue;

	SICOParam params[] =
	{
		{ dest, SICO_MEM_READ_WRITE, sizeInBytes, 0 },
		{ sourceA, SICO_MEM_READ_ONLY, sizeInBytes, 0 },
		{ sourceB, SICO_MEM_READ_ONLY, sizeInBytes, 0 },
	};

	scInitialize();

	if (!(device = scGetBestDevice()))
	{
    	printf("CLW: Unable to find OpenCL device\n");
		return SCIO_NoDevice;
	}

	if (!(kernel = scCompileKernelFromSourceFile(device, filename, "kern", "")))
		return SCIO_UnableToBuildKernel;

	// TODO: Abstract the queue

    if (!(queue = clCreateCommandQueue(device->context, device->deviceId, 0, &error)))
    {
    	printf("CLW: Unable to create command queue, error: %s\n", getErrorString(error));
    	return SCIO_GeneralFail;
    }

	if ((setupParameters(device, kernel, queue, params, oclw_sizeof_array(params))) != SCIO_Ok)
    	return SCIO_GeneralFail;

    if ((error = clEnqueueNDRangeKernel(queue, kernel->kern, 1, 0, &elementCount, 0, 0, 0, 0)) != CL_SUCCESS)
	{
		printf("CLW: clEnqueueNDRangeKernel failed, error %s\n", getErrorString(error));
    	return SCIO_GeneralFail;
	}

    clFinish(queue);

	if ((writeMemoryParams(device, queue, params, oclw_sizeof_array(params))) != SCIO_Ok)
    	return SCIO_GeneralFail;

    clFinish(queue);

	for (i = 0; i < oclw_sizeof_array(params); ++i)
	{	
		if (params[i].privData)
    		clReleaseMemObject(params[i].privData);
   	}

    clReleaseCommandQueue(queue);

   	return SCIO_Ok;
}
