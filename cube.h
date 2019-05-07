#ifndef CUBE_H
#define CUBE_H

class cube {
public:
	__device__ cube() {};
	__device__ ~cube() {};

	__device__ static float hit_face(glm::vec3 eyePoint, glm::vec3 rayV, float bound, int coord) {
		float t = (bound - eyePoint[coord]) / rayV[coord];
		glm::vec3 pt = eyePoint + rayV * t;
		for (int i = 0;i < 3;i++) {
			if (i == coord) continue;
			if (pt[i] > 0.5f || pt[i] < -0.5f) return -1.0f;
		}
		return t;
	};
	__device__ static float hit_plane(glm::vec3 eyePoint, glm::vec3 rayV) {
		float t = (-1.0f - eyePoint[2]) / rayV[2];
		glm::vec3 pt = eyePoint + rayV * t;
		return pt.x;
	};

	__device__ static float hit(glm::vec3 origin, glm::vec3 direction, glm::mat4 inverseTransform) {
		origin = inverseTransform * glm::vec4(origin, 1.0f);
		direction = inverseTransform * glm::vec4(direction, 0.0f);
		float t_min = 100000000.0f;
		float temp_t = -1;
		bool t_is_set = false;
		for (int i = 0;i < 3;i++) {
			for (int j = 0;j < 2; j++) {
				if (j == 0) temp_t = hit_face(origin, direction, 0.5f, i);
				else        temp_t = hit_face(origin, direction, -0.5f, i);
				if (temp_t > 0.0f && temp_t < t_min) {
					t_is_set = true;
					t_min = temp_t;
				}
			}
		}
		if (!t_is_set) return -1.0f;
		return t_min;
	};

	__device__ static glm::vec3 get_norm(glm::vec3 eyePointP, glm::vec3 rayV, float t, glm::mat4 inverseTransform) {
		glm::vec3 objectNormal = inverseTransform * glm::vec4(eyePointP, 1.0f) + t * inverseTransform * glm::vec4(rayV, 0.0f);

		if (IN_RANGE(objectNormal.x, 0.5f)) {
			objectNormal = glm::vec3(0.5f, 0.0f, 0.0f);
		}
		else if (IN_RANGE(objectNormal.x, -0.5f)) {
			objectNormal = glm::vec3(-0.5f, 0.0f, 0.0f);
		}
		else if (IN_RANGE(objectNormal.y, 0.5f)) {
			objectNormal = glm::vec3(0.0f, 0.5f, 0.0f);
		}
		else if (IN_RANGE(objectNormal.y, -0.5f)) {
			objectNormal = glm::vec3(0.0f, -0.5f, 0.0f);
		}
		else if (IN_RANGE(objectNormal.z, 0.5f)) {
			objectNormal = glm::vec3(0.0f, 0.0f, 0.5f);
		}
		else if (IN_RANGE(objectNormal.z, -0.5f)) {
			objectNormal = glm::vec3(0.0f, 0.0f, -0.5f);
		}
		glm::mat3 transform = glm::transpose(inverseTransform);
		glm::vec3 worldNormal = glm::normalize(transform * glm::vec4(objectNormal, 0.0f));
		return worldNormal;
	};


	__device__ static glm::vec2 getUV(glm::vec3 objectPoint) {
		glm::vec2 UV;
		if (IN_RANGE(objectPoint.x, 0.5f)) {
			UV.x = -objectPoint.z;
			UV.y = -objectPoint.y;
		}
		else if (IN_RANGE(objectPoint.x, -0.5f)) {
			UV.x = -objectPoint.z;
			UV.y = -objectPoint.y;
		}
		else if (IN_RANGE(objectPoint.y, 0.5f)) {
			UV.x = objectPoint.x;
			UV.y = -objectPoint.z;
		}
		else if (IN_RANGE(objectPoint.y, -0.5f)) {
			UV.x = objectPoint.x;
			UV.y = -objectPoint.z;
		}
		else if (IN_RANGE(objectPoint.z, 0.5f)) {
			UV.x = objectPoint.x;
			UV.y = -objectPoint.y;
		}
		else if (IN_RANGE(objectPoint.z, -0.5f)) {
			UV.x = objectPoint.x;
			UV.y = -objectPoint.y;
		}

		UV = UV + 0.5f;
		return UV;
	};
};
#endif