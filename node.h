#ifndef NODEH
#define NODEH
#include <glm/glm.hpp>
#include "scene/SceneParser.h"

class node {
public:
	node(glm::mat4 transform, vector<ScenePrimitive*> p) {
		transformation_matrix = transform;
		inverse_transform = glm::inverse(transform);
		primitives = p;
	};
	node() {};
	~node() {
		for (int i = 0;i < primitives.size();i++) {
			delete primitives.at(i);
		}
	};
	glm::mat4 transformation_matrix;
	glm::mat4 inverse_transform;
	vector<ScenePrimitive*> primitives;
	void operator = (const node &n) {
		this->transformation_matrix = n.transformation_matrix;
		this->inverse_transform = n.inverse_transform;
		this->primitives = n.primitives;
	}
};

class primitive {
public:	
	primitive(ScenePrimitive *prim){			
		this->inverse_transform = prim->inverseTransform;
		this->type = prim->type;
		this->ambient = glm::vec3(prim->material.cAmbient.r, prim->material.cAmbient.g, prim->material.cAmbient.b);
		this->color = glm::vec3(prim->material.cDiffuse.r, prim->material.cDiffuse.g, prim->material.cDiffuse.b);				
		this->specular = glm::vec3(prim->material.cSpecular.r, prim->material.cSpecular.g, prim->material.cSpecular.b);
		this->reflective = glm::vec3(prim->material.cReflective.r, prim->material.cReflective.g, prim->material.cReflective.b);		
		this->shininess = prim->material.shininess;		
	}
	primitive() {};
	~primitive() {};
	
	glm::mat4 inverse_transform;
	OBJ_TYPE type;
	glm::vec3 ambient, color, specular, reflective;
	float shininess;
	void operator =(const primitive &n) {
		//this->transformation_matrix = n.transformation_matrix;
		this->inverse_transform = n.inverse_transform;
		this->type = n.type;
		this->ambient = n.ambient;
		this->color = n.color;					
		this->specular = n.specular;		
		this->reflective = n.reflective;			
		this->shininess = n.shininess;
	};
};

#endif