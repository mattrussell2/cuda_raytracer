#ifndef SHAPE_H
#define SHAPE_H

#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>


//const float EPSILON = 1e-4;//1e-5;
#define IN_RANGE(a,b)   (((a>(b-1e-4f))&&(a<(b+1e-4f)))?1:0)

#define PI glm::pi<float>()  //PI is now a constant for 3.14159....
using namespace std;

enum OBJ_TYPE {
	SHAPE_CUBE = 0,
	SHAPE_CYLINDER = 1,
	SHAPE_CONE = 2,
	SHAPE_SPHERE = 3,
	SHAPE_PYRAMID = 4,
	SHAPE_TAPER = 5,
	SHAPE_MESH = 6,
	SHAPE_SPECIAL1 = 7
};

#endif