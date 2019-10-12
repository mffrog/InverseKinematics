#pragma once

#define MFF_PI 3.1415926535

#define MFF_FEPSILON 0.00001
#define MFF_DEPSILON 0.00000001

bool FloatEqual(float a, float b, float epsilon = MFF_FEPSILON);
bool DoubleEqual(double a, double b, double epsilon = MFF_DEPSILON);

template<typename T>
inline T ToRadian(T degree){
	return degree * MFF_PI / static_cast<T>(180);
}


template<typename T>
T Clamp(T min, T max, T value) {
	if (value < min) {
		return min;
	}
	else if (value > max) {
		return max;
	}
	else {
		return value;
	}
}
