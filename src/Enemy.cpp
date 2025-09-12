#include "Enemy.hpp"

void InitEnemy(enemy_t* enemy, int hp, float x, float y, float dir) {
	enemy->life = hp;
	enemy->x = x;
	enemy->y = y;
	enemy->dir = dir;
	enemy->time = 0.0f;
	enemy->sprite = { 0 };
	InitMove(&enemy->move);
}