#ifndef MOVE_INCLUDED
#define MOVE_INCLUDED

typedef struct {
	float ix, iy; // Start X and Y coords
	union {
		struct {
			float fx, fy; // Final X and Y coords
		};
		struct {
			float sdir, cdir, speed;
		};
	};
	float accel, rol; // Acceleration speed, rotate speed
	float time, current, d_time; // How many seconds
	int type; // Linear, cuadratic, cubic, smoothstep
} move_t;

void InitMove(move_t* move);

void SetMove(move_t* move, float ix, float iy, float fx, float fy, float time, int type);

void SetMoveDir(move_t* move, float ix, float iy, float dir, float speed, float accel, float rol, float time, int type);

void TickMove(move_t* move, float value, float* x, float* y);

#endif