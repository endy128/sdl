// road.h
#pragma once
#include <SDL3/SDL.h>
#include <vector>

struct RoadSegment {
  float x, y, z;        // Position in 3D space
  float width;          // Road width
  float curve;          // Curvature amount
  float elevation;      // Y-position for hills
  int spriteID;         // ID for roadside object
  SDL_Color roadColor;  // Road color
  SDL_Color grassColor; // Grass color
  SDL_Color lineColor;  // Line markings color
};

class Road {

public:
  Road(SDL_Renderer *renderer, int screenWidth, int screenHeight);
  ~Road();

  void Update(float deltaTime);
  void Render();

  float GetRoadWidth() const { return mRoadWidth; }
  float GetSegmentLength() const { return mSegmentLength; }
  int GetTotalSegments() const { return mSegments.size(); }

private:
  void InitializeRoad();
  void RenderSegment(int segmentIndex, float playerX, float playerY,
                     float playerZ);
  void ProjectToScreen(float x, float y, float z, float &outX, float &outY,
                       float &outW);

  SDL_Renderer *mRenderer;
  int mScreenWidth;
  int mScreenHeight;
  float previousScreenX = 0.0f;
  float previousScreenY = 0.0f;
  float previousScreenWidth = 0.0f;

  std::vector<RoadSegment> mSegments;

  // Road properties
  float mRoadWidth;
  float mSegmentLength;
  float mCameraHeight;
  float mCameraDepth;
  float mDrawDistance;

  // Track generation parameters
  int mTrackLength;

  float mPosition{0.0f};  // Add this member variable
};
