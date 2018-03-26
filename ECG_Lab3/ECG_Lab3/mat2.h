//
//  mat2.h
//  Helper class to find determinants more easily
//
//  Copyright © 2016 CGIS. All rights reserved.
//

#pragma once

#include <algorithm>
#include <iostream>
#include "vec3.h"

namespace egc {
	class mat2
	{
	public:
		//==============================================
		//matrixData is stored in column-major order
		//  m0  m2 
		//  m1  m3 
		//  
		//  matrixData[] = {m0, m1, m2, m3}
		//==============================================
		float matrixData[4];

		//==============================================
		//constructors
		//==============================================

		//default constructor creates an identity matrix
		mat2()
		{
			for (int i = 0; i < 4; i++) {
				matrixData[i] = (i % 3) ? 0.0f : 1.0f;
			}
		}

		mat2(const float* srcMatrix)
		{
			std::copy(srcMatrix, srcMatrix + 4, matrixData);
		}

		//get element by (row, column)
		float& at(int i, int j);
		const float& at(int i, int j) const;
		float determinant() const;

		//==============================================
		friend std::ostream& operator<< (std::ostream &out, const mat2& srcMatrix);

		bool operator ==(const mat2& srcMatrix) const
		{
			for (int i = 0; i < 4; i++)
				if (std::abs(matrixData[i] - srcMatrix.matrixData[i]) >= std::numeric_limits<float>::epsilon()) {
					return false;
				}
			return true;
		}
	};

	inline std::ostream& operator<< (std::ostream &out, const mat2& srcMatrix)
	{
		for (int i = 0; i < 4; i++)
			out << srcMatrix.matrixData[i] << "\t" << srcMatrix.matrixData[i + 2] << "\n";
		return out;
	};
}
