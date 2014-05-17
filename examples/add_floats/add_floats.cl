// Basic example of adding floats from one input and write to another output

__kernel void add_floats(global float* input, global float* output)
{
    size_t i = get_global_id(0);
    output[i] = input[i] + input[i];
}
