#ifndef TEXTUREMANAGER_INCLUDED
#define	TEXTUREMANAGER_INCLUDED

#include "OpenGL/Texture.h"
#include "Common.hpp"
#define MAX_TEXTURES 128

struct texture_manager_t {
	int cnt; // How many textures
	texture_metric_t texture_metrics[MAX_TEXTURES]; // Metrics for textures
	GLuint textures[MAX_TEXTURES]; // Texture ids for OpenGL
};

void InitTextureManager();
GLuint RegisterTexture(const char* source);
void DestroyTextureManager();
void SetUVFromID(quad_t* quad, float x, float y, float w, float h, int tex_id);
void SetTextureFromID(GLuint* tex, int tex_id);

extern texture_manager_t tex_manager;

#endif