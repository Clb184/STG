#ifndef TEXTUREMANAGER_INCLUDED
#define	TEXTUREMANAGER_INCLUDED

#include "OpenGL/Texture.h"

#define MAX_TEXTURES 128

struct texture_manager_t {
	int cnt; // How many textures
	texture_metric_t texture_metrics[MAX_TEXTURES]; // Metrics for textures
	GLuint textures[MAX_TEXTURES]; // Texture ids for OpenGL
};

void InitTextureManager();
GLuint RegisterTexture(const char* source);
void DestroyTextureManager();

extern texture_manager_t tex_manager;

#endif