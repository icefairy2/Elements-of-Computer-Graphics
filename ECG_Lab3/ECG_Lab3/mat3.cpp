#include "mat3.h"
#include "mat2.h"

namespace egc {
    mat3& mat3::operator =(const mat3& srcMatrix) {
        for (int i = 0; i < 9; i++) {
            matrixData[i] = srcMatrix.matrixData[i];
        }
        return *this;
    }

    mat3 mat3::operator *(float scalarValue) const {
        mat3 m;
        for (int i = 0; i < 9; i++) {
            m.matrixData[i] = matrixData[i] * scalarValue;
        }
        return m;
    }

    mat3 mat3::operator *(const mat3& srcMatrix) const {
        mat3 m;
        int i, j, k;
        for (i = 0; i < 3; i++)
            m.at(i, i) = 0.0f;
        for (i = 0; i < 3; i++) 
            for (j = 0; j < 3; j++)
                for (k = 0; k < 3; k++) 
                    m.at(i, j) += at(i, k) * srcMatrix.at(k, j);
        return m;
    }

    vec3 mat3::operator *(const vec3& srcVector) const {
        vec3 v;
        v.x = srcVector.x * at(0, 0) + srcVector.y * at(0, 1) + srcVector.z * at(0, 2);
        v.y = srcVector.x * at(1, 0) + srcVector.y * at(1, 1) + srcVector.z * at(1, 2);
        v.z = srcVector.x * at(2, 0) + srcVector.y * at(2, 1) + srcVector.z * at(2, 2);
        return v;
    }

    mat3 mat3::operator +(const mat3& srcMatrix) const {
        mat3 m;
        for (int i = 0; i < 9; i++) {
            m.matrixData[i] = matrixData[i] + srcMatrix.matrixData[i];
        }
        return m;
    }

    //get element by (row, column)
    float& mat3::at(int i, int j) {
        return matrixData[i + 3 * j];
    }

    const float& mat3::at(int i, int j) const {
        return matrixData[i + 3 * j];
    }

    float mat3::determinant() const {
        float det = 0;
        for (int i = 0; i < 3; i++)
            det += pow(-1, i) * at(0, i) * cofactor(0, i).determinant();
        return det;
    }

    mat3 mat3::inverse() const {
        mat3 trans = this->transpose();
        mat3 adj;
        float det = this->determinant();
        int i, j;
        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                adj.at(i, j) = pow(-1, i + j) * trans.cofactor(i, j).determinant();
        return adj * (1 / det);
    }

    mat3 mat3::transpose() const {
        mat3 m;
        int i, j;
        for (i = 0; i < 3; i++) 
            for (j = 0; j < 3; j++)
                m.at(i, j) = at(j, i);
        return m;
    }

    mat2 mat3::cofactor(int r, int c) const {
        float cofData[4];
        int i, j, k = 0;
        for(j = 0; j < 3; j++)
            for (i = 0; i < 3; i++)
                if (i != r && j != c) {
                    cofData[k] = at(i, j);
                    k++;
                }
        return mat2(cofData);
    }
}