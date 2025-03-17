#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>

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

    float vx = 0.1;
    float vy = 0.1;
    float acc = 1.2;
    int colour = 0;

#define vertLen 4
    SDL_Vertex vert[vertLen];

    // top left
    vert[0].position.x = 270;
    vert[0].position.y = 150;
    vert[0].color.r = 1.0;
    vert[0].color.g = 0.0;
    vert[0].color.b = 0.0;
    vert[0].color.a = 1.0;

    // top right
    vert[1].position.x = 370;
    vert[1].position.y = 150;
    vert[1].color.r = 1.0;
    vert[1].color.g = 1.0;
    vert[1].color.b = 0.0;
    vert[1].color.a = 1.0;

    // bottom right
    vert[2].position.x = 470;
    vert[2].position.y = 250;
    vert[2].color.r = 0.0;
    vert[2].color.g = 1.0;
    vert[2].color.b = 0.0;
    vert[2].color.a = 1.0;

    // bottom left
    vert[3].position.x = 170;
    vert[3].position.y = 250;
    vert[3].color.r = 0.0;
    vert[3].color.g = 0.0;
    vert[3].color.b = 1.0;
    vert[3].color.a = 1.0;


    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); // Set render draw color to black
        SDL_RenderClear(ren);                      // Clear the renderer

        SDL_SetRenderDrawColor(ren, 0, colour, 0, 255); // Set render draw color to green
        SDL_RenderFillRect(ren, &greenSquare);          // Render the rectangle

        const int indices[] = {0, 1, 2, 2, 3, 0};
        SDL_RenderGeometry(ren, NULL, vert, vertLen, indices, 6);

        SDL_RenderPresent(ren); // Render the screen

        greenSquare.x += sin(vx * acc);
        greenSquare.y += sin(vy * acc);

        if (greenSquare.x > 640 - 100 || greenSquare.x < 0)
        {
            vx *= -1;
        }

        if (greenSquare.y > 480 - 100 || greenSquare.y < 0)
        {
            vy *= -1;
        }

        colour += 1;
        if (colour > 255)
        {
            colour = 0;
        }

        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
