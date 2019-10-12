#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif // !_USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "../MathFunctions.h"
#include "../Vector/Vector3.h"
#include "../Matrix/Matrix4x4.h"

namespace mff {
	template<typename T>
	struct Quaternion {
		Quaternion():v(0.0),w(1.0){}
		Quaternion(T x, T y, T z, T w) : v(x,y,z), w(w){}
		Quaternion(const mff::Vector3<T>& v, T w) : v(v), w(w){}
		mff::Vector3<T> v;
		T w;

		mff::Vector3<T> rotate(const mff::Vector3<T>& v) const ;
	};

	template<typename T>
	Quaternion<T> MakeQuaternion(const mff::Vector3<T>& v, T angle) {
		angle *= 0.5;
		T sine = static_cast<T>(sin(angle));
		return Quaternion<T>(Normalize(v) * sine, static_cast<T>(cos(angle)));
	}

	template<>
	inline Quaternion<float> MakeQuaternion(const mff::Vector3<float>& v, float angle) {
		angle *= 0.5f;
		float sine = sinf(angle);
		return Quaternion<float>(Normalize(v) * sine, cosf(angle));
	}

	template<typename T>
	Quaternion<T> MakeQuatVecToVec(const Vector3<T>& begin, const Vector3<T>& end) {
		using vec3 = Vector3<T>;
		const vec3 normBegin = Normalize(begin);
		const vec3 normEnd = Normalize(end);
		if (normBegin == normEnd) {
			return Quaternion<T>(vec3(0.0), 1.0);
		}
		T cos = dot(normBegin, normEnd);
		cos = Clamp(-1.0, 1.0, cos);
		if (cos < -1.0 + MFF_DEPSILON) {
			vec3 axis = cross(vec3(0, 1, 0), normBegin);
			if (axis.LengthSq() < MFF_DEPSILON * MFF_DEPSILON) {
				axis = cross(vec3(1, 0, 0), normBegin);
			}
			axis = Normalize(axis);
			return MakeQuaternion(axis, static_cast<T>(M_PI));
		}
		vec3 axis = Normalize(cross(normBegin, normEnd));
		float cosDev = sqrt((1 + cos) * 0.5);
		float sinDev = sqrt((1 - cos) * 0.5);
		return Quaternion<T>(axis * sinDev, cosDev);
	}

	template<>
	inline Quaternion<float> MakeQuatVecToVec(const Vector3<float>& begin, const Vector3<float>& end) {
		using vec3 = Vector3<float>;
		const vec3 normBegin = Normalize(begin);
		const vec3 normEnd = Normalize(end);
		if (normBegin == normEnd) {
			return Quaternion<float>(vec3(0.0f), 1.0f);
		}
		float cos = dot(normBegin, normEnd);
		cos = Clamp(-1.0f, 1.0f, cos);
		if (FloatEqual(cos,-1.0f)) {
			vec3 axis = cross(vec3(0, 1, 0), normBegin);
			if (axis.LengthSq() < MFF_FEPSILON * MFF_FEPSILON) {
				axis = cross(vec3(1, 0, 0), normBegin);
			}
			axis = Normalize(axis);
			return MakeQuaternion(axis, static_cast<float>(M_PI));
		}
		vec3 axis = cross(normBegin, normEnd);
		if (axis.LengthSq() > MFF_FEPSILON) {
			axis = Normalize(axis);
		}
		float cosDev = sqrtf((1 + cos) * 0.5);
		float sinDev = sqrtf((1 - cos) * 0.5);
		return Quaternion<float>(axis * sinDev, cosDev);
	}

	template<typename T>
	Matrix4x4<T> ConvertToMat(const Quaternion<T>& quat) {
		T xx = quat.v.x * quat.v.x;
		T yy = quat.v.y * quat.v.y;
		T zz = quat.v.z * quat.v.z;

		T xy = quat.v.x * quat.v.y;
		T xz = quat.v.x * quat.v.z;
		T xw = quat.v.x * quat.w;

		T yz = quat.v.y * quat.v.z;
		T yw = quat.v.y * quat.w;

		T zw = quat.v.z * quat.w;

		return Matrix4x4<T>(
			Vector4<T>(1 - 2 * (yy + zz), 2 * (xy - zw), 2 * (xz + yw), 0),
			Vector4<T>(2 * (xy + zw), 1 - 2 * (xx + zz), 2 * (yz - xw), 0),
			Vector4<T>(2 * (xz - yw), 2 * (yz + xw), 1 - 2 * (xx + yy), 0),
			Vector4<T>(0, 0, 0, 1)
			);
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& rhs, const Quaternion<T>& lhs) {
		return Quaternion<T>(
			cross(rhs.v,lhs.v) + rhs.w * lhs.v + lhs.w * rhs.v,
			rhs.w * lhs.w - dot(rhs.v,lhs.v)
			);
	}

	template<typename T>
	inline mff::Vector3<T> Quaternion<T>::rotate(const mff::Vector3<T>& v) const {
		Quaternion<T> vq(v,static_cast<T>(0));
		vq = *this * vq * Quaternion<T>(-this->v, w);
		return vq.v;
	}

	template<typename T>
	void print(const Quaternion<T>& arg) {
		std::cout << "v( x = " << arg.v.x << ", y = " << arg.v.y << ", z = " << arg.v.z << "), w =" << arg.w << std::endl;
	}
} // namespace mff