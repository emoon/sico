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

static void sico_init(void** __unused state)
{
	assert_int_equal(scInitialize(), 1);
	scClose();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_get_devices(void** __unused state)
{
	int deviceCount;

	scGetAllDevices(&deviceCount);
	assert_int_not_equal(deviceCount, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sico_float_add_default_dev(void** __unused state)
{
    size_t i;
    float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    float inputData2[] = { 11.0f, 12.0f, 13.0f, 14.0f };
    const float expectedResult[] = { 12.0f, 14.0f, 16.0f, 18.0f };

    const size_t inputCount = SICO_SIZEOF_ARRAY(inputData);
    float dataRes[SICO_SIZEOF_ARRAY(inputData)];

    scRunKernel1DArraySimple(dataRes, inputData, inputData2, "tests/add_values.cl", inputCount, sizeof(inputData));

    for (i = 0; i < inputCount; ++i)
    	assert_true(fabs(dataRes[i] - expectedResult[i]) < FLT_EPSILON); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const UnitTest tests[] =
    {
        unit_test(sico_init),
        unit_test(sico_get_devices),
        unit_test(sico_float_add_default_dev),
    };

    return run_tests(tests);
}

