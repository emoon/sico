#include <sico.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    size_t i;
    float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    float inputData2[] = { 11.0f, 12.0f, 13.0f, 14.0f };
    const size_t inputCount = SICO_SIZEOF_ARRAY(inputData);
    float dataRes[SICO_SIZEOF_ARRAY(inputData)];

    scRunKernel1DArray(dataRes, inputData, inputData2, "examples/add_floats/add_floats.cl", inputCount, sizeof(inputData));

    for (i = 0; i < inputCount; ++i)
        printf("data %f\n", dataRes[i]);

    return 0;
}
