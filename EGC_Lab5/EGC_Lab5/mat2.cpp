#include "mat2.h"

namespace egc {

	float& mat2::at(int i, int j) {
		return matrixData[i + 2 * j];
	}

	const float& mat2::at(int i, int j) const {
		return matrixData[i + 2 * j];
	}

	float mat2::determinant() const {
		float det = at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
		return det;
	}
}