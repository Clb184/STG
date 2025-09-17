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
	float width, height;
	float scale_x, scale_y;
	float dirs, dirc;
	quad_t uv;
	uint32_t color;
} sprite_t;

void InitSprite(sprite_t* sprite);

void SetupSprite(TLVertex2D* vertex, float x, float y, sprite_t* sprite);

// These are just properties
void SetUV(quad_t* quad, float u0, float u1, float v0, float v1);

void SetSize(sprite_t* sprite, float width, float height);

void SetScale(sprite_t* sprite, float width, float height);

void SetDirection(sprite_t* sprite, float angle);

#endif