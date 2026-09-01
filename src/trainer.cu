#include <stdio.h>

// A global function runs on the GPU (the device)
__global__ void cuda_hello() {
    printf("Hello World from the GPU!\n");
}

int main() {
    // Call the GPU function from the CPU (the host)
    cuda_hello<<<1, 1>>>();
    
    // Wait for the GPU to finish before exiting
    cudaDeviceSynchronize();
    
    return 0;
}

