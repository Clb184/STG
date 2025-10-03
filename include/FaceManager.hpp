#ifndef FACEMANAGER_INCLUDED
#define FACEMANAGER_INCLUDED

#include "Face.hpp"
#include "List.hpp"

const int MAX_FACES = 16;

typedef struct {
	list_t<face_t, MAX_FACES> face_list;
	GLuint tex_list[MAX_FACES];
	int count;
	int num_draw;
	GLuint vertex_buffer;
	GLuint vertex_array;
} face_manager_t;

void InitFaceManager(face_manager_t* face_manager);

void DestroyFaceManager(face_manager_t* face_manager);

face_t* AddFace(face_manager_t* face_manager, float x, float y, float angle, GLuint texture);

void DrawFaces(face_manager_t* face_manager);

#endif