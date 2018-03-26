//
//  projection.cpp
//  Lab8_TODO
//
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "projection.h"

namespace egc {
    mat4 defineViewTransformMatrix(int startX, int startY, int width, int height)
    {
        mat4 viewTransformMatrix;
        viewTransformMatrix.at(0, 0) = width / 2;
        viewTransformMatrix.at(1, 1) = -height / 2;
        viewTransformMatrix.at(0, 3) = startX + (width / 2);
        viewTransformMatrix.at(1, 3) = startY + (height / 2);
        return viewTransformMatrix;
    }
    
    mat4 defineCameraMatrix(Camera myCamera)
    {
        mat4 cameraMatrix;

        vec3 gazeDirection = myCamera.cameraTarget - myCamera.cameraPosition;
        vec3 w = -gazeDirection.normalize();
        vec3 u = (crossProduct(myCamera.cameraUp, w)).normalize();
        vec3 v = crossProduct(w, u);

        cameraMatrix.matrixData[0] = u.x;
        cameraMatrix.matrixData[1] = v.x;
        cameraMatrix.matrixData[2] = w.x;
        
        cameraMatrix.matrixData[4] = u.y;
        cameraMatrix.matrixData[5] = v.y;
        cameraMatrix.matrixData[6] = w.y;
        
        cameraMatrix.matrixData[8] = u.z;
        cameraMatrix.matrixData[9] = v.z;
        cameraMatrix.matrixData[10] = w.z;
        
        mat4 transformMatrix = translate(-myCamera.cameraPosition);
        cameraMatrix = cameraMatrix*transformMatrix;

        return cameraMatrix;
    }
    
    mat4 definePerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar)
    {
        mat4 perspectiveMatrix;
        
        perspectiveMatrix.at(0, 0) = 1 / (aspect * tanf(fov / 2));
        perspectiveMatrix.at(1, 1) = 1 / (tanf(fov / 2));
        perspectiveMatrix.at(2, 2) = (zFar + zNear) / (zNear - zFar);
        perspectiveMatrix.at(2, 3) = 2 * zFar * zNear / (zFar - zNear);
        perspectiveMatrix.at(3, 3) = 0;
        perspectiveMatrix.at(3, 2) = 1;

        return perspectiveMatrix;
    }
    
    void perspectiveDivide(vec4 &inputVertex)
    {
        inputVertex = inputVertex * (1 / inputVertex.w);
    }
}