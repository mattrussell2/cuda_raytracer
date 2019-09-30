#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <iostream>
#include <algorithm>

#include "Camera.h"
#include "scene/SceneParser.h"
#include "node.h"
#include "MyGLCanvas.h"
#include "screen.h"


class MyGLCanvas : public Fl_Gl_Window {
public:
	glm::vec3 eyePosition;
	
	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	
	void loadSceneFile(const char* filenamePath);	
	void draw();
	void setPixels();
	
	void resize(int x, int y, int w, int h);
private:
	GLubyte* pixels = NULL;
	int handle(int e);
	void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);	
	void save_primitives(SceneNode *currNode, const glm::mat4 compositeMatrix);
	void recurse_levels(SceneNode *currNode, const glm::mat4 compositeMatrix);
	glm::mat4 calc_transformations(SceneNode *currNode, glm::mat4 compositeMatrix);
	int numPrimitives, width, height;

	//dynamically allocated 
	primitive *primitiveList;
	vector<node*> nodes;		
	Camera *camera;
	SceneParser* parser;

};
#endif // !MYGLCANVAS_H
