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
	float all_time;
	GLuint flat;
	GLuint volume;
	GLuint meiling_tex, meiling_vb, meiling_va;
	float var;

	enemy_t meiling;
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
	InitEnemy(&game_data->meiling, 100, 320.0f, 240.0f, 0.0f);
	game_data->meiling.sprite = { game_data->meiling_tex, 64.0f, 64.0f, 0.0f, 1.0f, 0.0f, 0.25f, 0.0f, 0.25f, 0xffffffff };

	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);
	game_data->past_time = 0.0f;
	game_data->all_time = 0.0f;
}

void Move(window_t* window, float delta_time, void* data) {
	GameData* game_data = (GameData*)data;

	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_1)) {
		SetUV(&game_data->meiling.sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_2)) {
		SetUV(&game_data->meiling.sprite.uv, 0.25f, 0.5f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_3)) {
		SetUV(&game_data->meiling.sprite.uv, 0.5f, 0.75f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_4)) {
		SetUV(&game_data->meiling.sprite.uv, 0.75f, 1.00f, 0.0f, 0.25f);
	}
	// Do stuff
	game_data->meiling.y = 240.0f + DirectX::XMScalarSin(game_data->var * 2.0f) * 30.0f;
	game_data->var += delta_time;
	game_data->all_time += delta_time;
}

void Draw(window_t* window, float delta_time, void* data) {
	GameData* game_data = (GameData*)data;
	char buf[64] = "";

	glUseProgram(game_data->flat);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(game_data->meiling_vb, GL_WRITE_ONLY);


	SetupSprite(vertex, game_data->meiling.x, game_data->meiling.y, &game_data->meiling.sprite);
	glUnmapNamedBuffer(game_data->meiling_vb);

	glBindTextureUnit(0, game_data->meiling.sprite.texture);
	glBindVertexArray(game_data->meiling_va);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	sprintf(buf, "%.2f fps", window->fps);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
	sprintf(buf, "Time passed: %.2f s", game_data->all_time);
	DrawString(game_data->font, 0, 24, buf, 0xffffffff);
	sprintf(buf, "Y = %.4f", game_data->meiling.y);
	DrawString(game_data->font, 0, 48, buf, 0xffffffff);
}

LOOP_FN(GameMain) {
	Move(window, delta_time, data);
	Draw(window, delta_time, data);
}

int main() {
	window_t window = { 0 };
	GameData data;
	CreateGLWindow("STG game", 800, 600, false, &window);
	Initialize(&data);
	RunMainLoop(&window, &data, GameMain);
	DestroyGLWindow(&window);
}
