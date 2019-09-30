#ifndef CONE_H
#define CONE_H

#include "shape.h"
#include<math.h>
using namespace std;

class cone {
public:
	__device__ cone() {};	
	__device__ static const float hit(glm::vec3 origin, glm::vec3 direction, glm::mat4 inverseTransform) {
		origin = inverseTransform * glm::vec4(origin, 1.0f);
		direction = inverseTransform * glm::vec4(direction, 0.0f);

		float a = origin.x;
		float b = origin.y;
		float c = origin.z;
		float x = direction.x;
		float y = direction.y;
		float z = direction.z;
		float ts[5];

		if (4.0f * pow(x, 2) - pow(y, 2) + 4.0f*pow(z, 2) != 0) {
			ts[0] = (-0.25f*sqrt(pow(32.0f*a*x - 8.0f*b*y + 32.0f*c*z + 4.0f*y, 2.0f) - 4.0f*(16.0f*pow(a, 2.0f) - 4.0f*pow(b, 2.0f) + 4.0f*b + 16.0f*pow(c, 2.0f) - 1.0f)*(16.0f*pow(x, 2.0f) - 4.0f*pow(y, 2.0f) + 16.0f*pow(z, 2.0f))) - 8.0f*a*x + 2.0f*b*y - 8.0f*c*z - y) / (2.0f*(4.0f*pow(x, 2.0f) - pow(y, 2.0f) + 4.0f*pow(z, 2.0f)));
			ts[1] = (0.25f*sqrt(pow(32.0f*a*x - 8.0f*b*y + 32.0f*c*z + 4.0f*y, 2.0f) - 4.0f*(16.0f*pow(a, 2.0f) - 4.0f*pow(b, 2.0f) + 4.0f*b + 16.0f*pow(c, 2.0f) - 1.0f)*(16.0f*pow(x, 2.0f) - 4.0f*pow(y, 2.0f) + 16.0f*pow(z, 2.0f))) - 8.0f*a*x + 2.0f*b*y - 8.0f*c*z - y) / (2.0f*(4.0f*pow(x, 2.0f) - pow(y, 2.0f) + 4.0f*pow(z, 2.0f)));
		}
		else {
			ts[0] = -1.0f;
			ts[1] = -1.0f;
		}

		if ((x == -0.5f * sqrt(pow(y, 2) - 4.0f * pow(z, 2))) && (-4.0f * a * sqrt(pow(y, 2) - 4.0f * pow(z, 2)) - 2.0f * b * y + 8.0f * c * z + y) != 0) {
			ts[2] = (16.0f*pow(a, 2.0f) - 4.0f*pow(b, 2.0f) + 4.0f*b + 16.0f*pow(c, 2.0f) - 1.0f) / (4.0f*(4.0f*a*sqrt(pow(y, 2.0f) - 4.0f*pow(z, 2.0f)) + 2.0f*b*y - 8.0f*c*z - y));
		}
		else ts[2] = -1.0f;

		if ((x == 0.5f * sqrt(pow(y, 2) - 4.0f * pow(z, 2))) && (4.0f * a * sqrt(pow(y, 2) - 4.0f * pow(z, 2)) - 2.0f * b * y + 8.0f * c * z + y) != 0) {
			ts[3] = (-16.0f*pow(a, 2.0f) - 4.0f*pow(b, 2.0f) + 4.0f*b + 16.0f*pow(c, 2.0f) - 1.0f) / (4.0f*(4.0f*a*sqrt(pow(y, 2.0f) - 4.0f*pow(z, 2.0f)) + 2.0f*b*y - 8.0f*c*z - y));
		}
		else ts[3] = -1.0f;

		ts[4] = (-0.5f - origin.y) / direction.y;

		glm::vec3 pt;
		float radius;
		float min_val = 100000000.0f;
		for (int i = 0; i < 5; i++) {
			if (ts[i] < 0) continue;
			pt = origin + direction * ts[i];
			if (i == 4) {
				radius = sqrt(pow(pt.x, 2) + pow(pt.z, 2));
				if (radius > 0.5f) {
					continue; // if radius is too large, continue		
				}
			}
			else {
				if (pt.y > 0.5f || pt.y < -0.5f) continue;
			}
			if (ts[i] < min_val) min_val = ts[i];
		}
		if (min_val < 100000000.0f) return min_val;
		else return -1.0f;
	}

	__device__ static const glm::vec3 get_norm(glm::vec3 eyePointP, glm::vec3 rayV, float t, glm::mat4 inverseTransform) { //glm::mat4 transformMatrix,
		glm::vec3 objectNormal = inverseTransform * glm::vec4(eyePointP, 1.0f) + t * inverseTransform * glm::vec4(rayV, 0.0f);

		if (IN_RANGE(objectNormal.y, -0.5f)) {
			objectNormal = glm::vec3(0.0f, objectNormal.y, 0.0f);
		}
		else {
			float length = sqrt(pow(objectNormal.x, 2) + pow(objectNormal.z, 2));
			objectNormal.x = (objectNormal.x / length);
			objectNormal.z = (objectNormal.z / length);
			objectNormal.y = 0.5;
		}
		glm::mat3 transform = glm::transpose(inverseTransform);
		glm::vec3 worldNormal = glm::normalize(transform * glm::vec4(objectNormal, 0.0f));
		return worldNormal;
	}


	__device__ static glm::vec2 getUV(glm::vec3 objectPoint) {
		glm::vec2 UV;

		if (IN_RANGE(objectPoint.y, -0.5f)) {
			UV.x = objectPoint.x + 0.5f;
			UV.y = objectPoint.z + 0.5f;
		}
		else {
			float x = objectPoint.x;
			float z = objectPoint.z;

			float theta = atan2(z, x) + PI;
			if (theta < 0) UV.x = -theta / (2.0f * PI);
			else		   UV.x = 1.0f - (theta / (2.0f * PI));
			UV.y = -objectPoint.y + 0.5f;
		}

		return UV;
	}

private:
};

#endif
