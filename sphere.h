#ifndef SPHEREH
#define SPHEREH

class sphere  {
public:
	__device__ sphere() {};
	__device__ static float hit(glm::vec3 origin, glm::vec3 direction, glm::mat4 inverseTransform){		
		origin = inverseTransform * glm::vec4(origin, 1.0f);
		direction = inverseTransform * glm::vec4(direction, 0.0f);		
		float radius = 0.5f;

		float a = dot(direction, direction);
		float b = 2.0f * dot(origin, direction);
		float c = dot(origin, origin) - radius * radius;
		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0) return -1.0;
		else
		{
			float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
			float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
			if (t1 < t2) return t1;
			return t2;
		}
	};

	__device__ static const glm::vec3 get_norm(glm::vec3 eyePointP, glm::vec3 rayV, float t, glm::mat4 inverseTransform) {
		glm::vec3 objectNormal = glm::normalize(inverseTransform * glm::vec4(eyePointP, 1.0f) + t * inverseTransform * glm::vec4(rayV, 0.0f));
		glm::mat3 transform = glm::transpose(inverseTransform);
		glm::vec3 worldNormal = glm::normalize(transform * glm::vec4(objectNormal, 0.0f));
		return worldNormal;
	};

	//from wikipedia: stereographic projection
	__device__ static glm::vec2 getUV(glm::vec3 objectPoint) {
		glm::vec2 UV;

		float phi = asin(-objectPoint.y / 0.5f);
		UV.y = phi / PI + 0.5f;

		if (objectPoint.y > 0.499f || objectPoint.y < -0.499f) {
			UV.x = 0.5f;
		}
		else {
			float theta = atan2(objectPoint.z, objectPoint.x) + PI;
			if (theta > 0) UV.x = -theta / (2.0f * PI);
			else		   UV.x = 1.0f - (theta / (2.0f * PI));
		}

		return UV;
	};
};

#endif