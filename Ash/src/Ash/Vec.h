#pragma once

#include <VolcaniCore/Core/Math.h>

namespace Ash {

template<typename T>
class Point3 {
public:
	T x, y, z;

public:
	Point3(T x, T y, T z)
		: x(x), y(y), z(z) { }
};

template<typename T>
class Normal3 {
public:
	T x, y, z;

public:
	Normal3(T x, T y, T z)
		: x(x), y(y), z(z) { }

	Normal3<T> FaceForward(Normal3<T> n, Normal3<T> v) {
		// return (Dot(n, v) < 0.0f) ? -n : n;
	}
};

}