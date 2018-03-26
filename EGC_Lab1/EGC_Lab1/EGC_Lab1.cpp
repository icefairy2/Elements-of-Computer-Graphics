//
//  main.cpp
//  SDL_Lab1
//
//  Created by CGIS on 16/02/16.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include <iostream>
#include "include\SDL.h"

//define window dimensions
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Surface *windowSurface = NULL;
SDL_Event currentEvent;

int rectX1=0, rectY1=0;
int rectX2=0, rectY2=0;

SDL_Rect rectangleCoordinates = {rectX1, rectY1, 0,0};
Uint32 rectagleColor;

bool quit = false;

int mouseX, mouseY;

int r=0,g=0,b=0;
char curr='r';

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
            //Try to get the window surface
            windowSurface = SDL_GetWindowSurface(window);
            
            if(windowSurface == NULL)
            {
                std::cout << "Failed to get the window surface: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                //Initialize surface color
                SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 255, 255, 255));
                
                //Update the surface
                SDL_UpdateWindowSurface(window);
            }

        }
    }
    
    return success;
}

void destroyWindow()
{
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    
    //Quit SDL
    SDL_Quit();
}

int min (int a, int b){
    if (a<b)
        return a;
    else
        return b;
}

int abs(int a) {
    if (a<0)
        return a*(-1);
    else return a;
}

int main(int argc, char * argv[]) {
    if(!initWindow())
    {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }
    
    rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
    
    while (!quit) {
        //Handle events on queue
        if(SDL_WaitEvent(&currentEvent) != 0)
        {
            //User requests quit
            if(currentEvent.type == SDL_QUIT)
            {
                quit = true;
            }
            
            //Mouse event -> pressed button
            if(currentEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if(currentEvent.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&mouseX, &mouseY);
                    rectX1=mouseX;
                    rectY1=mouseY;
                    std::cout << "Mouse click => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
                }
            }
            
            //Mouse event -> mouse movement
            if(currentEvent.type == SDL_MOUSEMOTION)
            {
                if(currentEvent.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&mouseX, &mouseY);
                    rectX2=mouseX;
                    rectY2=mouseY;
                    std::cout << "Mouse move => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
                }
                
            }
            
            //Keyboard event

            if(currentEvent.type == SDL_KEYDOWN)
            {
                switch(currentEvent.key.keysym.sym)
                {
                    case SDLK_r:
                        curr='r';
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                    case SDLK_g:
                        curr='g';
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                    case SDLK_b:
                        curr='b';
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                    default:
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                }
            }
            if(currentEvent.type == SDL_KEYDOWN)
            {
                switch(currentEvent.key.keysym.sym)
                {
                    case SDLK_UP:
                        switch (curr){
                            case 'r':r+=5; break;
                            case 'g':g+=5; break;
                            case 'b':b+=5; break;
                            default: rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b); break;
                        }
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                    case SDLK_DOWN:
                        switch (curr){
                            case 'r':r-=5; break;
                            case 'g':g-=5; break;
                            case 'b':b-=5; break;
                            default: rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b); break;
                        }
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                    
                        
                    default:
                        rectagleColor = SDL_MapRGB(windowSurface->format, r,g,b);
                        break;
                }
            }
            
            SDL_Rect rectangleCoordinates = {min(rectX1,rectX2), min(rectY1,rectY2), abs(rectX2-rectX1),abs(rectY2-rectY1)};

            SDL_FillRect(windowSurface, &rectangleCoordinates, rectagleColor);
            
            SDL_UpdateWindowSurface(window);
        }
    }
    
    destroyWindow();
    return 0;
}
