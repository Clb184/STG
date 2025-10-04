#include "TextureManager.hpp"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

texture_manager_t tex_manager; // Hold all texture data

void InitTextureManager() {
	tex_manager.cnt = 0;
	memset(tex_manager.textures, 0xffffffff, sizeof(GLuint) * MAX_TEXTURES);
	memset(tex_manager.texture_metrics, 0x00, sizeof(texture_metric_t) * MAX_TEXTURES);
}

GLuint RegisterTexture(const char* source) {

	int pos = tex_manager.cnt;
	if (pos >= MAX_TEXTURES) return -1;

	GLuint ret = -1;

	LoadTextureFromFile(source, &ret, &tex_manager.texture_metrics[pos]);
	tex_manager.textures[pos] = ret;
	tex_manager.cnt++;
	return ret;
}

void DestroyTextureManager() {
	// Delete all at once
	glDeleteTextures(tex_manager.cnt, tex_manager.textures);

	tex_manager.cnt = 0;
}