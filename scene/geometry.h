#ifndef GEOMETRY_H
#define GEOMETRY_H

/*  ============== Vertex ==============
	Purpose: Stores properties of each vertex
	Use: Used in face structure
	==================================== */  
class vertex{
public:
	glm::vec3 position;
};

/*  ============== Face ==============
	Purpose: Store list of vertices that make up a polygon.
	In modern versions of OpenGL this value will always be 3(a triangle)
	Use: Used in Shape data structure.
	==================================== */  
class face {
public:
	int vertexList[3];  // assuming each face has 3 vertices

	//normal vector
	glm::vec3 faceNormal;

	int frontFace;

	// Default constructor
	face() {
		vertexList[0] = -1;
		vertexList[1] = -1;
		vertexList[2] = -1;
		frontFace = 0;
	}
};

/* Edge: Connects two vertices, and two faces. 
 */
class edge{
public:
        int vertices[2];
        int faces[2];
        
        //default constructor
        edge(){
            //these are -1 because 0 would be a meaningful value
			vertices[0] = -1;
			vertices[1] = -1;
            faces[0] = -1;
            faces[1] = -1;
        }
};
#endif
