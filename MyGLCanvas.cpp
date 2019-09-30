#include "MyGLCanvas.h"
#include "cuda_wrapper_implementation.h"
//#include "xml_generator.h"
#define NUM_OPENGL_LIGHTS 8
using namespace std;
MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) 
{
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	width = screenWidth;
	height = screenHeight;

	pixels = new GLubyte[width  * height * 3];
	memset(pixels, 0, width  * height * 3);

	numPrimitives = 0;

	camera = new Camera();
	
	//xml_generator::generate_xml();	
	string filename;
	cout << "[chessboard.xml, cones.xml, cornell_box.xml, dk_side_moon.xml, mirror_test1.xml, reflective_cubes.xml, robot.xml, shadow_test.xml, spheres.xml, spheres_test.xml'] \nenter a filename: ";
	cin >> filename; cout << endl;	
	loadSceneFile(("./data/" + filename).c_str());
	setPixels();				
}

MyGLCanvas::~MyGLCanvas() 
{
	if (pixels != nullptr) {
		delete[]pixels;
		pixels = nullptr;
	}

	if (primitiveList != nullptr) {				
		delete[]primitiveList;
		primitiveList = nullptr;
	}
	
	for (int i = 0;i < nodes.size();i++) {
		delete nodes.at(i);
		nodes.at(i) = nullptr;
	}
	delete parser;
	parser = nullptr;
}


void MyGLCanvas::loadSceneFile(const char* filenamePath) 
{
	if (parser != NULL) {
		delete parser;
		parser = nullptr;
	}
	
	parser = new SceneParser(filenamePath);
	bool success = parser->parse();

	while (success == false) {	
		string fname;
		cout << "no success. enter a new filename: ";
		cin >> fname; 
		filenamePath = ("./data/" + fname).c_str();
		delete parser;
		parser = new SceneParser(filenamePath);
		success = parser->parse();			
	}	
	cout << "success! " << endl;
	SceneCameraData cameraData;
	parser->getCameraData(cameraData);
	eyePosition = cameraData.pos;

	camera->reset();		
	if (cameraData.isDir == true) {
		camera->orientLookVec(cameraData.pos, cameraData.look, cameraData.up);
	}
	else {
		camera->orientLookAt(cameraData.pos, cameraData.lookAt, cameraData.up);
	}
	
	//get the primitives as usual
	nodes.clear();
	if (numPrimitives > 0) {
		delete[] primitiveList;
		primitiveList = nullptr;
		numPrimitives = 0;
	}

	glm::mat4 compositeMatrix = glm::mat4(1.0f);
	SceneNode* root = parser->getRootNode();
	recurse_levels(root, compositeMatrix);	

	//now we have all the primitives with transformation matrices. save as a flattened ary (for gpu)
	//while we're at it, save the objects as primitives, w/ rgb values put into vec3s - makes lighting calcs easy later on. 
	primitiveList = new primitive[numPrimitives];		
	int currNode = 0;
	for (int i = 0;i < nodes.size();i++) {		
		for (int j = 0;j < nodes.at(i)->primitives.size();j++) {
			primitiveList[currNode] = primitive(nodes.at(i)->primitives.at(j));
			currNode++;			
		}
	}	
}

//determines the colors via cuda, and set the pixels
void MyGLCanvas::setPixels() {		
	SceneGlobalData SGD;
	parser->getGlobalData(SGD);		
	glm::vec3 *colors = cudaWrapper(camera, primitiveList, numPrimitives, parser->lightList, parser->numLights, SGD); 
	int r, g, b;
	size_t pixel_index;
	for (int j = height - 1; j >= 0; j--) {
		for (int i = 0; i < width; i++) {
			pixel_index = j * width + i;
			r = min(255, (int)(255.0f * colors[pixel_index].x));
			g = min(255, (int)(255.0f * colors[pixel_index].y));
			b = min(255, (int)(255.0f * colors[pixel_index].z));			
			setpixel(pixels, i, j, r, g, b);
		}
	}
	delete[]colors;
	colors = nullptr;
}

//Given the pixel (x, y) position, set its color to (r, g, b)
void MyGLCanvas::setpixel(GLubyte* buf, int x, int y, int r, int g, int b) {
	int pixelWidth = width;
	buf[(y*pixelWidth + x) * 3 + 0] = (GLubyte)r;
	buf[(y*pixelWidth + x) * 3 + 1] = (GLubyte)g;
	buf[(y*pixelWidth + x) * 3 + 2] = (GLubyte)b;
}

int MyGLCanvas::handle(int e) {		
	switch (e) {
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
		
	case FL_KEYDOWN:	
		switch (Fl::event_key()) {
		case 'w': eyePosition.z -= 0.05f;  break;
		case 'a': eyePosition.x -= 0.05f; break;
		case 's': eyePosition.z += 0.05f;  break;
		case 'd': eyePosition.x += 0.05f; break;
		}
		camera->setEyePoint(eyePosition);
		setPixels();		
		redraw();
		return(1);	
	}

	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::draw() {
	if (!valid()) {  
		glViewport(0, 0,screenWidth, screenHeight);	
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (parser == NULL) {
		return;
	}

	if (pixels == NULL) {
		return;
	}
	
	//draw the pixels!
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

//below are the functions to recursively calculate the composite matrices for the objects in the .xml files

//calculate the transformations on the composite matrix at the current scenenode and return the finished product
//note: don't pass by reference b/c the value would be shifting around in the recursive calls. 
glm::mat4 MyGLCanvas::calc_transformations(SceneNode *currNode, glm::mat4 compositeMatrix) {
	SceneTransformation* currTransform;
	glm::mat4 translate, scale, rotate, mat;
	for (int i = 0; i < currNode->transformations.size(); i++) {
		currTransform = currNode->transformations[i];
		if (currTransform->type == TRANSFORMATION_TRANSLATE) {
			translate = glm::mat4(1.0f);
			translate[3][0] = currTransform->translate.x;
			translate[3][1] = currTransform->translate.y;
			translate[3][2] = currTransform->translate.z;
			compositeMatrix = compositeMatrix * translate;
		}
		else if (currTransform->type == TRANSFORMATION_ROTATE) {
			if (glm::length(currTransform->rotate) > 1e-4) {
				rotate = glm::rotate(glm::mat4(1.0f), currTransform->angle, currTransform->rotate);
				compositeMatrix = compositeMatrix * rotate;
			}
		}
		else if (currTransform->type == TRANSFORMATION_SCALE) {
			scale = glm::mat4(1.0f);
			float viewAngle = 60.0f;
			float farPlane = 20.0f;
			float nearPlane = 0.01f;
			scale[0][0] = currTransform->scale.x;
			scale[1][1] = currTransform->scale.y;
			scale[2][2] = currTransform->scale.z;
			compositeMatrix = compositeMatrix * scale;
		}
		else if (currTransform->type == TRANSFORMATION_MATRIX) {
			mat = currTransform->matrix;
			compositeMatrix = compositeMatrix * mat;
		}
	}
	return compositeMatrix;
}

//we've found a base case of the recursion, so save the set of primitives alongside their transformation matrix
void MyGLCanvas::save_primitives(SceneNode *currNode, const glm::mat4 compositeMatrix) {
	vector<ScenePrimitive*> primitives;
	ScenePrimitive *currPrim;
	//there could hypothetically be multiple objects in exactly the same space (i.e. circle&square) with the same #of transformations.	
	for (int i = 0; i < currNode->primitives.size(); i++) {	
		currPrim = currNode->primitives.at(i);
		currPrim->transformationMatrix = compositeMatrix;
		currPrim->inverseTransform = glm::inverse(compositeMatrix);
		primitives.push_back(currPrim);
		numPrimitives++; //needed for flattening later on (for the gpu).
	}
	node *n = new node(compositeMatrix, primitives);
	nodes.push_back(n);
}

void MyGLCanvas::recurse_levels(SceneNode *currNode, const glm::mat4 compositeMatrix) {
	glm::mat4 updated_composite_matrix = calc_transformations(currNode, compositeMatrix);
	for (int i = 0; i < currNode->children.size();i++) {
		recurse_levels(currNode->children[i], updated_composite_matrix);
	}
	//save the primitives if there are any to save
	if (currNode->primitives.size() > 0) {
		save_primitives(currNode, updated_composite_matrix);
	}
}
