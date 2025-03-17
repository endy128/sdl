// road.cpp
#include "road.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <memory>
#include <string>

/* Constants */
// Screen dimension constants
constexpr int kScreenWidth{800};
constexpr int kScreenHeight{600};

/* Global Variables */
// The window we'll be rendering to
SDL_Window *gWindow{nullptr};

// The renderer used to draw to the window
SDL_Renderer *gRenderer{nullptr};
std::unique_ptr<Road> gRoad;

Road::Road(SDL_Renderer *renderer, int screenWidth, int screenHeight)
    : mRenderer(renderer), mScreenWidth(screenWidth),
      mScreenHeight(screenHeight), mRoadWidth(2000.0f), mSegmentLength(200.0f),
      mCameraHeight(1000.0f), mCameraDepth(0.84f), mDrawDistance(300),
      mTrackLength(1600) {
  InitializeRoad();
}

void Road::InitializeRoad() {
  mSegments.clear();

  // Create a simple track with some curves and hills
  for (int i = 0; i < mTrackLength; i++) {
    RoadSegment segment;
    segment.z = i * mSegmentLength;
    segment.x = 0;
    segment.y = 0;
    segment.width = mRoadWidth;

    // Add some curves every 100 segments
    segment.curve = 0;
    if (i > 300 && i < 400)
      segment.curve = 2.0f; // Right turn
    if (i > 600 && i < 700)
      segment.curve = -2.0f; // Left turn
    if (i > 900 && i < 1000)
      segment.curve = 3.0f; // Sharper right turn

    // Add some hills
    if (i > 1100 && i < 1200)
      segment.elevation = sin((i - 1100) / 100.0f * M_PI) * 1500;

    // Set colors (alternating for visual effect)
    if (i % 2 == 0) {
      segment.roadColor = SDL_Color{107, 107, 107, 255}; // Dark gray
      segment.grassColor = SDL_Color{16, 200, 16, 255};  // Light green
    } else {
      segment.roadColor =
          SDL_Color{105, 105, 105, 255};              // Slightly different gray
      segment.grassColor = SDL_Color{0, 154, 0, 255}; // Dark green
    }

    // Add road markings every few segments
    segment.lineColor = SDL_Color{255, 255, 255, 255}; // White

    mSegments.push_back(segment);
  }
}

void Road::Update(float deltaTime) {
  // Update road elements if needed
  // (For example, animate roadside objects, etc.)
  // Move forward at constant speed (adjust 5000.0f to change speed)
  mPosition += deltaTime * 5000.0f;

  // Loop back to start when we reach the end
  if (mPosition >= mTrackLength * mSegmentLength) {
    mPosition = 0.0f;
  }
}

void Road::Render() {
  // Get player position from player class
  float playerX = 0.0f; // This would come from the player
  float playerY = 0.0f;
  float playerZ = mPosition; // Use mPosition here

  // Find player segment
  int playerSegment =
      static_cast<int>(playerZ / mSegmentLength) % mSegments.size();

  // Clear screen with sky color
  SDL_SetRenderDrawColor(mRenderer, 113, 197, 207, 255); // Sky blue
  SDL_RenderClear(mRenderer);

  // Set initial clip values
  float clipX = 0;
  float clipY = mScreenHeight;
  float clipWidth = 0;

  // Start rendering from farther away, behind the player
  int startPos = playerSegment - 50;
  while (startPos < 0)
    startPos += mSegments.size();

  // Draw road segments from far to near
  for (int n = 0; n < mDrawDistance; n++) {
    int segmentIndex = (startPos + n) % mSegments.size();
    RenderSegment(segmentIndex, playerX, playerY, playerZ);
  }
}

void Road::RenderSegment(int segmentIndex, float playerX, float playerY,
                         float playerZ) {
  // Get segment
  RoadSegment &segment = mSegments[segmentIndex];

  // Calculate segment position relative to camera
  // Apply curve to x position
  float curve = segment.curve;
  float x = segment.x - playerX;
  float y =
      segment.y + segment.elevation + mCameraHeight - playerY; // Add elevation
  float z = segment.z - playerZ;

  // Skip if behind camera
  if (z <= 0)
    return;

  // Apply curve based on distance (more curve effect farther away)
  x += curve * (z * z) * 0.5f; // Increased multiplier for more noticeable curve

  // Project points to screen space
  float screenX, screenY, screenWidth;
  ProjectToScreen(x, y, z, screenX, screenY, screenWidth);

  // Define the road quad vertices
  SDL_Vertex roadQuad[4];

  // Bottom left
  roadQuad[0].position.x = screenX - screenWidth;
  roadQuad[0].position.y = screenY;
  roadQuad[0].color = {
      segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f,
      segment.roadColor.b / 255.0f, segment.roadColor.a / 255.0f};

  // Bottom right
  roadQuad[1].position.x = screenX + screenWidth;
  roadQuad[1].position.y = screenY;
  roadQuad[1].color = {
      segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f,
      segment.roadColor.b / 255.0f, segment.roadColor.a / 255.0f};

  // Top right (from previous segment)
  roadQuad[2].position.x = previousScreenX + previousScreenWidth;
  roadQuad[2].position.y = previousScreenY;
  roadQuad[2].color = {
      segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f,
      segment.roadColor.b / 255.0f, segment.roadColor.a / 255.0f};

  // Top left (from previous segment)
  roadQuad[3].position.x = previousScreenX - previousScreenWidth;
  roadQuad[3].position.y = previousScreenY;
  roadQuad[3].color = {
      segment.roadColor.r / 255.0f, segment.roadColor.g / 255.0f,
      segment.roadColor.b / 255.0f, segment.roadColor.a / 255.0f};

  // Using indices for triangles (SDL3 has improved geometry rendering)
  int indices[6] = {0, 1, 2, 2, 3, 0};

  // Draw road surface
  SDL_RenderGeometry(mRenderer, nullptr, roadQuad, 4, indices, 6);

  // Similar rendering for grass, lane markings, etc.
  // ...

  SDL_Vertex grassQuadLeft[4];
  float grassWidth = screenWidth * 4; // Make grass extend beyond road

  // Bottom vertices (current segment)
  grassQuadLeft[0].position = {screenX - grassWidth, screenY};  // Far left
  grassQuadLeft[1].position = {screenX - screenWidth, screenY}; // Road edge

  // Top vertices (previous segment)
  grassQuadLeft[2].position = {previousScreenX - previousScreenWidth,
                               previousScreenY};
  grassQuadLeft[3].position = {previousScreenX - grassWidth, previousScreenY};

  // Set grass color
  SDL_FColor grassColor = {
      segment.grassColor.r / 255.0f, segment.grassColor.g / 255.0f,
      segment.grassColor.b / 255.0f, segment.grassColor.a / 255.0f};

  for (int i = 0; i < 4; i++) {
    grassQuadLeft[i].color = grassColor;
  }

  // Right grass quad
  SDL_Vertex grassQuadRight[4];

  // Bottom vertices (current segment)
  grassQuadRight[0].position = {screenX + screenWidth, screenY}; // Road edge
  grassQuadRight[1].position = {screenX + grassWidth, screenY};  // Far right

  // Top vertices (previous segment)
  grassQuadRight[2].position = {previousScreenX + grassWidth, previousScreenY};
  grassQuadRight[3].position = {previousScreenX + previousScreenWidth,
                                previousScreenY};

  for (int i = 0; i < 4; i++) {
    grassQuadRight[i].color = grassColor;
  }

  // Draw grass quads
  int grassIndices[6] = {0, 1, 2, 2, 3, 0};
  SDL_RenderGeometry(mRenderer, nullptr, grassQuadLeft, 4, grassIndices, 6);
  SDL_RenderGeometry(mRenderer, nullptr, grassQuadRight, 4, grassIndices, 6);

  // Store values for next segment
  previousScreenX = screenX;
  previousScreenY = screenY;
  previousScreenWidth = screenWidth;

  SDL_Log("Curve: %f  Pos: %f  Ele: %f", curve, mPosition, segment.elevation);
}

void Road::ProjectToScreen(float x, float y, float z, float &outX, float &outY,
                           float &outW) {
  // Project 3D point to 2D screen space
  float scale = mCameraDepth / z;
  outX = (mScreenWidth / 2) + (scale * x * mScreenWidth / 2);
  outY = (mScreenHeight / 2) + (scale * y * mScreenHeight / 2);
  outW = scale * mRoadWidth * mScreenWidth / 2;
}

// destructor
Road::~Road() {
  // Add any cleanup code if needed
}

////////////////////////////////////////
void close() {
  // Clean up texture
  //   gPngTexture.destroy();

  // Destroy window
  SDL_DestroyRenderer(gRenderer);
  gRenderer = nullptr;
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;

  // Quit SDL subsystems
  //  IMG_Quit();  // No need to quit IMG subsystem
  SDL_Quit();
}

/* Function Implementations */
bool init() {
  // Initialization flag
  bool success{true};

  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Create window with renderer
    if (!SDL_CreateWindowAndRenderer(
            "SDL3 Tutorial: Textures and Extension Libraries", kScreenWidth,
            kScreenHeight, 0, &gWindow, &gRenderer)) {
      SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
      success = false;
    }

    // ***************

    gRoad = std::make_unique<Road>(gRenderer, kScreenWidth, kScreenHeight);
    // ***************
  }

  return success;
}

int main(int argc, char *args[]) {
  // Final exit code
  int exitCode{0};

  // Initialize
  if (!init()) {
    SDL_Log("Unable to initialize program!\n");
    exitCode = 1;
  } else {

    // The quit flag
    bool quit{false};
    Uint64 previousTime = SDL_GetTicks();

    // The event data
    SDL_Event e;
    SDL_zero(e);

    // The main loop
    while (quit == false) {
      // Get event data
      while (SDL_PollEvent(&e)) {
        // If event is quit type
        if (e.type == SDL_EVENT_QUIT) {
          // End the main loop
          quit = true;
        }
      }

      // Calculate delta time
      Uint64 currentTime = SDL_GetTicks();
      float deltaTime = (currentTime - previousTime) / 1000.0f;
      previousTime = currentTime;

      // ProcessInput();
      // Update and render
      gRoad->Update(deltaTime);
      gRoad->Render();

      // Present renderer
      SDL_RenderPresent(gRenderer);
      // Cap frame rate (SDL3 may have different timing functions)
      SDL_Delay(16); // Approximately 60 FPS
    }
  }

  // Clean up
  close();

  return exitCode;
}
