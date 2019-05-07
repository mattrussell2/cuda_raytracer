#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include <helper_functions.h>
#include <helper_cuda.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


__global__ void simpleMatMatMulKernel(float *d_A, float *d_B, float *d_C, int width) {
	int row = blockIdx.y*width + threadIdx.y;
	int col = blockIdx.x*width + threadIdx.x;

	if (row < width && col < width) {
		float product_val = 0;
		for (int k = 0;k < width;k++) {
			printf("\n");
			printf("d_A[row*width+k]: %f\n", d_A[row*width + k]);
			printf("d_B[row*width+k]: %f\n", d_B[row*width + k]);
			product_val += d_A[row*width + k] * d_B[k*width + col];
			printf("product val = %f\n", product_val);
		}
		d_C[row*width + col] = product_val;
	}
}
#endif


__global__ void simpleVecMatMulKernel(float *d_A, float *d_B, float *d_C, int width) {
	int row = blockIdx.y*width + threadIdx.y;
	int col = blockIdx.x*width + threadIdx.x;

	if (row < width && col < width) {
		float product_val = 0;
		for (int k = 0;k < width;k++) {			
			product_val += d_A[row*width + k] * d_B[col];
		}		
		d_C[col] = product_val;
	}
}


//blocks are how many times you run the kernel(?)
glm::vec4 vecMatMul(glm::mat4 mat, glm::vec4 vec) {
	// Initialize host memory	
	int block_size = 4;
	float *h_A = glm::value_ptr(mat);
	float *h_B = glm::value_ptr(vec);
	unsigned int mem_size_A = sizeof(float) * 16;
	unsigned int mem_size_B = sizeof(float) * 4;

	// Allocate device memory
	float *d_A, *d_B, *d_C;

	// Allocate host matrix C
	//dim3 dimsC(dimsB.x, dimsA.y, 1);
	unsigned int mem_size_C = 4 * sizeof(float);//dimsC.x * dimsC.y
	float *h_C = reinterpret_cast<float *>(malloc(mem_size_C));

	if (h_C == NULL) {
		fprintf(stderr, "Failed to allocate host matrix C!\n");
		exit(EXIT_FAILURE);
	}

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_A), mem_size_A));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_B), mem_size_B));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_C), mem_size_C));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice));

	// Setup execution parameters
	dim3 threads(block_size, block_size);
	dim3 grid(4 / threads.x, 4 / threads.y);
	//dim3 grid(dimsB.x / threads.x, dimsA.y / threads.y);

	cudaDeviceSynchronize();

	// Allocate CUDA events that we'll use for timing
	cudaEvent_t start;
	checkCudaErrors(cudaEventCreate(&start));

	cudaEvent_t stop;
	checkCudaErrors(cudaEventCreate(&stop));

	// Record the start event
	checkCudaErrors(cudaEventRecord(start, NULL));

	// Execute the kernel
	//int nIter = 300;

	//for (int j = 0; j < nIter; j++) {	
	printf("about to multiply");
	simpleVecMatMulKernel <<<grid, threads >>> (d_A, d_B, d_C, 4);
	//	}
	printf("finished multiplying");
		// Record the stop event
	checkCudaErrors(cudaEventRecord(stop, NULL));

	// Wait for the stop event to complete
	checkCudaErrors(cudaEventSynchronize(stop));

	printf("copying result to host");
	// Copy result from device to host
	checkCudaErrors(cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost));
	glm::vec4 retval = glm::make_vec4(h_C);
	//free(h_A);
	//free(h_B);
	free(h_C);
	checkCudaErrors(cudaFree(d_A));
	checkCudaErrors(cudaFree(d_B));
	checkCudaErrors(cudaFree(d_C));

	return retval;
}
glm::mat4 MatMatMul(glm::mat4 mat_A, glm::mat4 mat_B) {
	// Initialize host memory	
	int block_size = 4;
	float *h_A = glm::value_ptr(mat_A);
	float *h_B = glm::value_ptr(mat_B);
	unsigned int mem_size_A = sizeof(float) * 16;
	unsigned int mem_size_B = sizeof(float) * 16;

	// Allocate device memory
	float *d_A, *d_B, *d_C;

	// Allocate host matrix C
	//dim3 dimsC(dimsB.x, dimsA.y, 1);
	unsigned int mem_size_C = 16 * sizeof(float);//dimsC.x * dimsC.y
	float *h_C = reinterpret_cast<float *>(malloc(mem_size_C));

	if (h_C == NULL) {
		fprintf(stderr, "Failed to allocate host matrix C!\n");
		exit(EXIT_FAILURE);
	}

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_A), mem_size_A));

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_B), mem_size_B));

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_C), mem_size_C));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice));

	// Setup execution parameters
	dim3 threads(block_size, block_size);
	dim3 grid(4 / threads.x, 4 / threads.y);
	//dim3 grid(dimsB.x / threads.x, dimsA.y / threads.y);

	cudaDeviceSynchronize();

	// Allocate CUDA events that we'll use for timing
	cudaEvent_t start;
	checkCudaErrors(cudaEventCreate(&start));

	cudaEvent_t stop;
	checkCudaErrors(cudaEventCreate(&stop));

	// Record the start event
	checkCudaErrors(cudaEventRecord(start, NULL));

	// Execute the kernel
	//int nIter = 300;

	//for (int j = 0; j < nIter; j++) {	
	printf("about to multiply");
	simpleMatMatMulKernel << <grid, threads >> > (d_A, d_B, d_C, 4);
	//	}
	printf("finished multiplying");
	// Record the stop event
	checkCudaErrors(cudaEventRecord(stop, NULL));

	// Wait for the stop event to complete
	checkCudaErrors(cudaEventSynchronize(stop));

	printf("copying result to host");
	// Copy result from device to host
	checkCudaErrors(cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost));
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 4;j++) {
			printf("val at i,j: %f ", h_C[j*4+i]);
		}
		printf("\n");
	}
	glm::mat4 retval = glm::make_mat4(h_C);
	//free(h_A);
	//free(h_B);
	free(h_C);
	checkCudaErrors(cudaFree(d_A));
	checkCudaErrors(cudaFree(d_B));
	checkCudaErrors(cudaFree(d_C));

	return retval;
}