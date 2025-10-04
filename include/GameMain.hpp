#ifndef GAMEMAIN_INCLUDED
#define GAMEMAIN_INCLUDED

#include "EnemyManager.hpp"
#include "FaceManager.hpp"
#include "TextureManager.hpp"
#include "GLFW/glfw3.h"

struct game_main_t {
	float step;

	enemy_manager_t enm_manager; // Hold and manage enemies
	face_manager_t face_manager; // Hold and manager faces (dialogue)
};

bool InitializeGameMain(game_main_t* game_main);
void DestroyGameMain(game_main_t* game_main);

void MoveGameMain(game_main_t* game_main, GLFWwindow* window);
void DrawGameMain(game_main_t* game_main);

#endif