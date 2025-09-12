#include "Move.hpp"
#include <string.h>
#include "Common.hpp"

void InitMove(move_t* move) {
	memset(move, 0, sizeof(move_t));
}

void SetMove(move_t* move, float ix, float iy, float fx, float fy, float time, int type) {
	move->ix = ix;
	move->iy = iy;
	move->fx = fx;
	move->fy = fy;
	move->current = 0.0f;
	move->time = time;
	move->d_time = 1.0f / time;
	move->type = type;
}

void TickMove(move_t* move, float value) {
	float time = move->time, val = 0.0f;
	move->current += value;
	val = move->current;
	if (val >= time) {
		val = time;
	}
	float t = val * move->d_time;
	
	switch (move->type) {
	default:
	case 0: break; // Linear
	case 1:	t *= t; break; // Cuadratic
	case 2:	t *= t * t; break; // Cubic
	case 3:	t *= t * t * t; break; // Quad?

	case 4:	t *= t * t * t; break; 
	case 5:	t *= t * t * t; break;
	case 6:	t *= t * t * t; break;

	case 7:	t = 3.0f * t * t - 2.0f * t * t * t; break; // Smoothstep
	}

	move->x = Lerp(t, move->ix, move->fx);
	move->y = Lerp(t, move->iy, move->fy);
}