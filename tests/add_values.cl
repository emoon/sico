// Basic example of adding floats from two inputs and write to another output

__kernel void kern(global float* output, global float* inputA, global float* inputB)
{
    size_t i = get_global_id(0);
    output[i] = inputA[i] + inputB[i];
}
