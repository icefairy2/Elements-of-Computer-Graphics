//
//  main.cpp
//
//  Copyright � 2016 CGIS. All rights reserved.
//

#include <iostream>
#include "include\SDL.h"

#include "mat4.h"
#include "vec4.h"
#include "transform.h"
#include "camera.h"
#include "projection.h"
#include "clip.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

//define window dimensions
const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 512;

//The window
SDL_Window *window = NULL;
//The window renderer
SDL_Renderer* renderer = NULL;
SDL_Event currentEvent;

bool quit = false;

int mouseX, mouseY;
float rotationAngle = 0.0f;
float cameraZ = 3.0f;

egc::mat4 viewTransformMatrix;
egc::mat4 cameraMatrix;
egc::mat4 perspectiveMatrix;
egc::mat4 modelMatrix;
egc::Camera myCamera;

egc::vec2 viewportTopLeftCorner(30, 30);
egc::vec2 viewportDimensions(400, 400);

double z_buf[400][400];

bool backFaceCulling = true;
bool displayNormals = false;


egc::vec4 findCenterPointOfTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3);
egc::vec3 findNormalVectorToTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3);
bool isTriangleVisible(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3, egc::vec3 normalVector);
bool clipTriangleInHomegeneousCoordinates(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3);

void drawWireframeTriangle(SDL_Renderer *renderer, egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
    int dy = 2 * viewportTopLeftCorner.y + viewportDimensions.y;

    egc::vec4 triangleCenter = findCenterPointOfTriangle(vertex1, vertex2, vertex3);
    egc::vec3 normalVector = findNormalVectorToTriangle(vertex1, vertex2, vertex3);
    
    if (isTriangleVisible(vertex1, vertex2, vertex3, normalVector)) {
        SDL_RenderDrawLine(renderer, vertex1.x, -(vertex1.y - dy), vertex2.x, -(vertex2.y - dy));
        SDL_RenderDrawLine(renderer, vertex1.x, -(vertex1.y - dy), vertex3.x, -(vertex3.y - dy));
        SDL_RenderDrawLine(renderer, vertex2.x, -(vertex2.y - dy), vertex3.x, -(vertex3.y - dy));

        //display normal
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
        SDL_RenderDrawLine(renderer, triangleCenter.x, dy - triangleCenter.y, triangleCenter.x + normalVector.x * 300, dy - (normalVector.y * 300 + triangleCenter.y));
    }

    
}

int lineEq(egc::vec4 a, egc::vec4 b, float x, float y) {
    return (a.y - b.y)*x + (b.x - a.x)*y + a.x*b.y - b.x*a.y;
}

void drawRasterizedTriangle(SDL_Renderer *renderer, egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3) {
    float minX, minY, maxX, maxY;
    egc::vec3 normalVector = findNormalVectorToTriangle(vertex1, vertex2, vertex3);
   
    //find min and max for X
    if (vertex1.x < vertex2.x) {
        minX = vertex1.x;
        maxX = vertex2.x;
    }
    else {
        minX = vertex2.x;
        maxX = vertex1.x;
    }
    if (vertex3.x < minX)
        minX = vertex3.x;
    if (vertex3.x > maxX)
        maxX = vertex3.x;

    //find min and max for Y
    if (vertex1.y < vertex2.y) {
        minY = vertex1.y;
        maxY = vertex2.y;
    }
    else {
        minY = vertex2.y;
        maxY = vertex1.y;
    }
    if (vertex3.y < minY)
        minY = vertex3.y;
    if (vertex3.y > maxY)
        maxY = vertex3.y;

    //draw triangles
    if (isTriangleVisible(vertex1, vertex2, vertex3, normalVector)) {
        float alpha, beta, gamma;
        float FbcA = lineEq(vertex2, vertex3, vertex1.x, vertex1.y);
        float FacB = lineEq(vertex1, vertex3, vertex2.x, vertex2.y);

        int dy = 2 * viewportTopLeftCorner.y + viewportDimensions.y;
        for (int x = minX; x <= maxX; x++)
            for (int y = minY; y <= maxY; y++) {
                alpha = lineEq(vertex2, vertex3, x, y) / FbcA;
                beta = lineEq(vertex1, vertex3, x, y) / FacB;
                gamma = 1 - alpha - beta;
                if (0 < alpha && alpha < 1 && 0 < beta && beta < 1 && 0 < gamma && gamma < 1) {

                    SDL_SetRenderDrawColor(renderer, 255 * alpha, 255 * beta, 255 * gamma, 0);
                    SDL_RenderDrawPoint(renderer, x, dy - y);
                }
            }
    }
}

bool clipPointInHomogeneousCoordinate(egc::vec4 vertex)
{
    if (vertex.x < -vertex.w || vertex.x > vertex.w) {
        return false;
    }
    if (vertex.y < -vertex.w || vertex.y > vertex.w) {
        return false;
    }
    if (vertex.z < -vertex.w || vertex.z > vertex.w) {
        return false;
    }
    return true;
}

bool clipTriangleInHomegeneousCoordinates(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
    bool v1 = clipPointInHomogeneousCoordinate(vertex1);
    bool v2 = clipPointInHomogeneousCoordinate(vertex2);
    bool v3 = clipPointInHomogeneousCoordinate(vertex3);
    return v1 && v2 && v3;
}

egc::vec4 findCenterPointOfTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
    egc::vec4 triangleCenter;

    triangleCenter = (vertex1 + vertex2 + vertex3) * ((float)1 / 3);

    return triangleCenter;
}

egc::vec3 findNormalVectorToTriangle(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3)
{
    egc::vec3 n;

    n = -crossProduct(vertex2 - vertex1, vertex3 - vertex1);
    //n = n.normalize();

    return n;
}

bool isTriangleVisible(egc::vec4 vertex1, egc::vec4 vertex2, egc::vec4 vertex3, egc::vec3 normalVector)
{
    egc::vec4 center = findCenterPointOfTriangle(vertex1, vertex2, vertex3);
    egc::vec3 triangleCenter = egc::vec3(center.x, center.y, center.z);
    float angle = dotProduct(triangleCenter - myCamera.cameraPosition, normalVector);
    
    if (angle <= M_PI / 2)
        return false;

    return true;
}

void renderMesh(SDL_Renderer *renderer, std::vector<tinyobj::shape_t> shapes)
{
    //for each mesh in the 3d model representation
    for (size_t i = 0; i < shapes.size(); i++) {
        //for each triangle
        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            int vertexId1 = shapes[i].mesh.indices[3*f+0];
            int vertexId2 = shapes[i].mesh.indices[3*f+1];
            int vertexId3 = shapes[i].mesh.indices[3*f+2];
            
            //update each of the three vertices that form a triangle with the ModelView matrix
            egc::vec4 tempVertex1 = egc::vec4(shapes[i].mesh.positions[3*vertexId1+0], shapes[i].mesh.positions[3*vertexId1+1], shapes[i].mesh.positions[3*vertexId1+2], 1);
            egc::vec4 tempVertex2 = egc::vec4(shapes[i].mesh.positions[3*vertexId2+0], shapes[i].mesh.positions[3*vertexId2+1], shapes[i].mesh.positions[3*vertexId2+2], 1);
            egc::vec4 tempVertex3 = egc::vec4(shapes[i].mesh.positions[3*vertexId3+0], shapes[i].mesh.positions[3*vertexId3+1], shapes[i].mesh.positions[3*vertexId3+2], 1);
            
            tempVertex1 =  modelMatrix * tempVertex1;
            tempVertex2 =  modelMatrix * tempVertex2;
            tempVertex3 =  modelMatrix * tempVertex3;
           
            tempVertex1 = cameraMatrix *  tempVertex1;
            tempVertex2 = cameraMatrix *  tempVertex2;
            tempVertex3 = cameraMatrix *  tempVertex3;


            tempVertex1 = perspectiveMatrix *  tempVertex1;
            tempVertex2 = perspectiveMatrix *  tempVertex2;
            tempVertex3 = perspectiveMatrix *  tempVertex3;

            tempVertex1 = viewTransformMatrix *  tempVertex1;
            tempVertex2 = viewTransformMatrix *  tempVertex2;
            tempVertex3 = viewTransformMatrix *  tempVertex3;
            
            perspectiveDivide(tempVertex1);
            perspectiveDivide(tempVertex2);
            perspectiveDivide(tempVertex3);

            SDL_SetRenderDrawColor(renderer, 96, 96, 96, 0);
            drawWireframeTriangle(renderer, tempVertex1, tempVertex2, tempVertex3);
            drawRasterizedTriangle(renderer, tempVertex1, tempVertex2, tempVertex3);
        
        }
    }
}

std::vector<tinyobj::shape_t> readOBJ()
{
    std::string inputfile = "bunny.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string err;
    bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str());
    
    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }
    
    if (!ret) {
        exit(1);
    }
    
#ifdef PRINTINFO
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    
    for (size_t i = 0; i < shapes.size(); i++) {
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
        
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
        }
        
        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3*v+0],
                   shapes[i].mesh.positions[3*v+1],
                   shapes[i].mesh.positions[3*v+2]);
        }
    }
#endif
    return shapes;
}

bool initWindow()
{
    bool success = true;
    
    //Try to initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL initialization failed" << std::endl;
        success = false;
    }
    else{
        //Try to create the window
        window = SDL_CreateWindow("SDL Hello World Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
        
        if(window == NULL)
        {
            std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            //Create renderer for window
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
            if (renderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    
    return success;
}

void destroyWindow()
{
    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    //Quit SDL
    SDL_Quit();
}

void handleMouseEvents()
{
    //Mouse event -> pressed button
    if (currentEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        if (currentEvent.button.button == SDL_BUTTON_LEFT)
        {
            SDL_GetMouseState(&mouseX, &mouseY);
            
        }
        
        if (currentEvent.button.button == SDL_BUTTON_RIGHT)
        {
            SDL_GetMouseState(&mouseX, &mouseY);
            
        }
    }
    /*
    //Mouse event -> mouse movement
    if (currentEvent.type == SDL_MOUSEMOTION)
    {
        if (currentEvent.button.button & SDL_BUTTON_LMASK)
        {
            SDL_GetMouseState(&mouseX, &mouseY);
        }
        
        if (currentEvent.button.button & SDL_BUTTON_RMASK)
        {
            SDL_GetMouseState(&mouseX, &mouseY);
            
        }
    }
     */
}

void handleKeyboardEvents()
{
    //Keyboard event
    if (currentEvent.type == SDL_KEYDOWN)
    {
        switch (currentEvent.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                quit = true;
                break;
            
            case SDLK_a:
                rotationAngle += 10.0f;
                break;
            
            case SDLK_d:
                rotationAngle -= 10.0f;
                break;
            
            case SDLK_w:
                cameraZ -= 0.5f;
                break;
                
            case SDLK_s:
                cameraZ += 0.5f;
                break;
                
            case SDLK_z:
                backFaceCulling = true;
                break;
                
            case SDLK_c:
                backFaceCulling = false;
                break;
                
            case SDLK_q:
                displayNormals = true;
                break;
                
            case SDLK_e:
                displayNormals = false;
                break;
                
            default:
                break;
        }
    }
}

int main(int argc, char * argv[]) {
    if(!initWindow())
    {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }
    
    myCamera = egc::Camera(egc::vec3(-0.3f, 1.5f, cameraZ), egc::vec3(-0.3f, 1.5f, -10.0f), egc::vec3(0.0f, 1.0f, 0.0f));
    viewTransformMatrix = egc::defineViewTransformMatrix(viewportTopLeftCorner.x, viewportTopLeftCorner.y, viewportDimensions.x, viewportDimensions.y);
    cameraMatrix = egc::defineCameraMatrix(myCamera);
    perspectiveMatrix = egc::definePerspectiveProjectionMatrix(45.0f, 1.0, -0.1f, -10.0f);
    
    std::vector<tinyobj::shape_t> shapes = readOBJ();
    
    SDL_Rect viewportRectangle = {static_cast<int>(viewportTopLeftCorner.x), static_cast<int>(viewportTopLeftCorner.y), static_cast<int>(viewportDimensions.x), static_cast<int>(viewportDimensions.y)};
    
    while (!quit) {
        //Handle events on queue
        while (SDL_PollEvent(&currentEvent) != 0)
        {
            //User requests quit
            if(currentEvent.type == SDL_QUIT)
            {
                quit = true;
            }
            
            handleMouseEvents();
            handleKeyboardEvents();
            
            //Clear screen
            SDL_SetRenderDrawColor(renderer, 224, 224, 224, 0);
            SDL_RenderClear(renderer);
            
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
            SDL_RenderDrawRect(renderer, &viewportRectangle);
            
            modelMatrix = egc::rotateY(rotationAngle) * egc::scale(15.0f, 15.0f, 15.0f);
            myCamera.cameraPosition.z = cameraZ;
            cameraMatrix = egc::defineCameraMatrix(myCamera);
            
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
            renderMesh(renderer, shapes);

            //Update screen
            SDL_RenderPresent(renderer);
        }
    }
    
    destroyWindow();
    return 0;
}
