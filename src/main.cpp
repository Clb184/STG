#include <stdio.h>
#include <stdlib.h>

#include "XPFW.h"

typedef struct {
	GLuint flat;
	GLuint volume;
} GameData;

void Initialize(GameData* game_data) {
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	game_data->flat = pf;
}

LOOP_FN(GameMain) {
	GameData* game_data = (GameData*)data;
	glUseProgram(game_data->flat);
}

int main() {
	printf("test");
	window_t window = { 0 };
	GameData data;
	CreateGLWindow("STG game", 640, 480, false, &window);
	Initialize(&data);
	RunMainLoop(&window, &data, GameMain);
	DestroyGLWindow(&window);
}
