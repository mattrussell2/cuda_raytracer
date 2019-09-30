#ifndef CAMERA_H
#define CAMERA_H

#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include<iostream>
#include<string>

#define DEFAULT_FOCUS_LENGTH 1.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f
#define VIEW_ANGLE 60.0f
#define PI glm::pi<float>()


class Camera {
public:
	
	Camera();
	~Camera();

	void reset();
	void orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec);
	void orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec);
	
	void setScreenSize(int _screenWidth, int _screenHeight);
	
	void setEyePoint(glm::vec3 eyePoint);
	glm::mat4 getScaleMatrix();
	glm::mat4 getInverseScaleMatrix();
	glm::mat4 getModelViewMatrix();
	glm::mat4 getInverseModelViewMatrix();
	
	glm::mat4 getInverseRotationMatrix();
	glm::mat4 getInverseTranslationMatrix();
	

	glm::vec3 getEyePoint();
	glm::vec3 getLookVector();
	glm::vec3 getUpVector();
	
	int getScreenWidth();
	int getScreenHeight();	

private:
  float viewAngle, filmPlanDepth;
  float nearPlane, farPlane;
  int screenWidth, screenHeight;
  float screenWidthRatio;
  
  //defined by me	
  void calc_basis_vectors(glm::vec3, glm::vec3);
  void calc_rotation_matrix();
  void calc_translation_matrix(glm::vec3 v);
  glm::vec3 u, v, w;

  glm::vec3 x, y, z;
  
  glm::mat4 unhinging_matrix;
  glm::mat4 scaling_matrix;
  glm::mat4 rotation_matrix;
  glm::mat4 translation_matrix;
  glm::mat4 inverse_scaling_matrix;
  glm::mat4 inverse_modelview_matrix;
  
  glm::vec3 eye_point;
  glm::vec3 look_vector;
  glm::vec3 up_vector;	
};
#endif

