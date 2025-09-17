#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED

#include "Sprite.hpp"
#include "Move.hpp"

typedef struct {
	int life;
	float x;
	float y;
	float time;
	float dir;
	move_t move;
	sprite_t sprite;
} enemy_t;

void InitEnemy(enemy_t* enemy, int hp, float x, float y, float dir, GLuint texture);

#endif