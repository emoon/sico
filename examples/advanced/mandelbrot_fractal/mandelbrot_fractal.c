#include <sico.h>
#include <stdio.h>
#include <MiniFB.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 720
static unsigned int s_buffer[WIDTH * HEIGHT];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    SICODevice device;
    SICOKernel kernel;
    SICOCommanQueue queue;

    scInitialize();

    if (!(device = scGetBestDevice()))
    {
        printf("Unable to get OpenCL device\n");
        return 0;
    }

    if (!(kernel = scCompileKernelFromSourceFile(device, "examples/advanced/mandelbrot_fractal/mandelbrot_fractal.cl", "kern", "")))
        return 0;

    if (!mfb_open("SICO - OpenCL - Mandelbrot Fractal - Press ESC to Exit", WIDTH, HEIGHT))
        return 0;

    queue = scCreateCommandQueue(device);

    float time = 0.0f;

    for (;;)
    {
        SICOParam params[] =
        {
            { (uintptr_t)s_buffer, SICO_MEM_READ_WRITE, SICO_AutoAllocate, WIDTH * HEIGHT * sizeof(unsigned int), 0 },
            { (uintptr_t)&time, SICO_PARAMETER, 0, sizeof(float), 0 },
        };

        scAddKernel2D(queue, device, kernel, WIDTH, HEIGHT, params, SICO_SIZEOF_ARRAY(params));
        scCommandQueueFinish(queue);
        scFreeParams(params, SICO_SIZEOF_ARRAY(params)); // TODO: Get rid of this call.. a bit tricky though (only needed with autoallocate)

        int state = mfb_update(s_buffer);

        time += 0.01f;

        if (state < 0)
            break;
    }

    scDestroyCommandQueue(queue);
    scClose();

    mfb_close();

    return 1;
}
