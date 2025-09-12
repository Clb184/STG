#ifndef SPRITE_INCLUDED
#define SPRITE_INCLUDED

#include <GL/glew.h>
#include <stdint.h>
#include <Misc/Primitives.h>

typedef struct {
	float u0;
	float u1;
	float v0;
	float v1;
} quad_t;

typedef struct {
	GLuint texture;
	float width;
	float height;
	float dirs, dirc;

	quad_t uv;
	uint32_t color;
} sprite_t;

void InitSprite(sprite_t* sprite);

void SetupSprite(TLVertex2D* vertex, float x, float y, sprite_t* sprite);

void SetUV(quad_t* quad, float u0, float u1, float v0, float v1);

#endif