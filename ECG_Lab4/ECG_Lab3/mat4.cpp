#include "mat4.h"
#include "mat3.h"

namespace egc {
    mat4& mat4::operator =(const mat4& srcMatrix) {
        for (int i = 0; i < 16; i++) {
            matrixData[i] = srcMatrix.matrixData[i];
        }
        return *this;
    }

    mat4 mat4::operator *(float scalarValue) const {
        mat4 m;
        for (int i = 0; i < 16; i++) {
            m.matrixData[i] = matrixData[i] * scalarValue;
        }
        return m;
    }

    mat4 mat4::operator *(const mat4& srcMatrix) const {
        mat4 m;
        int i, j, k;
        for (i = 0; i < 4; i++)
            m.at(i, i) = 0.0f;
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                for (k = 0; k < 4; k++)
                    m.at(i, j) += at(i, k) * srcMatrix.at(k, j);
        return m;
    }

    vec4 mat4::operator *(const vec4& srcVector) const {
        vec4 v;
        v.x = srcVector.x * at(0, 0) + srcVector.y * at(0, 1) + srcVector.z * at(0, 2) + srcVector.w * at(0, 3);
        v.y = srcVector.x * at(1, 0) + srcVector.y * at(1, 1) + srcVector.z * at(1, 2) + srcVector.w * at(1, 3);
        v.z = srcVector.x * at(2, 0) + srcVector.y * at(2, 1) + srcVector.z * at(2, 2) + srcVector.w * at(2, 3);
        v.w = srcVector.x * at(3, 0) + srcVector.y * at(3, 1) + srcVector.z * at(3, 2) + srcVector.w * at(3, 3);
        return v;
    }

    mat4 mat4::operator +(const mat4& srcMatrix) const {
        mat4 m;
        for (int i = 0; i < 16; i++) {
            m.matrixData[i] = matrixData[i] + srcMatrix.matrixData[i];
        }
        return m;
    }

    //get element by (row, column)
    float& mat4::at(int i, int j) {
        return matrixData[i + 4 * j];
    }

    const float& mat4::at(int i, int j) const {
        return matrixData[i + 4 * j];
    }

    float mat4::determinant() const {
        float det = 0;
        for (int i = 0; i < 4; i++)
            det += pow(-1, i) * at(0, i) * cofactor(0, i).determinant();
        return det;
    }

    mat4 mat4::inverse() const {
        mat4 trans = this->transpose();
        mat4 adj;
        float det = this->determinant();
        int i, j;
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                adj.at(i, j) = pow(-1, i + j) * trans.cofactor(i, j).determinant();
        return adj * (1 / det);
    }

    mat4 mat4::transpose() const {
        mat4 m;
        int i, j;
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                m.at(i, j) = at(j, i);
        return m;
    }

    mat3 mat4::cofactor(int r, int c) const {
        float cofData[9];
        int i, j, k = 0;
        for (j = 0; j < 4; j++)
            for (i = 0; i < 4; i++)
                if (i != r && j != c) {
                    cofData[k] = at(i, j);
                    k++;
                }
        return mat3(cofData);
    }
}