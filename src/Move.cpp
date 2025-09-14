#include "Move.hpp"
#include <string.h>
#include "Common.hpp"
#include <DirectXMath.h>

void InitMove(move_t* move) {
	memset(move, 0, sizeof(move_t));
	move->type = -1;
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

void SetMoveDir(move_t* move, float ix, float iy, float dir, float speed, float accel, float rol, float time, int type) {
	move->ix = ix;
	move->iy = iy;
	DirectX::XMScalarSinCos(&move->sdir, &move->cdir, dir);
	switch (type) {
	case 2: move->sdir = dir; break;
	}
	move->accel = accel;
	move->speed = speed;
	move->rol = dir + rol;
	move->current = 0.0f;
	move->time = time;
	move->d_time = 1.0f / time;
	move->type = type + 0x100;
}

void TickMove(move_t* move, float value, float* x, float* y) {
	if (-1 == move->type) return;
	float time = move->time, val = 0.0f;
	bool on_end = false;
	move->current += value;
	val = move->current;
	if (val >= time) {
		value -= val - time;
		val = time;
		on_end = true;
	}
	float t = val * move->d_time;
	
	switch (move->type) {
	default:
		if (0x100 <= move->type) {
			goto move_dir;
		}
		break;
	case 0: break; // Linear
	case 1:	t *= t; break; // Cuadratic
	case 2:	t *= t * t; break; // Cubic
	case 3:	t *= t * t * t; break; // Quad?

	case 4:	t *= t * t * t; break; 
	case 5:	t *= t * t * t; break;
	case 6:	t *= t * t * t; break;

	case 7:	t = 3.0f * t * t - 2.0f * t * t * t; break; // Smoothstep
	}

	*x = Lerp(t, move->ix, move->fx);
	*y = Lerp(t, move->iy, move->fy);

	goto end;
move_dir:
	switch (move->type - 0x100) {
	case 0: // Move at constant speed
		*x = Lerp(t, move->ix, move->ix + move->time * move->speed * move->cdir);
		*y = Lerp(t, move->iy, move->iy + move->time * move->speed * move->sdir);
		break;
	case 1: // Move with acceleration
	{
		float factor = move->speed * val + 0.5 * move->accel * val * val;
		*x = move->ix + factor * move->cdir;
		*y = move->iy + factor * move->sdir;
	}
		break;
	case 2: //
	{
		float s, c;
		DirectX::XMScalarSinCos(&s, &c, Lerp(t, move->sdir, move->rol));
		move->ix += move->speed * value * c;
		move->iy += move->speed * value * s;
		*x = move->ix;
		*y = move->iy;
	}
		break;
	}
end:
	if (on_end) {
		move->type = -1;
	}
	return;
}