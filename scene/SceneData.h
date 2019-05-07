/*!
   @file   SceneData.h
   @author Nong Li (Fall 2008)
   @author Remco Chang (Dec 2013)

   @brief  Header file containing scene data structures.
*/

#ifndef SCENEDATA_H
#define SCENEDATA_H

/* Includes */
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../shape.h"
#include "ppm.h"

using namespace std;

/*
class Face {

public:

	Face(float left, float right, float top, float bottom) {
		this->left = left;
		this->right = right;
		this->top = top;
		this->bottom = bottom;
	};
	Face() {};
	~Face() {};

	bool inFace(glm::vec3 point) {
		return (point.x >= left && point.x <= right && point.y <= top && point.y >= bottom);
	};
private:
	float left, right, top, bottom;
};
 */

//! Enumeration for light types.
enum LightType {
   LIGHT_POINT, LIGHT_DIRECTIONAL, LIGHT_SPOT, LIGHT_AREA
};

//! Enumeration for types of primitives that can be stored in a scene file.
//enum PrimitiveType {
//	SHAPE_CUBE = 0,
//	SHAPE_CYLINDER = 1,
//	SHAPE_CONE = 2,
//	SHAPE_SPHERE = 3,
//	SHAPE_SPECIAL1 = 4,
//	SHAPE_SPECIAL2 = 5,
//	SHAPE_SPECIAL3 = 6,
//	SHAPE_MESH = 7
//};


//! Enumeration for types of transformations that can be applied to objects, lights, and cameras.
enum TransformationType {
   TRANSFORMATION_TRANSLATE, TRANSFORMATION_SCALE, 
   TRANSFORMATION_ROTATE, TRANSFORMATION_MATRIX
};

//! Struct to store a RGBA color in floats [0,1]
class SceneColor 
{
public:
   // union {
    //    struct {
	float r;
    float g;
    float b;
    float a;
    //    };
     //   float channels[4]; // points to the same four floats above...
	void operator =(const SceneColor &s) {
		this->r = s.r;
		this->g = s.g;
		this->b = s.b;
		this->a = s.a;
	}

   // @TODO: [OPTIONAL] You can implement some operators here for color arithmetic.

};

//! Scene global color coefficients
class SceneGlobalData 
{
public:
   float ka;  //! global ambient coefficient
   float kd;  //! global diffuse coefficient
   float ks;  //! global specular coefficient
   float kt;  //! global transparency coefficient
   void operator =(SceneGlobalData &d) {
	   this->ka = d.ka;
	   this->kd = d.kd;
	   this->ks = d.ks;
	   this->kt = d.kt;
   }
};

//! Data for a single lightclass SceneLightData 
class SceneLightData
{
public:
	int id;
	LightType type;
	SceneColor color;
	glm::vec3 function;
	glm::vec3 pos;        //! Not applicable to directional lights
	glm::vec3 dir;        //! Not applicable to point lights
	glm::vec3 col;
		
	void operator =(SceneLightData &d) {
		this->id = d.id;
		this->type = d.type;
		this->color = d.color;
		this->function = d.function;
		this->pos = d.pos;
		this->dir = d.dir;
		this->col = glm::vec3(d.color.r, d.color.g, d.color.b);
	};
};


//! Data for scene camera
class SceneCameraData
{
public:
   glm::vec3 pos;
   glm::vec3 lookAt;
   glm::vec3 look;
   glm::vec3 up;

   bool isDir;  //true if using look vector, false if using lookAt point

   float heightAngle;
   float aspectRatio;

   float aperture;      //! Only applicable for depth of field
   float focalLength;   //! Only applicable for depth of field
};

//! Data for file maps (ie: texture maps)
class SceneFileMap
{
public:
   bool isUsed;
   string filename;
   float repeatU;
   float repeatV;
   ppm *ppm_obj;
   void operator =(SceneFileMap &s) {
	   this->isUsed = s.isUsed;
	   this->filename = s.filename;
	   this->repeatU = s.repeatU;
	   this->repeatV = s.repeatV;
	   this->ppm_obj = s.ppm_obj;
   }
};

//! Data for scene materials
class SceneMaterial
{
public:
	SceneColor cDiffuse;
	SceneColor cAmbient;
	SceneColor cReflective;
	SceneColor cSpecular;
	SceneColor cTransparent;
	SceneColor cEmissive;

	SceneFileMap* textureMap;
	float blend;

	SceneFileMap* bumpMap;

	float shininess;

	float ior;   //! index of refaction
	int textureNum;
	void operator = (SceneMaterial &s) {
		this->cDiffuse = s.cDiffuse;
		this->cAmbient = s.cAmbient;
		this->cReflective = s.cReflective;
		this->cSpecular = s.cSpecular;
		this->cTransparent = s.cTransparent;
		this->cEmissive = s.cEmissive;
		this->textureMap = s.textureMap;
		this->blend = s.blend;
		this->bumpMap = s.bumpMap;
		this->shininess = s.shininess;
		this->textureNum = s.textureNum;
	}
};

//! Data for a single primitive.
class ScenePrimitive
{
public:
   OBJ_TYPE type;
   string meshfile;     //! Only applicable to meshes
   SceneMaterial material;
   glm::mat4 transformationMatrix;
   glm::mat4 inverseTransform;
   void operator =(ScenePrimitive &p) {
	   this->type = p.type;
	   this->meshfile = p.meshfile;
	   this->material = p.material;
	   this->transformationMatrix = p.transformationMatrix;
	   this->inverseTransform = p.inverseTransform;
   }
};

/*!

@struct CS123SceneTransformation
@brief Data for transforming a scene object.

  Aside from the TransformationType, the remaining of the data in the
  struct is mutually exclusive

*/
class SceneTransformation
{
public:
   TransformationType type;

   /*! Translate type */
   glm::vec3 translate;

   /*! Scale type */
   glm::vec3 scale;

   /*! Rotate type */
   glm::vec3 rotate;
   float angle;        //! the angle of rotation, in **radians**

   /*! Matrix type */
   glm::mat4 matrix;
};

//! Structure for non-primitive scene objects
class SceneNode
{
public:

   /*! Transformation at this node */
   std::vector<SceneTransformation*> transformations;

   /*! Primitives at this node */
   std::vector<ScenePrimitive*> primitives;

   /*! Children of this node */
   std::vector<SceneNode*> children;
};

#endif

