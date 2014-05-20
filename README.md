SICO
=============

SICO SI(mple) CO(mpute) is a library built on top of OpenCL in order to make it easier to use OpenCL. OpenCL in general has quite a bit of setup before one actually can start using it. The goal of SICO is to make this a bit easier and acceasseble (mainly from C/C++ to begin with) if you for example want to run an OpenCL kernel accress a 1D array you would write

  scRunKernel1DArray(...)
  
And here is a small example:

  float inputData[] = { 1.0f, 2.0f, 3.0f, 4.0f };
  float outputData[sizeof_array(inputData)];

  scRunKernel1DArray(outputData, inputData, "mykernel.cl", sizeof_array(inputData), sizeof(inputData);
  
And you are done.

This example of course trades some performance for simplicity but SICO can be used with more function calls and with more controls in order to improve performance.

