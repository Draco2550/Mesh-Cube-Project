#include "triangle.h"
#ifndef MESH_H
#define MESH_H


//Define a constant for the num of vertices for the Mesh
#define N_MESH_VERTICES 8 //TODO



//Use "extern" to declare our array of vertices in mesh.h and be able to initialize them in mesh.c

extern vec3_t mesh_vertices[N_MESH_VERTICES];

//6 cube faces, 2 triangles per faces
#define N_MESH_FACES (12)

extern face_t mesh_faces[N_MESH_FACES];


#endif // !MESH_H
