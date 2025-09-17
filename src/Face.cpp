#include "Face.hpp"
#include <DirectXMath.h>

void InitFace(face_t* face, float x, float y, float dir, GLuint texture) {
	face->x = x;
	face->y = y;
	InitSprite(&face->sprite);
	face->sprite.texture = texture;
	DirectX::XMScalarSinCos(&face->sprite.dirs, &face->sprite.dirc, dir);
}