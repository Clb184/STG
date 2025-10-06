#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#define RAD(x) (x * 3.14159f / 180.0f)

constexpr inline float Lerp(float t, float a, float b) {
	return (1 - t) * a + t * b;
}

typedef struct {
	float u0;
	float u1;
	float v0;
	float v1;
} quad_t;

#endif