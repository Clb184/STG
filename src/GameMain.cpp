#include "GameMain.hpp"
#include <assert.h>
#include "Common.hpp"
#include "DirectXMath.h"

bool InitializeGameMain(game_main_t* game_main) {
	assert(nullptr != game_main);

	// I'll use a max of 128 surfaces for spritesheets for the lols
	InitTextureManager();
	InitEnemyManager(&game_main->enm_manager);
	InitFaceManager(&game_main->face_manager);
	game_main->step = 1.0f;


	// This is just for test purposes
	GLuint mei = RegisterTexture("boss10.png");
	GLuint facea = RegisterTexture("face06a.png");
	GLuint faceb = RegisterTexture("face06b.png");
	/*
	face_t* face = AddFace(&game_main->face_manager, 160.0f, 480.0f - 128.0f, 0.0f, facea);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.0f, 0.5f, 0.0f, 1.0f);

	face = AddFace(&game_main->face_manager, 160.0f + 128.0f, 480.0f - 128.0f, 0.0f, facea);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.5f, 1.0f, 0.0f, 1.0f);

	face = AddFace(&game_main->face_manager, 160.0f + 256.0f, 480.0f - 128.0f, 0.0f, faceb);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.0f, 0.5f, 0.0f, 1.0f);

	face = AddFace(&game_main->face_manager, 160.0f + 384.0f, 480.0f - 128.0f, 0.0f, faceb);
	SetSize(&face->sprite, 128.0f, 256.0f);
	SetDirection(&face->sprite, RAD(0.0f));
	SetUV(&face->sprite.uv, 0.5f, 1.0f, 0.0f, 1.0f);
	*/
	enemy_t* meiling = AddEnemy(&game_main->enm_manager, 100, 0.0f, 240.0f, 0.0f, mei);
	SetSize(&meiling->sprite, 64.0f, 64.0f);
	SetDirection(&meiling->sprite, RAD(0.0f));
	SetUV(&meiling->sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);
	return true;
}

void DestroyGameMain(game_main_t* game_main) {
	assert(nullptr != game_main);

	// Destroy each one of them
	DestroyEnemyManager(&game_main->enm_manager);
	DestroyFaceManager(&game_main->face_manager);
	DestroyTextureManager();	
}

void MoveGameMain(game_main_t* game_main, GLFWwindow* window) {
	assert(nullptr != game_main);
	float tick = game_main->step;
	enemy_t* meiling = &game_main->enm_manager.enemy_list.nodes[0].data;
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_1)) {
		SetUV(&meiling->sprite.uv, 0.0f, 0.25f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_2)) {
		SetUV(&meiling->sprite.uv, 0.25f, 0.5f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_3)) {
		SetUV(&meiling->sprite.uv, 0.5f, 0.75f, 0.0f, 0.25f);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_4)) {
		SetUV(&meiling->sprite.uv, 0.75f, 1.00f, 0.0f, 0.25f);
	}

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 220.0f - 320.0f, 100.0f, 80, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 420.0f - 320.0f, 350.0f, 40, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_E)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 30.0f - 320.0f, 410.0f, 60, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
		SetMove(&meiling->move, meiling->x, meiling->y, 320.0f - 320.0f, 0.0f, 120, 7);
	}
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
		SetMoveDir(&meiling->move, meiling->x, meiling->y, RAD(90.0f), 300.0f, -300.0f, 0.0f, 60, 1);
	}

	TickMove(&meiling->move, tick, &meiling->x, &meiling->y);
}

void DrawGameMain(game_main_t* game_main) {
	assert(nullptr != game_main);
	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(-320.0f, 320.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);
	DrawEnemies(&game_main->enm_manager);
	DrawFaces(&game_main->face_manager);
}