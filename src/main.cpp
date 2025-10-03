#include <stdio.h>
#include <stdlib.h>
#include <DirectXMath.h>

#include "XPFW.h"
#include "Sprite.hpp"
#include "Enemy.hpp"
#include "Face.hpp"
#include "Common.hpp"
#include "FaceManager.hpp"
#include "EnemyManager.hpp"

typedef struct {
	FT_Library library;
	font_t* font;
	font_descriptor_t font_desc;
	float past_time;
	float all_time;
	GLuint flat;
	GLuint volume;
	GLuint meiling_tex, meiling_vb, meiling_va;
	GLuint facea, faceb;
	float var;

	face_manager_t face_manager;
	enemy_manager_t enm_manager;

	float lowest_fps;
	float highest_fps;

	float tick_spd;
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

	// Test face
	LoadTextureFromFile("face06a.png", &game_data->facea, nullptr);
	LoadTextureFromFile("face06b.png", &game_data->faceb, nullptr);
	game_data->var = 0.0f;
	InitFaceManager(&game_data->face_manager);
	face_t* face = AddFace(&game_data->face_manager, 160.0f, 480.0f - 128.0f, 0.0f, game_data->facea);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.0f, 0.5f, 0.0f, 1.0f);

	face = AddFace(&game_data->face_manager, 160.0f + 128.0f, 480.0f - 128.0f, 0.0f, game_data->facea);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.5f, 1.0f, 0.0f, 1.0f);

	face = AddFace(&game_data->face_manager, 160.0f + 256.0f, 480.0f - 128.0f, 0.0f, game_data->faceb);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.0f, 0.5f, 0.0f, 1.0f);

	face = AddFace(&game_data->face_manager, 160.0f + 384.0f, 480.0f - 128.0f, 0.0f, game_data->faceb);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.5f, 1.0f, 0.0f, 1.0f);

	// Enemy manager
	InitEnemyManager(&game_data->enm_manager);

	enemy_t* meiling =  AddEnemy(&game_data->enm_manager, 100, 320.0f, 240.0f, 0.0f, game_data->meiling_tex);
	SetSize(&meiling->sprite, 64.0f, 64.0f);
	SetDirection(&meiling->sprite, RAD(0.0f));
	SetUV(&meiling->sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);

	// Initialize font render
	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);

	// Other data
	game_data->past_time = 0.0f;
	game_data->all_time = 0.0f;

	game_data->lowest_fps = 1000.0f;
	game_data->highest_fps = -1000.0f;

	game_data->tick_spd = 1.0f;
}

void Move(window_t* window,  void* data) {
	float delta_time = window->delta_time;
	GameData* game_data = (GameData*)data;
	enemy_t* meiling = &game_data->enm_manager.enemy_list.nodes[0].data;
	float tick = game_data->tick_spd;

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
		SetMove(&meiling->move, meiling->x, meiling->y, 220.0f, 100.0f, 80, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_W)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 420.0f, 350.0f, 40, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_E)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 30.0f, 410.0f, 60, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_S)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 320.0f, 0.0f, 120, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_D)) {
		SetMoveDir(&meiling->move, meiling->x, meiling->y, RAD(90.0f), 300.0f, -300.0f, 0.0f, 60, 1);
	}

	TickMove(&meiling->move, tick, &meiling->x, &meiling->y);
	// Do stuff
	//game_data->meiling.y = 240.0f + DirectX::XMScalarSin(game_data->var * 2.0f) * 30.0f;
	game_data->var += delta_time;
	game_data->all_time += delta_time;
}

void Draw(window_t* window, void* data) {
	GameData* game_data = (GameData*)data;
	char buf[64] = "";

	glUseProgram(game_data->flat);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(game_data->meiling_vb, GL_WRITE_ONLY);

	// Enemy
	DrawEnemies(&game_data->enm_manager);

	// Face
	DrawFaces(&game_data->face_manager);

	// Text
	float fps = GetWindowFPS(window);
	if (fps > game_data->lowest_fps) game_data->lowest_fps = fps;
	else if (fps < game_data->lowest_fps && fps != INFINITY) game_data->highest_fps = fps;
	sprintf(buf, "%.2f fps", fps);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
	sprintf(buf, "Time passed: %.2f s", game_data->all_time);
	DrawString(game_data->font, 0, 24, buf, 0xffffffff);
	/*
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
	*/
}

void Destroy(GameData* game_data) {
	DestroyFaceManager(&game_data->face_manager);
	DestroyEnemyManager(&game_data->enm_manager);
}

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

int main() {
	window_t window = { 0 };
	GameData* data = new GameData;
	CreateGLWindow("STG game", 640, 480, false, &window);
	Initialize(data);
	RunMainLoop(&window, data, Move, Draw);
	Destroy(data);
#ifdef WIN32
	char buf[512];
	sprintf(buf, "Highest FPS: %.2f\n Lowest FPS: %.2f",data->highest_fps, data->lowest_fps);
	//MessageBox(NULL, buf, " ", MB_OK);
#endif
	delete data;
}
