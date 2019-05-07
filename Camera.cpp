#include "Camera.h"
#include<stdio.h>

Camera::Camera() {
	reset();
}

Camera::~Camera() {
}

void Camera::reset() {	
	orientLookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
}

void Camera::setEyePoint(glm::vec3 eyePoint) {	
	orientLookAt(eyePoint, look_vector,up_vector);		
}
//orientLookAt
// this function is basically the same as orientLookVec, but with a point instead of a vector.
// So, calculate the vector based on the point and eyepoint, and then call orientLookVec.
void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint, glm::vec3 upVec) {
	glm::vec3 lookVec = glm::normalize(lookatPoint - eyePoint);
	orientLookVec(eyePoint, lookVec, upVec);
}

//orientLookVec
// this function sets the private member variables eye_point, look_vector, and up_vector.
// it then calculates the translation matrix, determines the basis vectors for camera world based
// on the provided look vector and up vectors, and then calculates the rotation matrix to rotate 
// the world toward the lookVector supplied
void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec) {	
	eye_point = eyePoint;		
	look_vector = lookVec;
	up_vector = upVec;
	
	calc_translation_matrix(eye_point);
	calc_basis_vectors(lookVec, upVec);
	calc_rotation_matrix();
}

//calc_basis_vectors
// this function calculates the correct basis vectors for camera space, based on a look vector and up vector
void Camera::calc_basis_vectors(glm::vec3 lookVec, glm::vec3 upVec) {
	//establish the basis vectors of the view space
	w = glm::normalize(-1.0f * lookVec);	//inverse look vector
	u = glm::normalize(glm::cross(upVec, w));
	v = glm::cross(w, u);
}

//calc_rotation_matrix
// this function assumes that the uvw basis vectors have already been calculated
// it updates the rotation matrix to rotate the camera to look in the right direction
void Camera::calc_rotation_matrix() {
	//establish the rotation matrix		
	glm::vec3 xyz_array[3] = { x, y, z };
	glm::vec3 uvw_array[3] = { u, v, w };

	rotation_matrix = glm::mat4(1.0f);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3;j++) {
			rotation_matrix[j][i] = glm::dot(uvw_array[i], xyz_array[j]);
		}
	}
}

//calc_translation_matrix
// this function calculates the translation matrix (move negatively in x,y,z by the vector v)
// to move the world away from the eye (E) of the camera.
// it's basically the same as 'translate', but is named more conveniently.
void Camera::calc_translation_matrix(glm::vec3 E) {
	translation_matrix = glm::mat4(1.0f);
	translation_matrix[3][0] = -E[0];
	translation_matrix[3][1] = -E[1];
	translation_matrix[3][2] = -E[2];
}

//getModelViewMatrix()
// returns the modelviewmatrix. 
// weird thing happening - to make the same as GLM::lookAt, have to use transpose (why?)
glm::mat4 Camera::getModelViewMatrix(){
	return rotation_matrix * translation_matrix;
}

//getScaleMatrix()
//NEAR_PLANE 0.01f
//#define FAR_PLANE 20.0f
//#define VIEW_ANGLE 60.0f
// caculates the scaling matrix to bring the camera pyramid in the correct dimensions
// (-1 to 1 for x and y, 0 to -1 for z)
glm::mat4 Camera::getScaleMatrix(){
	//float f = farPlane;
	scaling_matrix = glm::mat4(1.0f);
	scaling_matrix[0][0] = 0.0866022f; //1.0f/(tan(glm::radians(viewAngle) / 2.0f) * farPlane
	scaling_matrix[1][1] = 0.0866022f;//1.0f / (tan(glm::radians(viewAngle) / 2.0f) * ((float)screenWidth / (float)screenHeight) * farPlane);
	scaling_matrix[2][2] = 0.05f; //1.0f / f;
	return scaling_matrix;
}

glm::mat4 Camera::getInverseScaleMatrix(){
	//float f = farPlane;
	inverse_scaling_matrix = glm::mat4(1.0f);
	inverse_scaling_matrix[0][0] = 11.5470380f;//tan(glm::radians(viewAngle) / 2.0f) * farPlane;
	inverse_scaling_matrix[1][1] = 11.5470380f;//tan(glm::radians(viewAngle) / 2.0f) * ((float)screenWidth / (float)screenHeight) * farPlane;
	inverse_scaling_matrix[2][2] = 20.0f;
	inverse_scaling_matrix[3][3] = 1.0f;
	return inverse_scaling_matrix;
}
glm::mat4 Camera::getInverseModelViewMatrix(){
	glm::mat4 inverse_translation_matrix = glm::mat4(1.0);
	inverse_translation_matrix[3][0] = eye_point[0];
	inverse_translation_matrix[3][1] = eye_point[1];
	inverse_translation_matrix[3][2] = eye_point[2];
	glm::mat4 inverse_rotation_matrix = glm::transpose(rotation_matrix);

	return inverse_translation_matrix * inverse_rotation_matrix;
}

glm::mat4 Camera::getInverseRotationMatrix(){
	return glm::transpose(rotation_matrix);
}

glm::mat4 Camera::getInverseTranslationMatrix(){
	glm::mat4 inverse_translation_matrix = glm::mat4(1.0);
	inverse_translation_matrix[3][0] = eye_point[0];
	inverse_translation_matrix[3][1] = eye_point[1];
	inverse_translation_matrix[3][2] = eye_point[2];
	return inverse_translation_matrix;
}

glm::vec3 Camera::getEyePoint(){
	return eye_point;
}

glm::vec3 Camera::getLookVector(){
	return look_vector;
}
//fixing for first run
glm::vec3 Camera::getUpVector(){
	return up_vector;
}

int Camera::getScreenWidth(){
	return screenWidth;
}

int Camera::getScreenHeight() {
	return screenHeight;
}
