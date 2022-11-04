/*
 * @Author: Bingyang Jin
 * @Date: 2022-10-26 20:35:07
 * @Editor: Bingyang Jin
 * @FilePath: /src/10_heatSourceGPU/heatSource.cu
 * @Description: create the file
 */

#include <stdio.h>
#include <vector>
#include <cuda_runtime.h>
#include "cuda-samples-master/Common/helper_cuda.h"
#include "device_launch_parameters.h"
#include "cublas_v2.h"
#include "device_functions.h"
#include "../common_book/book.h"

#define REAL double
#define MAX_NUMBER 100000
#define ITER_TIME 1000
#define SPEED 0.33f

std::vector<std::vector<int>> adjoinTriangles; // 三角形的邻接三角形
extern std::vector<REAL> gIntensity[2];
extern int currentPass; // 用这个来表示当前 gIntensity 是第几个，在 0 和 1 之间交替
extern std::vector<int> gSources; // 热源点

struct TriangleAdjoin {
	bool isConstant = false;
	REAL intensity = 0;
	int adj[3];
};

/**
 * CUDA Kernel Device code
 *
 * Computes the vector addition of A and B into C. The 3 vectors have the same
 * number of elements numElements.
 */

__global__ void heatCalculate(TriangleAdjoin* d, int numElements)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < numElements)
	{
		for (int m = 0; m < ITER_TIME; m++) {
			REAL out = d[i].intensity;
			for (int j = 0; j < 3; j++) {
				int index = d[i].adj[j];
				out += SPEED * (d[index].intensity - d[i].intensity);
			}

			__syncthreads();

			d[i].intensity = out;
			if (d[i].isConstant) {
				d[i].intensity = 1;
			}

			__syncthreads();
		}
	}
};

extern "C" int doPropogateGPU()
{
	// 启动定时器
	cudaEvent_t start, stop;
	float elapsedTime;
	HANDLE_ERROR(cudaEventCreate(&start));
	HANDLE_ERROR(cudaEventCreate(&stop));
	HANDLE_ERROR(cudaEventRecord(start, 0));

	// Error code to check return values for CUDA calls
	cudaError_t err = cudaSuccess;

	// Print the vector length to be used, and compute its size
	int numElements = gIntensity[currentPass].size();
	int prevPass = currentPass;
	currentPass = 1 - currentPass;

	size_t size = numElements * sizeof(TriangleAdjoin);

	// 准备 CPU 数据
	// Allocate the host input vector h
	TriangleAdjoin* h = (TriangleAdjoin*)malloc(size);

	// Verify that allocations succeeded
	if (h == NULL)
	{
		fprintf(stderr, "Failed to allocate host vectors!\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numElements; i++) {
		//h[i] = gIntensity[prevPass][i];
		h[i].intensity = gIntensity[prevPass][i];
		h[i].isConstant = false;
		for (int j = 0; j < 3; j++) {
			h[i].adj[j] = adjoinTriangles[i][j];
		}
	}
	for (int i = 0; i < gSources.size(); i++) {
		h[gSources[i]].isConstant = true;
	}

	// 在GPU上创建内存
	// Allocate the device input vector d
	TriangleAdjoin* d = NULL;
	err = cudaMalloc((void**)&d, size);

	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to allocate device vector d_0 (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// 将 CPU 内的值拷贝到 GPU 中
	// Copy the host input vectors A and B in host memory to the device input vectors in device memory
	err = cudaMemcpy(d, h, size, cudaMemcpyHostToDevice);

	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to copy vector A from host to device (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// 进行 GPU 计算
	// Launch the Vector Add CUDA Kernel
	int threadsPerBlock = 1024;
	int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

	heatCalculate << <blocksPerGrid, threadsPerBlock >> > (d, numElements);

	err = cudaGetLastError();

	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to launch heatAdd kernel (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// 将 GPU 的结果拷贝回 CPU 中
	// Copy the device result vector in device memory to the host result vector
	// in host memory.
	free(h);
	h = (TriangleAdjoin*)malloc(size);
	err = cudaMemcpy(h, d, size, cudaMemcpyDeviceToHost);

	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to copy vector C from device to host (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numElements; i++) {
		gIntensity[currentPass][i] = h[i].intensity;
	}
	for (int i = 0; i < gSources.size(); i++) {
		gIntensity[currentPass][gSources[i]] = 1.0;
	}

	//printf("###################\n");

	// 释放空间
	// Free device global memory
	err = cudaFree(d);

	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	// Free host memory
	free(h);

	// 计算时间
	HANDLE_ERROR(cudaEventRecord(stop, 0));
	HANDLE_ERROR(cudaEventSynchronize(stop));
	HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime, start, stop));
	printf("Do %d Times and Time taken: %3.1f ms\n", ITER_TIME, elapsedTime);

	return 0;
}