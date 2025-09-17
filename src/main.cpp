#include <stdio.h>
#include <stdlib.h>
#include <DirectXMath.h>

#include "XPFW.h"
#include "Sprite.hpp"
#include "Enemy.hpp"
#include "Face.hpp"
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
	GLuint meiling_face_tex, meiling_face_vb, meiling_face_va;
	float var;

	enemy_t meiling;
	face_t face;
} GameData;

void Initialize(GameData* game_data) {
	// Default 2D shader
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	game_data->flat = pf;

	// Test enemy
	LoadTextureFromFile("boss10.png", &game_data->meiling_tex, nullptr);
	CreateTL2DVertexBuffer(4, nullptr, GL_DYNAMIC_DRAW, &game_data->meiling_vb, &game_data->meiling_va);
	game_data->var = 0.0f;
	InitEnemy(&game_data->meiling, 100, 320.0f, 240.0f, 0.0f, game_data->meiling_tex);
	SetSize(&game_data->meiling.sprite, 64.0f, 64.0f);
	SetDirection(&game_data->meiling.sprite, RAD(45.0f));
	SetUV(&game_data->meiling.sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);

	// Test face
	LoadTextureFromFile("face06a.png", &game_data->meiling_face_tex, nullptr);
	CreateTL2DVertexBuffer(4, nullptr, GL_DYNAMIC_DRAW, &game_data->meiling_face_vb, &game_data->meiling_face_va);
	game_data->var = 0.0f;
	InitFace(&game_data->face, 640.0f - 64.0f, 480.0f - 128.0f, 0.0f, game_data->meiling_face_tex);
	SetSize(&game_data->face.sprite, 128.0f, 256.0f);
	SetDirection(&game_data->face.sprite, RAD(0.0f));
	SetUV(&game_data->face.sprite.uv, 0.0f, 0.5f, 0.0f, 1.0f);

	// Initialize font render
	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);

	// Other data
	game_data->past_time = 0.0f;
	game_data->all_time = 0.0f;
}

void Move(window_t* window, float delta_time, void* data) {
	GameData* game_data = (GameData*)data;
	enemy_t* meiling = &game_data->meiling;
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_1)) {
		SetUV(&meiling->sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_2)) {
		SetUV(&meiling->sprite.uv, 0.25f, 0.5f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_3)) {
		SetUV(&meiling->sprite.uv, 0.5f, 0.75f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_4)) {
		SetUV(&meiling->sprite.uv, 0.75f, 1.00f, 0.0f, 0.25f);
	}

	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_Q)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 220.0f, 100.0f, 1.2f, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_W)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 420.0f, 350.0f, 0.75f, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_E)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 30.0f, 410.0f, 1.0f, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_S)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 320.0f, 0.0f, 2.0f, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_D)) {
		SetMoveDir(&meiling->move, meiling->x, meiling->y, RAD(90.0f), 300.0f, -300.0f, 0.0f, 1.0f, 1);
	}

	TickMove(&meiling->move, delta_time, &meiling->x, &meiling->y);
	// Do stuff
	//game_data->meiling.y = 240.0f + DirectX::XMScalarSin(game_data->var * 2.0f) * 30.0f;
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

	// Enemy
	SetupSprite(vertex, game_data->meiling.x, game_data->meiling.y, &game_data->meiling.sprite);
	glUnmapNamedBuffer(game_data->meiling_vb);

	glBindTextureUnit(0, game_data->meiling.sprite.texture);
	glBindVertexArray(game_data->meiling_va);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Face
	vertex = (TLVertex2D*)glMapNamedBuffer(game_data->meiling_face_vb, GL_WRITE_ONLY);

	SetupSprite(vertex, game_data->face.x, game_data->face.y, &game_data->face.sprite);
	glUnmapNamedBuffer(game_data->meiling_face_vb);

	glBindTextureUnit(0, game_data->face.sprite.texture);
	glBindVertexArray(game_data->meiling_face_va);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Text
	sprintf(buf, "%.2f fps", window->fps);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
	sprintf(buf, "Time passed: %.2f s", game_data->all_time);
	DrawString(game_data->font, 0, 24, buf, 0xffffffff);
	sprintf(buf, "X = %.3f", game_data->meiling.x);
	DrawString(game_data->font, 0, 48, buf, 0xffffffff);
	sprintf(buf, "Y = %.3f", game_data->meiling.y);
	DrawString(game_data->font, 0, 72, buf, 0xffffffff);

	sprintf(buf, "Dir = %.3f", atan2f(game_data->meiling.move.sdir, game_data->meiling.move.cdir));
	DrawString(game_data->font, 0, 288, buf, 0xffffffff);
	sprintf(buf, "DirS / FinalX = %.3f", game_data->meiling.move.sdir);
	DrawString(game_data->font, 0, 312, buf, 0xffffffff);
	sprintf(buf, "DirC / FinalY = %.3f",game_data->meiling.move.cdir);
	DrawString(game_data->font, 0, 336, buf, 0xffffffff);
	sprintf(buf, "Speed = %.3f", game_data->meiling.move.speed);
	DrawString(game_data->font, 0, 360, buf, 0xffffffff);
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
