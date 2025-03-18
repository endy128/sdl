#include <SDL3/SDL.h>
#include <vector>

struct RoadSegment {
  float x, y, z;
  float width;
  float curve;
  float elevation;
  SDL_Color roadColor;
  SDL_Color glassColor;
  SDL_Color lineColor;
};

class Road {
public:
  Road(SDL_Renderer *ren, int screenWidth, int screenHeight);
  ~Road();
  void Update(float deltaTime);
  void Render();

private:
  void InitialiseRoad();
  void RenderSegment(int segmentIndex);
  void ProjectToScreen(float x, float y, float z, float &screenX,
                       float &screenY, float &scale);

  SDL_Renderer *mRenderer;
  int mScreenWidth;
  int mScreenHeight;
  std::vector<RoadSegment> mSegments;

  float mSegmentWidth;
  float mSegmentHeight{10.0f};
  float mDrawDistance;

  int mTrackLength;

  float previousScreenX = 0.0f;
  float previousScreenY = 0.0f;
  float previousScreenWidth = 0.0f;
  int mCurrentSegment = 0;
  float mPreviousX1, mPreviousY1, mPreviousX2, mPreviousY2{0.0f};
  float mPlayerPosition = 0.0f;  // Position along the track
  float mSpeed = 0.0f;          // Current speed
  const float ACCELERATION = 2.0f; // Units per second
  const float MAX_SPEED = 10.0f;   // Maximum speed
};
