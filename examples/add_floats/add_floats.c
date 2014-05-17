#include <CLW.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    size_t i;
    const size_t floatCount = 64 * 1024;
    const size_t byteSize = floatCount * sizeof(float);

    float* data = malloc(byteSize);
    float* dataRes = malloc(byteSize);

    for (i = 0; i < floatCount; ++i)
        data[i] = (float)i;

    wclRunKernel1DArray(dataRes, data, "examples/add_floats/add_floats.cl", floatCount, byteSize);

    for (i = 0; i < 10; ++i)
        printf("data %f\n", dataRes[i]);

    return 0;
}
