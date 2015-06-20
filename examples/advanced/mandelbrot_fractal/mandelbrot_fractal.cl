// Taken from https://www.shadertoy.com/view/lsX3W4 and slightly changed for OpenCL syntax

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// This shader computes the distance to the Mandelbrot Set for everypixel, and colorizes
// it accoringly.
// 
// Z -> Z²+c, Z0 = 0. 
// therefore Z' -> 2·Z·Z' + 1
//
// The Hubbard-Douady potential G(c) is G(c) = log Z/2^n
// G'(c) = Z'/Z/2^n
//
// So the distance is |G(c)|/|G'(c)| = |Z|·log|Z|/|Z'|
//
// More info here: http://www.iquilezles.org/www/articles/distancefractals/distancefractals.htm

__kernel void kern(global int* output, float time)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    float2 p;
    p.x = -1.0f + 2.0f * (x / 1280.0f);
    p.y = -1.0f + 2.0f * (y / 720.0f);
    p.x *= 1280.0f/720.0f;

    // animation	
    float tz = 0.5f - 0.5f * cos(0.225f * time);
    float zoo = pow(0.5f, 13.0f * tz);
    float2 c = (float2)(-0.05f, 0.6805f) + (p * zoo); 

    // iterate
    float2 z  = float2(0.0f);
    float m2 = 0.0f;
    float2 dz = float2(0.0f);

    for (int i = 0; i < 256; i++)
    {
        if (m2 > 1024.0f) 
            continue;

        // Z' -> 2·Z·Z' + 1
        dz = 2.0f * float2(z.x * dz.x-z.y * dz.y, z.x * dz.y + z.y * dz.x) + (float2)(1.0f, 0.0f);

        // Z -> Z² + c			
        z = (float2)(z.x * z.x - z.y * z.y, 2.0f * z.x * z.y) + c;

        m2 = dot(z,z);
    }

    // distance	
    // d(c) = |Z|·log|Z|/|Z'|
    float d = 0.5f * sqrt(dot(z,z) / dot(dz,dz)) * log(dot(z,z));

    // do some soft coloring based on distance
    d = clamp((100 * d) / zoo, 0.0f, 1.0f);
    d = pow(d, 0.25f);

    int t = clamp(d, 0.0f, 1.0f) * 255.0f;

    output[(y * 1280) + x] = (t << 16) | (t << 8) | t;
}
