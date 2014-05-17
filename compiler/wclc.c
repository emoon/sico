///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This code is mostly taken from https://github.com/Celtoys/oclpc but somewhat adopted to fit the wrapper:
// * Changed the coding style a bit
// * Fixes to work on Mac
// * Warning fixes

// Licence:
//
// Copyright 2014 Celtoys Ltd
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <CLW.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#ifdef __APPLE__ 
#include <OpenCL/opencl.h>
#else
#include <malloc.h>
#include <CL/opencl.h>
#endif

// stdout configuration
static bool s_PrintHeader = true;
static bool s_PrintHelp = false;
static bool s_Verbose = false;

// Selection of platform/device by index from the command-line
static int32_t s_PlatformIndex = -1;
static int32_t s_DeviceIndex = -1;

// Selection of platform/device by substring name match
static char s_PlatformNameSubstr[128] = { 0 };
static char s_DeviceNameSubstr[128] = { 0 };
static char s_InputFilename[1024] = { 0 };
static char s_outputFilename[1024] = { 0 };
static char s_BuildArgs[8192] = { 0 };

#define LOG if (s_Verbose) printf

#ifdef _WIN32
#define strcasecmp strcmpi
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char* lowerCaseString(char* str)
{
	int i;

	for(i = 0; str[i]; i++)
		str[i] = (char)tolower(str[i]);

	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* parseArguments(int argc, const char* argv[])
{
	int i;

	for (i = 1; i < argc; i++)
	{
		const char* arg = argv[i];

		// Is this an option?
		if (arg[0] == '-')
		{
			if (!strcasecmp(arg, "-h"))
			{
				s_PrintHelp = true;
			}

			else if (!strcasecmp(arg, "-noheader"))
			{
				s_PrintHeader = false;
			}

			else if (!strcasecmp(arg, "-verbose"))
			{
				s_Verbose = true;
			}

			else if (!strcasecmp(arg, "-platform_index") && i < argc - 1)
			{
				s_PlatformIndex = atoi(argv[i + 1]);
				i++;
			}

			else if (!strcasecmp(arg, "-device_index") && i < argc - 1)
			{
				s_DeviceIndex = atoi(argv[i + 1]);
				i++;
			}

			else if (!strcasecmp(arg, "-o") && i < argc - 1)
			{
				strcpy(s_outputFilename, argv[i + 1]);
				i++;
			}

			else if (!strcasecmp(arg, "-platform_substr") && i < argc - 1)
			{
				strncpy(s_PlatformNameSubstr, argv[i + 1], sizeof(s_PlatformNameSubstr) - 1);
				lowerCaseString(s_PlatformNameSubstr);
				i++;
			}

			else if (!strcasecmp(arg, "-device_substr") && i < argc - 1)
			{
				strncpy(s_DeviceNameSubstr, argv[i + 1], sizeof(s_DeviceNameSubstr) - 1);
				lowerCaseString(s_DeviceNameSubstr);
				i++;
			}

			else
			{
				// Add any options that this tool doesn't realise to the build arguments
				strncat(s_BuildArgs, " ", sizeof(s_BuildArgs) - 1);
				strncat(s_BuildArgs, arg, sizeof(s_BuildArgs) - 1);
				if (i < argc - 1 && argv[i + 1][0] != '-')
				{
					strncat(s_BuildArgs, " ", sizeof(s_BuildArgs) - 1);
					strncat(s_BuildArgs, argv[i + 1], sizeof(s_BuildArgs) - 1);
					i++;
				}
			}
		}

		else
		{
			// Must be a filename
			strncpy(s_InputFilename, arg, sizeof(s_InputFilename) - 1);
		}
	}

	if (s_InputFilename[0] == 0)
		return "No input filename specified";

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printHeader()
{
	printf("wclc OpenCL Precompiler Copyright 2014 Celtoys Ltd\n");
	printf("Licensed under the Apache License, Version 2.0 \n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printUsage()
{
	printf("Usage: oclpc [options] filename\n");

	if (s_PrintHelp)
	{
		printf("\nOptions are:\n\n");
		printf("   -noheader          Supress header\n");
		printf("   -o          		  Output which writes a dummy file for build systems to track\n");
		printf("   -verbose           Print logs detailing what wclc is doing behind the scenes\n");
		printf("   -platform_index    Specify zero-based platform index tp select\n");
		printf("   -device_index      Specify zero-based device index tp select\n");
		printf("   -platform_substr   Specify substring to match when selecting platform by name\n");
		printf("   -device_substr     Specify substring to match when selecting device by name\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	// List of available platforms
	cl_uint nb_platforms;
	cl_platform_id* platform_ids;

	// Selected platform ID
	cl_platform_id platform_id;

	// List of available devices for the selected platform
	cl_uint nb_devices;
	cl_device_id* device_ids;

	// Selected device ID
	cl_device_id device_id;

	cl_context context;
} OpenCL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* getPlatformIDs(OpenCL* ocl)
{
	cl_int error;

	assert(ocl != NULL);

	// Count number of platforms
	error = clGetPlatformIDs(0, 0, &ocl->nb_platforms);
	if (error != CL_SUCCESS)
		return "clGetPlatformIDs query failed";
	if (ocl->nb_platforms == 0)
		return "No OpenCL platforms found";

	LOG("Number of platforms found: %d\n", ocl->nb_platforms);

	// Get the list of platform IDs
	ocl->platform_ids = malloc(ocl->nb_platforms * sizeof(cl_platform_id));
	if (ocl->platform_ids == NULL)
		return "malloc platform array failed";

	error = clGetPlatformIDs(ocl->nb_platforms, ocl->platform_ids, NULL);
	if (error != CL_SUCCESS)
		return "clGetPlatformIDS call failed";

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getPlatformString(cl_platform_id id, cl_platform_info info, bool to_lower)
{
	static char platform_string[256];
	cl_int error = clGetPlatformInfo(id, info, sizeof(platform_string) - 1, platform_string, NULL);
	if (error != CL_SUCCESS)
		return NULL;

	// Lower-case for substring matching
	if (to_lower)
		lowerCaseString(platform_string);

	return platform_string;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* selectPlatform(OpenCL* ocl)
{
	const char* platform_name;

	assert(ocl != NULL);
	assert(ocl->nb_platforms > 0);
	assert(ocl->platform_ids != NULL);

	// Select platform by index provided on command-line first
	if (s_PlatformIndex != -1)
	{
		if (s_PlatformIndex < 0 || s_PlatformIndex >= (int)ocl->nb_platforms)
			return "Out of range platform index provided on command-line";
		ocl->platform_id = ocl->platform_ids[s_PlatformIndex];
		LOG("Platform %d selected by index from command-line\n", s_PlatformIndex);
	}

	else if (s_PlatformNameSubstr[0] != 0)
	{
		// Linear search through platform list for platform with a name matching the input substring
		uint32_t i;
		for (i = 0; i < ocl->nb_platforms; i++)
		{
			platform_name = getPlatformString(ocl->platform_ids[i], CL_PLATFORM_NAME, true);
			if (platform_name == NULL)
				return "GetPlatformString call with CL_PLATFORM_NAME failed";

			if (strstr(platform_name, (const char*)s_PlatformNameSubstr) != NULL)
			{
				ocl->platform_id = ocl->platform_ids[i];
				LOG("Platform %d selected by name matching '%s' from command-line\n", i, s_PlatformNameSubstr);
				break;
			}
		}
	}

	else
	{
		ocl->platform_id = ocl->platform_ids[0];
		LOG("Using platform 0 as no command-line overrides were specified\n");
	}

	// Display the name
	platform_name = getPlatformString(ocl->platform_id, CL_PLATFORM_NAME, false);
	if (platform_name == NULL)
		return "GetPlatformString call with CL_PLATFORM_NAME failed";
	LOG("Platform '%s' selected with ID %d\n", platform_name, (int)ocl->platform_id);

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getDeviceIDs(OpenCL* ocl)
{
	cl_int error;

	assert(ocl != NULL);
	assert(ocl->platform_id != 0);

	// Count number of devices
	error = clGetDeviceIDs(ocl->platform_id, CL_DEVICE_TYPE_ALL, 0, 0, &ocl->nb_devices);
	if (error != CL_SUCCESS)
		return "clGetDeviceIDs query failed";
	if (ocl->nb_devices == 0)
		return "No OpenCL devices found";
	LOG("Number of devices found: %d\n", ocl->nb_devices);

	// Get the list of device IDs
	ocl->device_ids = malloc(ocl->nb_devices * sizeof(cl_device_id));
	if (ocl->device_ids == NULL)
		return "malloc device array failed";
	error = clGetDeviceIDs(ocl->platform_id, CL_DEVICE_TYPE_ALL, ocl->nb_devices, ocl->device_ids, NULL);
	if (error != CL_SUCCESS)
		return "clGetDeviceIDs call failed";

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getDeviceString(cl_device_id id, cl_device_info info, bool to_lower)
{
	static char device_string[256];
	cl_int error = clGetDeviceInfo(id, info, sizeof(device_string) - 1, device_string, NULL);
	if (error != CL_SUCCESS)
		return NULL;

	// Lower-case for substring matching
	if (to_lower)
		lowerCaseString(device_string);

	return device_string;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* selectDevice(OpenCL* ocl)
{
	const char* device_name;

	assert(ocl != NULL);
	assert(ocl->nb_devices > 0);
	assert(ocl->device_ids != NULL);

	// Select device by index provided on command-line first
	if (s_DeviceIndex != -1)
	{
		if (s_DeviceIndex < 0 || s_DeviceIndex >= (int)ocl->nb_devices)
			return "Out of range device index provided on command-line";
		ocl->device_id = ocl->device_ids[s_DeviceIndex];
		LOG("Device %d selected by index from command-line\n", s_DeviceIndex);
	}

	else if (s_DeviceNameSubstr[0] != 0)
	{
		// Linear search through device list for device with a name matching the input substring
		uint32_t i;
		for (i = 0; i < ocl->nb_devices; i++)
		{
			device_name = getDeviceString(ocl->device_ids[i], CL_DEVICE_NAME, true);
			if (device_name == NULL)
				return "GetDeviceString call with CL_DEVICE_NAME failed";

			if (strstr(device_name, (const char*)s_DeviceNameSubstr) != NULL)
			{
				ocl->device_id = ocl->device_ids[i];
				LOG("Device %d selected by name matching '%s' from command-line\n", i, s_DeviceNameSubstr);
				break;
			}
		}
	}

	else
	{
		ocl->device_id = ocl->device_ids[0];
		LOG("Using device 0 as no command-line overrides were specified\n");
	}

	// Display the name
	device_name = getDeviceString(ocl->device_id, CL_DEVICE_NAME, false);
	if (device_name == NULL)
		return "GetDeviceString call with CL_DEVICE_NAME failed";
	LOG("Device '%s' selected with ID %d\n", device_name, (int)ocl->device_id);

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* createContext(OpenCL* ocl)
{
	cl_int error;
	cl_context_properties props[3];

	assert(ocl != NULL);
	assert(ocl->platform_id != 0);
	assert(ocl->device_id != 0);

	// Specify context platform
	props[0] = CL_CONTEXT_PLATFORM;
	props[1] = (cl_context_properties)ocl->platform_id;
	props[2] = 0;

	// Create the context
	// TODO: Register a callback handler
	ocl->context = clCreateContext(props, 1, &ocl->device_id, 0, 0, &error);
	if (error != CL_SUCCESS)
		return "clCreateContext call failed";

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy(OpenCL** ocl)
{
	assert(ocl != NULL);
	assert(*ocl != NULL);

	if ((*ocl)->context != NULL)
	{
		clReleaseContext((*ocl)->context);
		(*ocl)->context = NULL;
	}

	if ((*ocl)->device_ids != NULL)
	{
		free((*ocl)->device_ids);
		(*ocl)->device_ids = NULL;
	}

	if ((*ocl)->platform_ids != NULL)
	{
		free((*ocl)->platform_ids);
		(*ocl)->platform_ids = NULL;
	}

	free(*ocl);
	*ocl = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* create(OpenCL** ocl)
{
	const char* error_string;

	assert(ocl != NULL);

	// Allocate space for the object
	*ocl = malloc(sizeof(OpenCL));
	if (*ocl == NULL)
		return "malloc failed for sizeof(OpenCL)";

	memset(*ocl, 0, sizeof(OpenCL));

	// Select a platform
	if ((error_string = getPlatformIDs(*ocl)))
		goto error;

	if ((error_string = selectPlatform(*ocl)))
		goto error;

	// Select a device
	if ((error_string = getDeviceIDs(*ocl)))
		goto error;

	if ((error_string = selectDevice(*ocl)))
		goto error;

	if ((error_string = createContext(*ocl)))
		goto error;

	return NULL;

error:

	destroy(ocl);
	return error_string;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int loadAndCompileProgram(OpenCL* ocl)
{
	FILE* fp;
	uint32_t program_size;
	size_t prog_size;
	const char* program_data;

	cl_int error;
	cl_program program;

	size_t los_size;
	uint8_t* los_data;

	cl_build_status build_status;

	assert(ocl != NULL);

	// Load the program from disk
	LOG("Opening file %s\n", s_InputFilename);
	fp = fopen(s_InputFilename, "rb");
	if (fp == NULL)
	{
		printf("ERROR: Couldn't open input file %s\n", s_InputFilename);
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	program_size = (uint32_t)ftell(fp);
	LOG("File size is %d bytes\n", program_size);
	fseek(fp, 0, SEEK_SET);
	program_data = malloc(program_size);
	if (program_data == NULL)
	{
		printf("ERROR: Failed to allocate memory for program data\n");
		fclose(fp);
		return 1;
	}
	if (fread((void*)program_data, 1, program_size, fp) != program_size)
	{
		printf("ERROR: Couldn't read program data from disk\n");
		free((void*)program_data);
		fclose(fp);
		return 1;
	}
	fclose(fp);

	prog_size = (size_t)program_size;

	// Create the program from source and build it
	LOG("Creating and building the program\n");
	program = clCreateProgramWithSource(ocl->context, 1, &program_data, &prog_size, &error);
	if (error != CL_SUCCESS)
	{
		free((void*)program_data);
		printf("ERROR: clCreateProgramWithSource call failed\n");
		return 1;
	}
	error = clBuildProgram(program, 1, &ocl->device_id, s_BuildArgs, 0, 0);
	free((void*)program_data);

	// Allocate enough space for the build log and retrieve it
	clGetProgramBuildInfo(program, ocl->device_id, CL_PROGRAM_BUILD_LOG, 0, 0, &los_size);
	LOG("Log size is %d bytes\n", (int)los_size);
	los_data = malloc(los_size);
	if (los_data == NULL)
	{
		printf("ERROR: Failed to allocate log data of size %d\n", (int)los_size);
		return 1;
	}
	clGetProgramBuildInfo(program, ocl->device_id, CL_PROGRAM_BUILD_LOG, los_size, los_data, 0);

	// Query for build success and write an errors/warnings
	error = clGetProgramBuildInfo(program, ocl->device_id, CL_PROGRAM_BUILD_STATUS, sizeof(build_status), &build_status, 0);
	LOG("Build status error code: %d\n", error);
	LOG("Build status code: %d\n", build_status);
	printf("%s", los_data);
	free(los_data);
	return error != CL_SUCCESS || build_status != CL_BUILD_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void addIncludePathForFile(const char* filename)
{
	char* fptr;

	// Add the entire filename as an include path
	strncat(s_BuildArgs, " -I \"", sizeof(s_BuildArgs) - 1);
	strncat(s_BuildArgs, filename, sizeof(s_BuildArgs) - 1);

	// Point to the end of the command-line string and scan back, looking for the first path separator
	fptr = s_BuildArgs + strlen(s_BuildArgs) - 1;
	while (fptr != filename && *fptr != '/' && *fptr != '\\')
		fptr--;

	// NULL-terminate at the separator to remove the filename
	*fptr = 0;

	// Close path string
	strncat(s_BuildArgs, "\"", sizeof(s_BuildArgs) - 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
	const char* error;
	int return_code;
	OpenCL* ocl;

	// Attempt to parse arguments
	if ((error = parseArguments(argc, argv)))
	{
		printHeader();
		printf("\nError parsing arguments: %s\n\n", error);
		printUsage();
		return 1;
	}

	// Print program information
	if (s_PrintHeader)
		printHeader();
	if (s_PrintHelp)
		printUsage();

	// Some OpenCL compilers can't pick up includes in the same directory as the input filename
	// without explicitly telling them about it
	addIncludePathForFile(s_InputFilename);

	LOG("\n");

	if ((error = create(&ocl)))
	{
		printf("\nError initialising OpenCL: %s\n\n", error);
		return 1;
	}

	if ((return_code = loadAndCompileProgram(ocl)) == CL_BUILD_SUCCESS)
	{
		FILE* f;

		if (s_outputFilename[0] != 0)
		{
			remove(s_outputFilename);
			f = fopen(s_outputFilename, "w");
			if (f)
				fclose(f);
			else
				printf("Unable to open %s for write\n", s_outputFilename);
		}
	}

	destroy(&ocl);

	return 0;
}

