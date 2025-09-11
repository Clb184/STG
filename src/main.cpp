#include <stdio.h>
#include <stdlib.h>
#include <DirectXMath.h>

#include "XPFW.h"
#include "Sprite.hpp"
#include "Enemy.hpp"
#include "Common.hpp"

typedef struct {
	FT_Library library;
	font_t* font;
	font_descriptor_t font_desc;
	float past_time;
	GLuint flat;
	GLuint volume;
	GLuint meiling_tex, meiling_vb, meiling_va;
	float var;

	sprite_t meiling_sprt;
} GameData;

void Initialize(GameData* game_data) {
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	game_data->flat = pf;

	LoadTextureFromFile("boss10.png", &game_data->meiling_tex, nullptr);
	CreateTL2DVertexBuffer(4, nullptr, GL_DYNAMIC_DRAW, &game_data->meiling_vb, &game_data->meiling_va);
	game_data->var = 0.0f;

	game_data->meiling_sprt = { game_data->meiling_tex, 64.0f, 64.0f, 0.0f, 1.0f, 0.0f, 0.25f, 0.0f, 0.25f, 0xffffffff };

	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);
	game_data->past_time = 0.0f;
}

LOOP_FN(GameMain) {
	const float logic_step = 1.0f / 60.0f;
	GameData* game_data = (GameData*)data;
	glUseProgram(game_data->flat);
	glBindTextureUnit(0, game_data->meiling_tex);
	glBindVertexArray(game_data->meiling_va);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(game_data->meiling_vb, GL_WRITE_ONLY);
	
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_1)) {
		SetUV(&game_data->meiling_sprt.uv, 0.0f, 0.25f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_2)) {
		SetUV(&game_data->meiling_sprt.uv, 0.25f, 0.5f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_3)) {
		SetUV(&game_data->meiling_sprt.uv, 0.5f, 0.75f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_4)) {
		SetUV(&game_data->meiling_sprt.uv, 0.75f, 1.00f, 0.0f, 0.25f);
	}

	float y = 240.0f + DirectX::XMScalarSin(game_data->var * 2.0f) * 30.0f;
	SetupSprite(vertex, 320.0f, y, &game_data->meiling_sprt);

	glUnmapNamedBuffer(game_data->meiling_vb);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (game_data->past_time >= logic_step) {
		// Do stuff
		game_data->past_time = 0.0;
	}

	game_data->var += delta_time;
	game_data->past_time += delta_time;
	char buf[64] = "";
	sprintf(buf, "Delta = %.4f", game_data->past_time);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
	sprintf(buf, "y = %.4f", y);
	DrawString(game_data->font, 0, 24, buf, 0xffffffff);
}

int main() {
	window_t window = { 0 };
	GameData data;
	CreateGLWindow("STG game", 800, 600, false, &window);
	Initialize(&data);
	RunMainLoop(&window, &data, GameMain);
	DestroyGLWindow(&window);
}
