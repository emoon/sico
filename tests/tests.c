#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <sico.h>
#include <math.h>
#include <float.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_init(void** state)
{
    (void)state;
    assert_int_equal(scInitialize(), 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_get_devices(void** state)
{
    (void)state;
    int deviceCount;

    scGetAllDevices(&deviceCount);
    assert_int_not_equal(deviceCount, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_float_add_default_dev(void** state)
{
    (void)state;

    int count = 64 * 1024;
    size_t dataSize = sizeof(float) * (size_t)count;

    float* inputData = (float*)malloc(dataSize);
    float* inputData2 = (float*)malloc(dataSize);
    float* dataRes = (float*)malloc(dataSize);
    float* expectedResult = (float*)malloc(dataSize);

    for (int i = 0; i < count; ++i)
    {
        float v = (float)i;
        inputData[i] = v;
        inputData2[i] = 10.0f;
        expectedResult[i] = v + 10.0f;
        dataRes[i] = 0.0f;
    }

    scRunKernel1DArraySimple(dataRes, inputData, inputData2, "tests/add_values.cl", (size_t)count, dataSize);

    for (int i = 0; i < count; ++i)
        assert_true(fabs(dataRes[i] - expectedResult[i]) < FLT_EPSILON);

    free(inputData);
    free(inputData2);
    free(dataRes);
    free(expectedResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_alloc_free(void** state)
{
    int deviceCount = 1;

    (void)state;

    struct SICODevice** devices = scGetAllDevices(&deviceCount);

    for (int i = 0; i < deviceCount; ++i)
    {
        SICOHandle readWrite = scAlloc(devices[i], CL_MEM_READ_WRITE, 64 * 1024, 0);
        assert_int_not_equal(readWrite, 0);
        assert_int_equal(scFree(readWrite), 1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const UnitTest tests[] =
    {
        unit_test(sico_init),
        unit_test(sico_get_devices),
        unit_test(sico_float_add_default_dev),
        unit_test(sico_alloc_free),
    };

    int ret = run_tests(tests);

    scClose();

    return ret;
}

