#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#define RAD(x) (x * 180.0 / 3.14159)

constexpr inline float Lerp(float t, float a, float b) {
	return t * a + (1 - t) * b;
}

#endif