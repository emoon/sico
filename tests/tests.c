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
    size_t i;
    float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    float inputData2[] = { 11.0f, 12.0f, 13.0f, 14.0f };
    const float expectedResult[] = { 12.0f, 14.0f, 16.0f, 18.0f };

    const size_t inputCount = SICO_SIZEOF_ARRAY(inputData);
    float dataRes[SICO_SIZEOF_ARRAY(inputData)];

    (void)state;

    scRunKernel1DArraySimple(dataRes, inputData, inputData2, "tests/add_values.cl", inputCount, sizeof(inputData));

    for (i = 0; i < inputCount; ++i)
        assert_true(fabs(dataRes[i] - expectedResult[i]) < FLT_EPSILON);
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

