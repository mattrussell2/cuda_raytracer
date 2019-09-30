#include <iostream>
#include <time.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "cuda_wrapper_definition.h"
#include "Camera.h"
#include "cone.h"
#include "cube.h"
#include "cylinder.h"
#include "sphere.h"
#include "matrix_multiply.h"
#include "node.h"
#include "screen.h"


#define checkCudaErrors(val) check_cuda( (val), #val, __FILE__, __LINE__ )

void check_cuda(cudaError_t result, char const *const func, const char *const file, int const line) {
	if (result) {
		std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << " at " <<
			file << ":" << line << " '" << func << "' \n";
		// Make sure we call CUDA Device Reset before exiting
		cudaDeviceReset();
		printf("error");
		return;
		//exit(99);
	}
};

__device__ bool check_light_is_closest(glm::vec3 origin, glm::vec3 ray, float light_t, LightType type, const primitive *d_primitiveList, int numPrimitives) {
	int currShape;
	float t = -1.0;
	origin = origin + ray * 2.0f * 1e-4f;
	for (int i = 0; i < numPrimitives;i++) {
		currShape = d_primitiveList[i].type;
		switch (currShape) {
		case SHAPE_SPHERE:			
			t = sphere::hit(origin, ray, d_primitiveList[i].inverse_transform);
			break;
		case SHAPE_CUBE:
			t = cube::hit(origin, ray, d_primitiveList[i].inverse_transform);
			break;
		case SHAPE_CYLINDER:
			t = cylinder::hit(origin, ray, d_primitiveList[i].inverse_transform);
			break;
		case SHAPE_CONE:
			t = cone::hit(origin, ray, d_primitiveList[i].inverse_transform);
			break;
		default:
			break;
		}
		if (type == LIGHT_DIRECTIONAL && t > 0) return false;
		else if (t > 0 && t < light_t) return false;
	}
	return true;
};

__device__ glm::vec3 calcPhongLight(glm::vec3 eye, glm::vec3 ray, float t, glm::vec3 intersection, glm::vec3 normal, primitive *n, const primitive *d_primitiveList, int d_numPrimitives,
									const SceneLightData *d_lightList, int d_numLights, SceneGlobalData SGD) {
		
	glm::vec3 lineOfSight = glm::normalize(-1.0f * (float)t * ray);	
	
	//start final color as the ambient light for each r,g,b
	glm::vec3 final_color = SGD.ka * n->ambient;	
	glm::vec3 light_ray;
	for (int light = 0; light < d_numLights; light++) {
		if (d_lightList[light].type == LIGHT_DIRECTIONAL) { light_ray = -1.0f * d_lightList[light].dir; }
		else if (d_lightList[light].type == LIGHT_POINT) { light_ray = d_lightList[light].pos - intersection; }
		else continue;

		glm::vec3 worldLightVec(glm::normalize(light_ray));

		//skip this light if the object is closer	
		if (!check_light_is_closest(intersection, worldLightVec, glm::length(light_ray), d_lightList[light].type, d_primitiveList, d_numPrimitives))
			continue;

		float WLVdotNORM = glm::dot(worldLightVec, normal);		
		glm::vec3 reflectedRay = glm::normalize(2.0f * WLVdotNORM * normal - worldLightVec);		
		float REFdotLOS = glm::dot(reflectedRay, lineOfSight);
		if (WLVdotNORM < 0) WLVdotNORM = 0.0f;
		if (REFdotLOS < 0) REFdotLOS = 0.0f;		

		final_color += d_lightList[light].col * (SGD.kd * n->color * WLVdotNORM) + SGD.ks * n->specular * pow(REFdotLOS, n->shininess);		
	}

	return final_color;
};


__device__ void getIntersection(glm::vec3 &eye, glm::vec3 ray, float &t, glm::vec3 &currIntersection, glm::vec3 &currNormal, primitive *&currPrimitive, primitive *d_primitiveList, int d_numPrimitives) {
	t = 100000000;
	float temp_t;	
	primitive *n;

	eye = eye + ray * 2.0f * 1e-4f;	
	for (int i = 0;i < d_numPrimitives;i++) {
		n = &(d_primitiveList[i]);
		switch (n->type) {
		case SHAPE_SPHERE:			
			temp_t = sphere::hit(eye, ray, n->inverse_transform);
			if (temp_t > t || temp_t < 0) continue;
			t = temp_t;
			currNormal = sphere::get_norm(eye, ray, t, n->inverse_transform);
			currIntersection = eye + (float)t * ray;
			currPrimitive = n;
			break;
		case SHAPE_CYLINDER:
			temp_t = cylinder::hit(eye, ray, n->inverse_transform);
			if (temp_t > t || temp_t < 0) continue;
			t = temp_t;
			currNormal = cylinder::get_norm(eye, ray, t, n->inverse_transform);
			currIntersection = eye + (float)t * ray;
			currPrimitive = n;
			break;
		case SHAPE_CUBE:
			temp_t = cube::hit(eye, ray, n->inverse_transform);
			if (temp_t > t || temp_t < 0) continue;
			t = temp_t;
			currNormal = cube::get_norm(eye, ray, t, n->inverse_transform);
			currIntersection = eye + (float)t * ray;
			currPrimitive = n;
			break;
		case SHAPE_CONE:
			temp_t = cone::hit(eye, ray, n->inverse_transform);
			if (temp_t > t || temp_t < 0) continue;
			t = temp_t;
			currNormal = cone::get_norm(eye, ray, t, n->inverse_transform);
			currIntersection = eye + (float)t * ray;
			currPrimitive = n;
			break;
		default:
			break;
		}
	}	
	if (IN_RANGE(t, 100000000)) t = -1.0f;
}

//calculate all the rays and their colors in parallel. 
__global__ void render(glm::vec3 *d_finalColors, int max_x, int max_y,  
					   glm::vec3 *rOrigins, glm::vec3 *rRays, glm::vec3 *rNormals, glm::vec3 *rIntersections, float *rTvals, primitive **rPrimitives,
					   glm::vec3 eyePoint, glm::mat4 inverseCameraTransformMatrix,
					   primitive *d_primitiveList, int d_numPrimitives, const SceneLightData *d_lightList, int d_numLights, SceneGlobalData SGD, 
					   int recursionDepth) {

	// final_color;
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int j = threadIdx.y + blockIdx.y * blockDim.y;
	if ((i >= max_x) || (j >= max_y)) return;
	int pixel_index = j * max_x + i;
	
	//map a pixel to the film plane
	float u = 2.0 * float(i) / float(max_x) - 1.0;
	float v = 2.0 * float(j) / float(max_y) - 1.0;
	float z = -1.0;
	glm::vec3 world_space_pt = glm::vec3(inverseCameraTransformMatrix * glm::vec4(u, v, z, 1.0f));
	
	int start = pixel_index * recursionDepth;	
	glm::vec3 intersection, normal, inboundVector, reflectiveRay;
	float t;
	primitive *p;
	rOrigins[start] = eyePoint;
	rRays[start] = glm::normalize(world_space_pt - eyePoint);
	for (int i = 0; i < recursionDepth; i++) {		
		getIntersection(rOrigins[start + i],rRays[start + i], t, intersection, normal, p, d_primitiveList, d_numPrimitives);
		rTvals[start + i] = t;                                   //t value of the current ray
		if (t < 0) break;                                        //base case (didn't hit anything)	
		rNormals[start + i] = normal;                            //normal of the 'future' intersection
		rIntersections[start + i] = intersection;                //intersection at the point you're headed to
		rPrimitives[start + i] = p;                              //primitive you're GOING to hit
		
		//determine the next ray
		inboundVector = glm::normalize(intersection - rOrigins[start+i]);
		reflectiveRay = glm::normalize(inboundVector - 2.0f * glm::dot(inboundVector, normal) * normal);
		if (i < recursionDepth - 1) {
			rOrigins[start + i + 1] = intersection;                       //save these values for the next 'recursive' call
			rRays[start + i + 1] = reflectiveRay;
		}
	}	

	//now we've got a structure with information for each step of the way. work from the back to front and add up the lighting info.
	glm::vec3 final_color(0.0f); 	
	glm::vec3 local_color;			
	for (int i = recursionDepth - 1; i >= 0; i--) {	
		if (rTvals[start + i] < 0) continue;		
		local_color = calcPhongLight(rOrigins[start + i], rRays[start + i], rTvals[start+i], rIntersections[start+i], rNormals[start+i], rPrimitives[start+i],
									 d_primitiveList, d_numPrimitives, d_lightList, d_numLights, SGD);
		final_color = local_color + SGD.ks * rPrimitives[start + i]->reflective * final_color;					
	}	
	d_finalColors[pixel_index] = final_color;
};


glm::vec3* runCuda(Camera *camera, primitive *primitiveList, int numPrimitives, 
				   SceneLightData *lightList, int numLights, SceneGlobalData SGD) { 

	checkCudaErrors(cudaSetDevice(0));
	checkCudaErrors(cudaDeviceReset());
	
	int nx = screenWidth;
	int ny = screenHeight;
	int tx = 8;
	int ty = 8;

	int numPixels = screenWidth * screenHeight;
	
	primitive *d_primitiveList; 
	checkCudaErrors(cudaMallocManaged((void **)&d_primitiveList, sizeof(primitive) * numPrimitives));
	checkCudaErrors(cudaMemcpy(d_primitiveList, primitiveList, sizeof(primitive) * numPrimitives, cudaMemcpyHostToDevice));
	
	SceneLightData *d_lightList;
	checkCudaErrors(cudaMallocManaged((void **)&d_lightList, sizeof(SceneLightData) * numLights));
	checkCudaErrors(cudaMemcpy(d_lightList, lightList, sizeof(SceneLightData) * numLights, cudaMemcpyHostToDevice));
		
	int recursionDepth = 5;
	
	glm::vec3 *rOrigins, *rRays, *rNormals, *rIntersections;
	float *rTvals; primitive **rPrimitives;
	
	cudaMalloc((void **)&rOrigins, sizeof(glm::vec3) * numPixels * recursionDepth);
	cudaMalloc((void **)&rRays, sizeof(glm::vec3) * numPixels * recursionDepth);
	cudaMalloc((void **)&rNormals, sizeof(glm::vec3) * numPixels * recursionDepth);
	cudaMalloc((void **)&rIntersections, sizeof(glm::vec3) * numPixels * recursionDepth);
	cudaMalloc((void **)&rTvals, sizeof(float) * numPixels * recursionDepth);	
	cudaMalloc((void **)&rPrimitives, sizeof(primitive*) * numPixels * recursionDepth);
		
	float *rTvalsTmp = new float[numPixels * recursionDepth];
	for (int i = 0;i < numPixels * recursionDepth;i++) {
		rTvalsTmp[i] = -1.0f;
	}
	cudaMemcpy(rTvals, rTvalsTmp, sizeof(float) * numPixels * recursionDepth, cudaMemcpyHostToDevice);
	delete []rTvalsTmp;

	glm::vec3 *d_finalColors;
	cudaMalloc((void **)&d_finalColors, sizeof(glm::vec3) * numPixels);
	
	//get camera information		
	glm::vec3 eye_point = camera->getEyePoint();
	glm::mat4 inverse_transform = camera->getInverseModelViewMatrix() * camera->getInverseScaleMatrix();
	
	clock_t start, stop;
	start = clock();
	
	//number of blocks and threads needed for per-pixel optimization
	dim3 blocks(nx / tx + 1, ny / ty + 1);
	dim3 threads(tx, ty);

	render <<< blocks, threads >>> (d_finalColors, nx, ny, rOrigins, rRays, rNormals, rIntersections, rTvals, rPrimitives,
								    eye_point, inverse_transform, d_primitiveList, numPrimitives, d_lightList, numLights, SGD, recursionDepth);
		
	checkCudaErrors(cudaDeviceSynchronize());
		
	stop = clock();
	double timer_seconds = ((double)(stop - start)) / CLOCKS_PER_SEC;
	printf("took %f seconds\n", (float)timer_seconds);

	//copy device memory back to the host --- [the memory is freed MyGLCanvas]
	glm::vec3* h_finalColors = (glm::vec3 *)malloc(sizeof(glm::vec3) * numPixels);
	checkCudaErrors(cudaMemcpy(h_finalColors, d_finalColors, sizeof(glm::vec3) * numPixels, cudaMemcpyDeviceToHost));
		
	checkCudaErrors(cudaFree(rRays));
	checkCudaErrors(cudaFree(rNormals));
	checkCudaErrors(cudaFree(rIntersections));
	checkCudaErrors(cudaFree(rTvals));
	checkCudaErrors(cudaFree(rPrimitives));
	checkCudaErrors(cudaFree(d_finalColors));	
	checkCudaErrors(cudaFree(d_primitiveList));
	checkCudaErrors(cudaFree(d_lightList));	
	return h_finalColors;
}