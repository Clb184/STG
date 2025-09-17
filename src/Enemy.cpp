#include "Enemy.hpp"

void InitEnemy(enemy_t* enemy, int hp, float x, float y, float dir, GLuint texture) {
	enemy->life = hp;
	enemy->x = x;
	enemy->y = y;
	enemy->dir = dir;
	enemy->time = 0.0f;
	InitSprite(&enemy->sprite);
	enemy->sprite.texture = texture;
	InitMove(&enemy->move);
}