#include "TextureManager.hpp"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

texture_manager_t tex_manager; // Hold all texture data

void InitTextureManager(texture_manager_t* tex_manager, int max) {
	assert(nullptr != tex_manager);
	tex_manager->cnt = 0;
	tex_manager->textures = (GLuint*)malloc(sizeof(GLuint) * max);
	memset(tex_manager->textures, 0xffffffff, sizeof(GLuint) * max);
	tex_manager->texture_metrics = (texture_metric_t*)calloc(max, sizeof(texture_metric_t));
}

GLuint RegisterTexture(texture_manager_t* tex_manager, const char* source) {
	assert(nullptr != tex_manager);

	int pos = tex_manager->cnt;
	GLuint ret = -1;

	LoadTextureFromFile(source, &ret, &tex_manager->texture_metrics[pos]);
	tex_manager->textures[pos] = ret;
	tex_manager->cnt++;
	return ret;
}

void DestroyTextureManager(texture_manager_t* tex_manager) {
	assert(nullptr != tex_manager);
	tex_manager->cnt = 0;
	free(tex_manager->textures);
	free(tex_manager->texture_metrics);
}