#ifndef CYLINDER_H
#define CYLINDER_H

class cylinder {
public:
	__device__ cylinder() {}
	__device__ static float hit(glm::vec3 origin, glm::vec3 direction, glm::mat4 inverseTransform) {		
		origin = inverseTransform * glm::vec4(origin, 1.0f);
		direction = inverseTransform * glm::vec4(direction, 0.0f);

		float a, b, c, d;
		a = origin.x;
		b = direction.x;
		c = origin.z;
		d = direction.z;
		float ts[4], discriminant;

		discriminant = -4.0f * pow(a, 2.0f) * pow(d, 2.0f) + 8.0f * a * b * c * d - 4.0f * pow(b, 2.0f) * pow(c, 2.0f) + pow(b, 2.0f) + pow(d, 2.0f);
		if (discriminant < 0 || IN_RANGE(0.0f, pow(b, 2) + pow(d, 2))) {
			ts[0] = -1.0f;
			ts[1] = -1.0f;
		}
		else {
			ts[0] = -1.0f * (sqrt(discriminant) + 2.0f * a * b + 2.0f * c * d) / (2.0f * (pow(b, 2.0f) + pow(d, 2.0f)));
			ts[1] = (sqrt(discriminant) - 2.0f * a * b - 2.0f * c * d) / (2.0f * (pow(b, 2.0f) + pow(d, 2.0f)));
		}

		ts[2] = (0.5f - origin.y) / direction.y;
		ts[3] = (-0.5f - origin.y) / direction.y;

		glm::vec3 pt;
		float rad;
		float min_val = 100000000.0f;
		for (int i = 0;i < 4;i++) {
			if (ts[i] < 0) continue;
			pt = origin + direction * ts[i];//eyePointP + rayV
			if (i > 1) {
				rad = sqrt(pow(pt.x, 2) + pow(pt.z, 2));
				if (rad > 0.5f) continue; // if radius is too large, continue		
			}
			else {
				if (pt.y > 0.5f || pt.y < -0.5f) continue;
			}
			if (ts[i] < min_val) min_val = ts[i];
		}
		if (min_val < 100000000.0f) return min_val;
		else return -1.0f;

	};
	__device__ static glm::vec3 get_norm(glm::vec3 eyePointP, glm::vec3 rayV, float t, glm::mat4 inverseTransform) {
		glm::vec3 objectNormal = inverseTransform * glm::vec4(eyePointP, 1.0f) + t * inverseTransform * glm::vec4(rayV, 0.0f);
		if (IN_RANGE(objectNormal.y, 0.5f) || IN_RANGE(objectNormal.y, -0.5f)) {
			objectNormal = glm::vec3(0.0f, objectNormal.y, 0.0f);
		}
		else objectNormal = glm::normalize(glm::vec3(objectNormal.x, 0.0f, objectNormal.z));
		glm::mat3 transform = glm::transpose(inverseTransform);
		glm::vec3 worldNormal = glm::normalize(transform * glm::vec4(objectNormal, 0.0f));
		return worldNormal;
	};

	__device__ static glm::vec2 getUV(glm::vec3 objectPoint) {
		glm::vec2 UV;

		if (IN_RANGE(objectPoint.y, 0.5f) || IN_RANGE(objectPoint.y, -0.5f)) {
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
	};
};



#endif
