#include "road.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

#define TRACKLENGTH 1000
#define SCREENX 640
#define SCREENY 480
#define VIEWDISTANCE 10 // how many segments to render
#define SEGMENTHEIGHT 20.0F
#define SEGMENTWIDTH 200.0F

Road::Road(SDL_Renderer *ren, int screenWidth, int screenHeight)
    : mRenderer(ren), mScreenWidth(screenWidth), mScreenHeight(screenHeight),
      mSegmentWidth(SEGMENTWIDTH), mSegmentHeight(SEGMENTHEIGHT),
      mDrawDistance(VIEWDISTANCE), mTrackLength(TRACKLENGTH) {
  InitialiseRoad();
}

Road::~Road() {}

void Road::InitialiseRoad() {
  mSegments.clear();               // Clear any existing segments
  mSegments.reserve(mTrackLength); // Reserve space for efficiency

  for (int i = 0; i < mTrackLength; i++) {
    RoadSegment segment; // Create a temporary segment
    segment.x = 0.0f;    // change to add curves?
    segment.y = 0.0f;    // change to add elevation?
    segment.z = i * mSegmentHeight;
    segment.width = mSegmentWidth;

    segment.curve = 0.0f;
    segment.elevation = 0.0f;

    if (i % 2 == 0) {
      segment.roadColor = SDL_Color{128, 128, 128, 255};
      segment.glassColor = SDL_Color{0, 128, 0, 255};
    } else {
      segment.roadColor = SDL_Color{105, 105, 105, 255};
      segment.glassColor = SDL_Color{0, 105, 0, 255};
    }
    if (i % 6 == 0) // line colour
    {
      segment.lineColor = SDL_Color{255, 255, 255, 255};
    }
    if (i % 50 == 0) {
      segment.roadColor = SDL_Color{255, 0, 0, 255};
      segment.glassColor = SDL_Color{0, 128, 0, 255};
    }
    mSegments.push_back(segment);
  }
  std::cout << "Initialised road with " << mSegments.size() << " segments"
            << std::endl;
}

void Road::Render() {
  SDL_SetRenderDrawColor(mRenderer, 113, 197, 207, 255); // draw the sky
  SDL_RenderClear(mRenderer);

  // Calculate which segment we're on based on player position
  int baseSegment = static_cast<int>(mPlayerPosition / mSegmentHeight);
  std::cout << "Base segment: " << baseSegment << std::endl;

  // Render segments starting from current position
  // for (int i = baseSegment; i < baseSegment + VIEWDISTANCE; i++) {
  //   int currentSegment = (baseSegment + i) % mTrackLength;
  //   if (currentSegment >= 0 && currentSegment < mTrackLength) {
  //     RenderSegment(currentSegment);
  //   }
  // }

  float min = mSegments[baseSegment].z;
  float max = mSegments[baseSegment + VIEWDISTANCE].z;

  // render only segements from the base segment to the base segment + view
  for (int i = baseSegment; i < baseSegment + VIEWDISTANCE; i++) {
    RenderSegment(i, min, max);
  }

}

void Road::RenderSegment(int segmentIndex, float min, float max) {

  RoadSegment &segment = mSegments[segmentIndex];

  float screenX = SCREENX / 2;
  float screenY = SCREENY;

  // do not draw segments behind the camera

  // draw VIEWDISTANCE segments

  // draw centre of screen +/- 1/2 road width
  // draw width relative to position on screen, scale with distance

  // set previous x and y to bottom or screen and max road width
  if (segmentIndex == 0) {
    // bottom right
    mPreviousX1 = SCREENX;
    mPreviousY1 = SCREENY;
    // bottom left
    mPreviousX2 = 0;
    mPreviousY2 = SCREENY;
  }

  // interpret segement z number to a fixed range
  // output = minOutput + (input - minInput) * (maxOutput - minOutput) / (maxInput - minInput)
  float minOutput, maxOutput, z{0.0f};
  minOutput = 0.0f;
  maxOutput = SCREENY / 2;
  z = minOutput + (segment.z - min) * (maxOutput - minOutput) / (max - min);


  SDL_Vertex roadQuad[4];
  // top left
  roadQuad[0].position.x = screenX - mSegmentWidth;
  roadQuad[0].position.y = screenY - z ;
  roadQuad[0].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // top right
  roadQuad[1].position.x = screenX + mSegmentWidth;
  roadQuad[1].position.y = screenY - z ;
  roadQuad[1].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // bottom right
  roadQuad[2].position.x = mPreviousX1;
  roadQuad[2].position.y = mPreviousY1;
  roadQuad[2].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // bottom left
  roadQuad[3].position.x = mPreviousX2;
  roadQuad[3].position.y = mPreviousY2;
  roadQuad[3].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  int indices[] = {0, 1, 2, 2, 3, 0};

  SDL_RenderGeometry(mRenderer, nullptr, roadQuad, 4, indices, 6);
  std::cout << mSegments[segmentIndex].z  << std::endl;

  mPreviousX1 = roadQuad[1].position.x;
  mPreviousY1 = roadQuad[1].position.y;
  mPreviousX2 = roadQuad[0].position.x;
  mPreviousY2 = roadQuad[0].position.y;
}

// void Road::ProjectToScreen(float x, float y, float z, float &screenX,
//                            float &screenY, float &screenWidth) {
//   float cameraHeight = 1000.0f;
//   float cameraDepth = 0.84f;

//   // Adjust z to be relative to player
//   float relativeZ = z - cameraHeight;

//   if (relativeZ <= 0)
//     relativeZ = 0.1f; // Prevent division by zero

//   float scale = cameraDepth / relativeZ;
//   screenX = mScreenWidth / 2 + (x * scale * mScreenWidth / 2);
//   screenY = mScreenHeight / 2 - (y * scale * mScreenHeight / 2);
//   screenWidth = mSegmentWidth * scale; // Scale the road width with
//   perspective
// }

void Road::Update(float deltaTime) {
    // Handle keyboard input
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    
    if (keystate[SDL_SCANCODE_UP]) {
        mSpeed = std::min(mSpeed + ACCELERATION * deltaTime, MAX_SPEED);
    } else if (keystate[SDL_SCANCODE_DOWN]) {
        mSpeed = std::max(mSpeed - ACCELERATION * deltaTime, 0.0f);
    } else {
        // Gradual slow down when no key is pressed
        mSpeed = std::max(0.0f, mSpeed - ACCELERATION * deltaTime * 0.5f);
    }

    // Update position
    mPlayerPosition += mSpeed;
    
    // Wrap around the track
    if (mPlayerPosition >= mTrackLength * mSegmentHeight) {
        mPlayerPosition = 0;
    }
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *win = SDL_CreateWindow("SDL3 Project", 640, 480, 0);
  if (win == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, NULL);
  if (ren == nullptr) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Event e;
  bool quit = false;

  // initialise the road
  Road road(ren, SCREENX, SCREENY);

  Uint64 previousTime = SDL_GetTicks();
    
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Calculate delta time
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        road.Update(deltaTime);
        road.Render();
        SDL_RenderPresent(ren);
    }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
