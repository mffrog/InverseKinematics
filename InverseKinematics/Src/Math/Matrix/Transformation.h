#pragma once
#include "Matrix4x4.h"

namespace mff {
	template<typename T>
	Matrix4x4<T> Transition(const Vector3<T>& position) {
		return Matrix4x4<T>(
			Vector4<T>(1.0f, 0.0f, 0.0f, position.x),
			Vector4<T>(0.0f, 1.0f, 0.0f, position.y),
			Vector4<T>(0.0f, 0.0f, 1.0f, position.z),
			Vector4<T>(0.0f, 0.0f, 0.0f, 1.0f)
			);
	}

	template<typename T>
	Matrix4x4<T> LookTo(const Vector3<T>& position, const Vector3<T>& orientation, const Vector3<T>& up) {
		using vec3 = Vector3<T>;
		const vec3 z = Normalize(orientation);
		const vec3 x = Normalize(cross(up, z));
		const vec3 y = cross(z, x);
		return Matrix4x4<T>(
			Vector4<T>(x, -dot(x, position)),
			Vector4<T>(y, -dot(y, position)),
			Vector4<T>(z, -dot(z, position)),
			Vector4<T>(0.0f, 0.0f, 0.0f, 1.0f)
			);
	}

	template<typename T>
	Matrix4x4<T> LookAtLH(const Vector3<T>& position, const Vector3<T>& target, const Vector3<T>& up) {
		return LookTo(position, target - position, up);
	}

	template<typename T>
	Matrix4x4<T> LookAtRH(const Vector3<T>& position, const Vector3<T>& target, const Vector3<T>& up) {
		return LookTo(position, position - target, up);
	}



	template<typename T>
	Matrix4x4<T> Perspective(T fovy, T aspect, T near, T far) {
		T tanRecip = static_cast<T>(1) / tan(fovy * 0.5);
		T zRate = static_cast<T>(1) / (far - near);
		return Matrix4x4<T>(
			Vector4<T>(tanRecip / aspect, 0, 0, 0),
			Vector4<T>(0, tanRecip, 0, 0),
			Vector4<T>(0, 0, -(far + near) * zRate, -2 * near * far * zRate),
			Vector4<T>(0, 0, -1, 0)
			);

	}
} // namespace mff