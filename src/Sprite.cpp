#include "Sprite.hpp"

void SetupSprite(TLVertex2D* vertex, float x, float y, sprite_t* sprite) {
	float hw = 0.5f * sprite->width, hh = 0.5f * sprite->height;
	float s = sprite->dirs, c = sprite->dirc;

	vertex[0].x = c * -hw - s * -hh + x;
	vertex[0].y = s * -hw + c * -hh + y;
	vertex[0].u = sprite->uv.u0;
	vertex[0].v = sprite->uv.v0;
	vertex[0].color = sprite->color;

	vertex[1].x = c * -hw - s * hh + x;
	vertex[1].y = s * -hw + c * hh + y;
	vertex[1].u = sprite->uv.u0;
	vertex[1].v = sprite->uv.v1;
	vertex[1].color = sprite->color;

	vertex[2].x = c * hw - s * -hh + x;
	vertex[2].y = s * hw + c * -hh + y;
	vertex[2].u = sprite->uv.u1;
	vertex[2].v = sprite->uv.v0;
	vertex[2].color = sprite->color;

	vertex[3].x = c * hw - s * hh + x;
	vertex[3].y = s * hw + c * hh + y;
	vertex[3].u = sprite->uv.u1;
	vertex[3].v = sprite->uv.v1;
	vertex[3].color = sprite->color;
}

void SetUV(quad_t* quad, float u0, float u1, float v0, float v1) {
	quad->u0 = u0;
	quad->u1 = u1;
	quad->v0 = v0;
	quad->v1 = v1;
}