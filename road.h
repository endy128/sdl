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

class Road
{
public:
    Road(SDL_Renderer *ren, int screenWidth, int screenHeight);
    ~Road();
    void Update(float deltaTime, float playerX, float playerZ);
    void Render();

private:
    void InitialiseRoad();
    void RenderSegment(int segmentIndex);
    void ProjectToScreen(float x, float y, float z, float &screenX, float &screenY, float &scale);

    SDL_Renderer* mRenderer;
    int mScreenWidth;
    int mScreenHeight;
    std::vector<RoadSegment> mSegments;

    float mRoadWidth;
    float mSegmentLength;
    float mDrawDistance;

    int mTrackLength;

    float previousScreenX = 0.0f;
    float previousScreenY = 0.0f;
    float previousScreenWidth = 0.0f;
    int mCurrentSegment = 0;
    float mPrevX1, mPrevY1, mPrevX2, mPrevY2 {0.0f};
};
