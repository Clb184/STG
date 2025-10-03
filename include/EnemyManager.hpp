#ifndef ENEMYMANAGER_INCLUDED
#define ENEMYMANAGER_INCLUDED

#include "Enemy.hpp"
#include "List.hpp"

const int MAX_ENEMY = 256;

typedef struct {
	list_t<enemy_t, MAX_ENEMY> enemy_list;
	GLuint tex_list[MAX_ENEMY];
	int count;
	int num_draw;
	GLuint vertex_buffer;
	GLuint vertex_array;
} enemy_manager_t;

void InitEnemyManager(enemy_manager_t* enm_manager);

void DestroyEnemyManager(enemy_manager_t* enm_manager);

enemy_t* AddEnemy(enemy_manager_t* enm_manager, int hp, float x, float y, float dir, GLuint texture);

void DrawEnemies(enemy_manager_t* enm_manager);

#endif