#include <SDL3/SDL.h>
#include <vector>

// global constants
namespace {
const float segmentHeight{10.0f};
} // namespace

struct RoadSegment {
  float x, y, z{0.0f};
  float width{0.0f};
  double curve{0.0f};
  float elevation{0.0f};
  float cornerAccumulator{0.0f};
  SDL_Color roadColor{0, 0, 0, 0};
  SDL_Color glassColor{0, 0, 0, 0};
  SDL_Color lineColor{0, 0, 0, 0};
};

class Road {
public:
  Road(SDL_Renderer *ren, int screenWidth, int screenHeight);
  ~Road();
  void Update(float deltaTime);
  void Render();

private:
  float createCorner(int index, int startIndex, int endIndex, double radius);
  void InitialiseRoad();
  void RenderSegment(int segmentIndex, float min, float max, int drawIndex);
  void ProjectToScreen(float x, float y, float z, float &screenX,
                       float &screenY, float &scale);

  SDL_Renderer *mRenderer;
  int mScreenWidth;
  int mScreenHeight;
  std::vector<RoadSegment> mSegments;

  float mCornerAccumulator{0.0f};

  float mSegmentWidth;
  float mSegmentHeight{segmentHeight};
  float mDrawDistance;

  int mTrackLength;

  float previousScreenX{0.0f};
  float previousScreenY{0.0f};
  float previousScreenWidth{0.0f};
  int mCurrentSegment = 0;
  float mPreviousX1{0.0f};
  float mPreviousY1{0.0f};
  float mPreviousX2{0.0f};
  float mPreviousY2{0.0f};
  float mPlayerPosition{0.0f}; // Position along the track
  float mSpeed{0.0f};          // Current speed
  // FIXME: fix names of these variables
  //  const float ACCELERATION = 2.0f; // Units per second
  //  const float MAX_SPEED = 10.0f;   // Maximum speed

  static constexpr float ACCELERATION{2.0f}; // Units per second squared
  static constexpr float MAX_SPEED{40.0f};   // Units per second
  static constexpr float DECELERATION{0.5f}; // Multiplier for natural slowdown
};
