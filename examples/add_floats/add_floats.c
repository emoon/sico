#include <sico.h>
#include <stdio.h>
#include <malloc.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    size_t i;
	float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
	const size_t inputCount = oclw_sizeof_array(inputData);
	float dataRes[oclw_sizeof_array(inputData)];

    scRunKernel1DArray(dataRes, inputData, "examples/add_floats/add_floats.cl", inputCount, sizeof(inputData));

    for (i = 0; i < inputCount; ++i)
        printf("data %f\n", dataRes[i]);

    return 0;
}
