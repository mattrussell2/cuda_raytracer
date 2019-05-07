#ifndef CUDA_WRAPPER_DEFINITION_H
#define CUDA_WRAPPER_DEFINITION_H
#include "Camera.h"
#include "Node.h"

glm::vec3* runCuda(Camera *c, primitive *primitiveList, int numPrimitives, SceneLightData *lightList, int numLights, SceneGlobalData SGD);

#endif