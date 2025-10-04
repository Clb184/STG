#include "TextureManager.hpp"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void InitTextureManager(texture_manager_t* tex_manager, int cnt) {
	assert(nullptr != tex_manager);
	tex_manager->cnt = cnt;
	tex_manager->textures = (GLuint*)malloc(sizeof(GLuint) * cnt);
	memset(tex_manager->textures, 0xffffffff, sizeof(GLuint) * cnt);
	tex_manager->texture_metrics = (texture_metric_t*)calloc(cnt, sizeof(texture_metric_t));
}

void DestroyTextureManager(texture_manager_t* tex_manager) {
	assert(nullptr != tex_manager);
	tex_manager->cnt = 0;
	free(tex_manager->textures);
	free(tex_manager->texture_metrics);
}