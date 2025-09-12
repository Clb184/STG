#ifndef MOVE_INCLUDED
#define MOVE_INCLUDED

typedef struct {
	float ix, iy; // Start X and Y coords
	float fx, fy; // Final X and Y coords
	float x, y; // Calculated coordinates
	float time, current, d_time; // How many seconds
	int type; // Linear, cuadratic, cubic, smoothstep
} move_t;

void InitMove(move_t* move);

void SetMove(move_t* move, float ix, float iy, float fx, float fy, float time, int type);

void TickMove(move_t* move, float value);

#endif