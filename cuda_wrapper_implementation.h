#ifndef CUDA_WRAPPER_IMPLEMENTATION_H
#define CUDA_WRAPPER_IMPLEMENTATION_H
#include "cuda_wrapper_definition.h"
#include "Camera.h"

glm::vec3* cudaWrapper(Camera *c, primitive *primitiveList, int numPrimitives, SceneLightData *lightList, int numLights, SceneGlobalData SGD){ 
	return runCuda(c, primitiveList, numPrimitives,lightList, numLights, SGD); 
};
#endif