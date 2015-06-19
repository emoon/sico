#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <sico.h>

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

int main()
{
    const UnitTest tests[] =
    {
        unit_test(sico_init),
        unit_test(sico_get_devices),
    };

    return run_tests(tests);
}

