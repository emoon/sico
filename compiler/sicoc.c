///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sico.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void printHeader()
{
    printf("SICOC - Si(mple) Co(mpute) Compiler\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void writeOutput(const char* filename)
{
    FILE* f;

    if (filename[0] == 0)
        return;

    remove(filename);

    f = fopen(filename, "w");
    if (f)
        fclose(f);
    else
        printf("SICOC: Unable to open %s for write\n", filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
    struct SICODevice* device;

    if (!scInitialize())
    {
        printf("Unable to init OpenCL\n");
        return 0;
    }

    printHeader();

    if (argc < 3)
    {
        printf("Usage: sicoc <input.cl> output\n");
        return 0;
    }

    if (!(device = scGetBestDevice()))
        return 0;

    if (!scCompileFromFile(device, argv[1], 0))
        return 0;

    writeOutput(argv[2]);

    scClose();

    return 0;
}

