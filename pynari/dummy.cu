#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

__global__ void dummyKernel_pynari()
{
  printf("dummuy\n");
}

extern "C" void dummy_pynari()
{
  dummyKernel_pynari<<<32, 32>>>();
 
}

