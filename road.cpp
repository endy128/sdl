#include "road.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

#define TRACKLENGTH 1000
#define SCREENX 640
#define SCREENY 480
#define VIEWDISTANCE 40 // how many segments to render
#define SEGMENTHEIGHT 10.0F
#define SEGMENTWIDTH 200.0F
#define DEBUG 1

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

    if (i > 50 && i < 100) {
      segment.curve = 0.5f;
    }

    if (i > 200 && i < 300) {
      segment.curve = -0.5f;
    }

    if (i > 400 && i < 500) {
      segment.elevation = 0.5f;
    }
    if (i > 600 && i < 700) {
      segment.elevation = -0.5f;
    }

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

void Road::Update(float deltaTime) {
  const bool *keystate = SDL_GetKeyboardState(nullptr);

  if (keystate[SDL_SCANCODE_UP]) {
    mSpeed = std::min(mSpeed + ACCELERATION * deltaTime, MAX_SPEED);
  } else if (keystate[SDL_SCANCODE_DOWN]) {
    mSpeed = std::max(mSpeed - ACCELERATION * deltaTime, 0.0f);
  } else {
    // Gradual slow down when no key is pressed
    mSpeed = std::max(0.0f, mSpeed - ACCELERATION * deltaTime * 0.5f);
  }

  // Update position and wrap using proper modulo
  mPlayerPosition += mSpeed;
  mPlayerPosition = fmod(mPlayerPosition, mTrackLength * mSegmentHeight);
  if (mPlayerPosition < 0) {
    mPlayerPosition += mTrackLength * mSegmentHeight;
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

void Road::RenderSegment(int segmentIndex, float min, float max,
                         int drawIndex) {
  RoadSegment &segment = mSegments[segmentIndex];

  // STEP 1: Calculate relative position to player
  float relativeZ = segment.z - mPlayerPosition;
  while (relativeZ < 0)
    relativeZ += (mTrackLength * mSegmentHeight);

  // STEP 2: Define road perspective constants
  float cameraHeight = 100.0f; // Height of camera above road
  float roadWidth = 700.0f;    // Base width (will be scaled by perspective)
  float clipNear = 0.1f;       // Near clipping plane

  // STEP 3: Projection and perspective calculations
  // Map the segment Z position to screen Y position using perspective
  float projectionScale = cameraHeight / relativeZ;

  // This maps the Z distance to a screen Y coordinate (horizon at SCREENY/3)
  float screenY = (SCREENY / 3) + (projectionScale * SCREENY / 2);

  // Width is scaled by perspective too
  float scaledWidth = (roadWidth * projectionScale) / 2.0f;

  // Center of the screen is the center of the road
  float screenX = SCREENX / 2.0f;

  // STEP 4: Calculate the four corners of our road segment
  float x1 = screenX - scaledWidth; // Left edge
  float x2 = screenX + scaledWidth; // Right edge
  float y = screenY;

  // Store current segment coordinates
  float currentX1 = x1;
  float currentX2 = x2;
  float currentY = y;

  // STEP 5: Special cases for first segment
  if (drawIndex == 0) {
    // For the first segment (farthest visible), set previous to be slightly
    // smaller
    float farScaledWidth =
        scaledWidth * 0.8f;      // Slightly smaller for perspective
    float farY = screenY + 5.0f; // Slightly higher on screen

    mPreviousX1 = screenX - farScaledWidth;
    mPreviousX2 = screenX + farScaledWidth;
    mPreviousY1 = farY;
    mPreviousY2 = farY;
  }

  // Special case for last segment (nearest to player)
  if (drawIndex == VIEWDISTANCE - 1) {
    currentY = SCREENY; // Force it to bottom of screen
    // Make it wide enough to reach screen edges
    currentX1 = 0;
    currentX2 = SCREENX;
  }

  // STEP 6: Draw the road quad
  SDL_Vertex roadQuad[4];

  // Near left (bottom left)
  roadQuad[0].position.x = currentX1 + segment.curve * scaledWidth;
  roadQuad[0].position.y = currentY;
  roadQuad[0].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // Near right (bottom right)
  roadQuad[1].position.x = currentX2 + segment.curve * scaledWidth;
  roadQuad[1].position.y = currentY;
  roadQuad[1].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // Far right (top right)
  roadQuad[2].position.x = mPreviousX2;
  roadQuad[2].position.y = mPreviousY1;
  roadQuad[2].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  // Far left (top left)
  roadQuad[3].position.x = mPreviousX1;
  roadQuad[3].position.y = mPreviousY2;
  roadQuad[3].color = {segment.roadColor.r / 255.0f,
                       segment.roadColor.g / 255.0f,
                       segment.roadColor.b / 255.0f, 1.0f};

  int indices[] = {0, 1, 2, 2, 3, 0};
  SDL_RenderGeometry(mRenderer, nullptr, roadQuad, 4, indices, 6);

  // Store current values for next segment
  mPreviousX1 = roadQuad[0].position.x;
  mPreviousX2 = roadQuad[1].position.x;
  mPreviousY1 = currentY;
  mPreviousY2 = currentY;

  if (DEBUG) {
    printf("Segment %d: z=%0.1f relZ=%0.1f Y=%0.1f width=%0.1f\n", segmentIndex,
           segment.z, relativeZ, y, scaledWidth * 2);
  }
}

void Road::Render() {
  SDL_SetRenderDrawColor(mRenderer, 113, 197, 207, 255); // Draw the sky
  SDL_RenderClear(mRenderer);

  // Calculate which segment we're on based on player position
  int baseSegment =
      static_cast<int>(mPlayerPosition / mSegmentHeight) % mTrackLength;

  // CRITICAL: Render from far to near (back to front)
  for (int n = VIEWDISTANCE - 1; n >= 0; n--) {
    int currentSegment = (baseSegment + n) % mTrackLength;
    RenderSegment(currentSegment, 0, 0, VIEWDISTANCE - 1 - n);
  }
}
