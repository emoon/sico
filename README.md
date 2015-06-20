SICO
=============

SICO SI(mple) CO(mpute) is a library built on top of OpenCL in order to make it easier to use OpenCL. OpenCL in general has quite a bit of setup before one actually can start using it. The goal of SICO is to make this a bit easier and accessible (mainly from C/C++ to begin with) if you for example want to run an OpenCL kernel accress a 1D array you would write

```
scRunKernel1DArray(...)
```  
  
And here is a small example:

```
float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
float inputData2[] = { 5.0f, 6.0f, 7.0f, 8.0f };
float outputData[sizeof_array(inputData)];

scRunKernel1DArraySimple(outputData, inputData, inputData2, "mykernel.cl", sizeof_array(inputData), sizeof(inputData);
```

And you are done. See https://github.com/emoon/sico/blob/master/examples/add_floats/add_floats.c for a full example similar to this code.

This example of course trades some performance for simplicity but SICO can be used with more function calls and with more controls in order to improve performance.

Another example is a Mandelbrot fractal

![fractal](https://cloud.githubusercontent.com/assets/350822/8268555/37961350-178a-11e5-8e2a-97a4f4901d69.jpg)

And the code can be found here https://github.com/emoon/sico/blob/master/examples/advanced/mandelbrot_fractal/mandelbrot_fractal.c

Building
--------

SICO uses the Tundra build system to build it's code. The binaries for Tundra is included for Windows and Mac but for Linux/*BSD/*NIX you need to build it yourself. The latest version can be obtained from here https://github.com/deplinenoise/tundra


Mac
---

From the root directory run scripts/mac_build_debug.sh
To generate a xcode solution run scripts/mac_gen_xcode_solution.sh


Windows
-------

From the root directory run scripts\win64_build_debug.cmd (Notice that msvc is expected to be in the path so you may have to run scripts\vcvarsx86_amd64.bat first)
To run from Visual Studio a project can be generated using scripts\win64_gen_vs_solution.cmd


Linux/*BSD/*NIX
---------------

Build the Tundra binary unless you have already.
From the root directory run scripts/unix_gcc_build_debug.sh


Status
------

The library is still in early development but feedback, ideas, etc are always welcome.

