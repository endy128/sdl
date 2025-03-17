#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include "road.h"

#define TRACKLENGTH 1000
#define SCREENX 640
#define SCREENY 480

Road::Road(SDL_Renderer *ren, int screenWidth, int screenHeight)
    : mRenderer(ren),
      mScreenWidth(screenWidth),
      mScreenHeight(screenHeight),
      mRoadWidth(20.0f),
      mSegmentLength(100.0f),
      mDrawDistance(300.0f),
      mTrackLength(TRACKLENGTH)
{
    InitialiseRoad();
}

Road::~Road()
{
}

void Road::InitialiseRoad()
{
    mSegments.clear();  // Clear any existing segments
    mSegments.reserve(mTrackLength);  // Reserve space for efficiency

    for (int i = 0; i < mTrackLength; i++)
    {
        RoadSegment segment;  // Create a temporary segment
        segment.x = 0.0f;  // change to add curves?
        segment.y = 0.0f; // change to add elevation?
        segment.z = i * mSegmentLength;
        segment.width = mRoadWidth;

        segment.curve = 0.0f;
        segment.elevation = 0.0f;

        if (i % 2 == 0)
        {
            segment.roadColor = SDL_Color{128, 128, 128, 255};
            segment.glassColor = SDL_Color{0, 128, 0, 255};
        }
        else
        {
            segment.roadColor = SDL_Color{105, 105, 105, 255};
            segment.glassColor = SDL_Color{0, 105, 0, 255};
        }
        if (i % 6 == 0) // line colour
        {
            segment.lineColor = SDL_Color{255, 255, 255, 255};
        }

        mSegments.push_back(segment);
    }
    std::cout << "Initialised road with " << mSegments.size() << " segments" << std::endl;
}

void Road::Render()
{
    float playerX = 0.0f;
    float playerY = 0.0f;  // Raise the camera
    float playerZ = 0.0f;
    
    SDL_SetRenderDrawColor(mRenderer, 113, 197, 207, 255); // draw the sky
    SDL_RenderClear(mRenderer);

    // find the current segment (start = 0)
    // get curr_segment + next 50 segments
    // loop through and render each segment

    for (int i = 50; i >= 0; i--)
    {
        Road::RenderSegment(i);
    }
}

void Road::RenderSegment(int segmentIndex)
{
    RoadSegment &segment = mSegments[segmentIndex];


    // do not draw segments behind the camera
    if (segment.z < 0)
    {
        return;
    }

    // define the road quad vertices
    float screenX = SCREENX / 2;
    float screenY = SCREENY / 2;
    float scale {0.010f};
    float roadHeight = mSegmentLength *  scale * (segmentIndex + 1 );
    float roadWidth = mRoadWidth * scale * (segmentIndex + 1 ) * 7;
    
    SDL_Vertex roadQuad[4];


    // top left
    roadQuad[0].position.x = mPrevX1;
    roadQuad[0].position.y = mPrevY1;
    roadQuad[0].color = {segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f, segment.roadColor.b / 255.0f, 1.0f};

    // top right
    roadQuad[1].position.x = mPrevX2;
    roadQuad[1].position.y = mPrevY2;
    roadQuad[1].color = {segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f, segment.roadColor.b / 255.0f, 1.0f};

    // bottom right
    roadQuad[2].position.x = screenX + segmentIndex * roadWidth;
    roadQuad[2].position.y = screenY + segmentIndex * roadHeight;
    roadQuad[2].color = {segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f, segment.roadColor.b / 255.0f, 1.0f};
 
    // bottom left
    roadQuad[3].position.x = screenX - segmentIndex * roadWidth;
    roadQuad[3].position.y = screenY + segmentIndex * roadHeight;
    roadQuad[3].color = {segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f, segment.roadColor.b / 255.0f, 1.0f};

    int indices[] = {0, 1, 2, 2, 3, 0};

    SDL_RenderGeometry(mRenderer, nullptr, roadQuad, 4, indices, 6);

    mPrevX1 = roadQuad[3].position.x;
    mPrevX2 = roadQuad[2].position.x;
    mPrevY1 = roadQuad[3].position.y;
    mPrevY2 = roadQuad[2].position.y;
    
    // previousScreenX = screenX;
    // previousScreenY = screenY;
    // previousScreenWidth = screenWidth;

    // std::cout << "Rendered segment " << segmentIndex << " at " << roadQuad[0].position.x << ", " << screenY << std::endl;
}

void Road::ProjectToScreen(float x, float y, float z, float &screenX, float &screenY, float &screenWidth)
{
    float cameraHeight = 1000.0f;
    float cameraDepth = 0.84f;
    
    // Adjust z to be relative to player
    float relativeZ = z - cameraHeight;
    
    if (relativeZ <= 0) relativeZ = 0.1f;  // Prevent division by zero
    
    float scale = cameraDepth / relativeZ;
    screenX = mScreenWidth / 2 + (x * scale * mScreenWidth / 2);
    screenY = mScreenHeight / 2 - (y * scale * mScreenHeight / 2);
    screenWidth = mRoadWidth * scale;  // Scale the road width with perspective
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("SDL3 Project", 640, 480, 0);
    if (win == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, NULL);
    if (ren == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    bool quit = false;

    // Define a rectangle
    SDL_FRect greenSquare{270, 190, 100, 100};

    // initialise the road
    Road road(ren, SCREENX, SCREENY);

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
        }

        road.Render();
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
