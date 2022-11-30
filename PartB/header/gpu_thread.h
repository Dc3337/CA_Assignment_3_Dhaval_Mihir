#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>

// Create other necessary functions here
__global__ void redMatMul(int N, const int* A, const int* B, int* out)
{
	// enforce N to be power of 2 and greater than 2
	assert(N >= 4 and N == (N & ~(N - 1)));

	// Compute each thread's global row and column index
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int index = row * (N>>1) + col;
	row = row<<1;
	col = col<<1;
	// Iterate over row, and down column
	int sum = 0;
	for (int k = 0; k < N; k++)
	{
		// Accumulate results for a single thread
		sum += A[row * N + k] *	 B[k * N + col];
		sum += A[(row + 1) * N + k] * B[k * N + col];
		sum += A[row * N + k] * B[k * N + (col + 1)];
		sum += A[(row + 1) * N + k] * B[k * N + (col + 1)];
	}
	out[index] = sum;
}

// Fill in this function
void gpuThread(int N, int *matA, int *matB, int *output)
{
	// auto begin = TIME_NOW;
	size_t bytes = N * N * sizeof(int);

	// Allocating CUDA memory
	int* devA, * devB, * devO;
	cudaMalloc(&devA, bytes);
	cudaMalloc(&devB, bytes);
	cudaMalloc(&devO, bytes);

	cudaMemcpy(devA, matA, bytes, cudaMemcpyHostToDevice);
	cudaMemcpy(devB, matB, bytes, cudaMemcpyHostToDevice);
	cudaMemcpy(devO, output, bytes, cudaMemcpyHostToDevice);

	// 2D dim struct for matrix
	int THREADS = 32;
	int BLOCKS = N / (2*THREADS);
	dim3 threads(THREADS, THREADS);
	dim3 blocks(BLOCKS, BLOCKS);
	redMatMul << <blocks, threads >> > (N, devA, devB, devO);

	// back ot CPU memory
	cudaMemcpy(output, devO, bytes, cudaMemcpyDeviceToHost);
	// auto end = TIME_NOW;
    // cout << "GPU execution time: " << 
    // (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
}
