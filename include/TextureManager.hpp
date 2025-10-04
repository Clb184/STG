#ifndef TEXTUREMANAGER_INCLUDED
#define	TEXTUREMANAGER_INCLUDED

#include <OpenGL/Texture.h>

struct texture_manager_t {
	int cnt; // How many textures
	texture_metric_t* texture_metrics; // Metrics for textures
	GLuint* textures; // Texture ids for OpenGL
};

void InitTextureManager(texture_manager_t* tex_manager, int max);
GLuint RegisterTexture(texture_manager_t* tex_manager, const char* source);
void DestroyTextureManager(texture_manager_t* tex_manager);

extern texture_manager_t tex_manager;

#endif