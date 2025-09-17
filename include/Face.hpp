#ifndef FACE_INCLUDED
#define FACE_INCLUDED

#include "Sprite.hpp"
#include "Move.hpp"

typedef struct {
	float x;
	float y;
	move_t move;
	sprite_t sprite;
} face_t;

void InitFace(face_t* face, float x, float y, float dir, GLuint texture);

#endif