#pragma once

#include "Vec.h"

namespace Ash {

template<typename T>
class Bounds3 {
public:
	Point3<T> Min, Max;

public:
	Bounds() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		Min = Point2<T>(maxNum, maxNum);
		Max = Point2<T>(minNum, minNum);
	}
	~Bounds() = default;
};

}