#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdio.h>

#define MAX_PARTICLES 20
#define WINDOW_W 200
#define WINDOW_H 600

struct Entity
{
    double w, h;
    double x, y;
    double vx, vy;
    bool bBounce;

    double GetLength()
    {
        return sqrt(x * x + y * y);
    }
    
    double GetVelocity()
    {
        return sqrt(vx * vx + vy * vy);
    }
};

void Bounce(Entity& Particle);

int main(int argc, char* argv[])
{

    const int FPS = 50;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart, frameTime;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow(
        "Bouncing Balls",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, false);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_ClearError();

    bool isRunning = true;
    Uint32 nPause = 0;
    SDL_Event event;

    // Texture
    SDL_Surface* tmpSurface;

    tmpSurface = IMG_Load("assets/background.png");
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);

    SDL_FreeSurface(tmpSurface);
    // End of Texture

    // Rectangle
    SDL_Rect backgroundRect{ 0, 0, WINDOW_W, WINDOW_H };
    SDL_Rect ParticleRect[MAX_PARTICLES]{};
    Entity Particle[MAX_PARTICLES]{};
   
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle[i].w = 10;
        Particle[i].h = 10;
        Particle[i].x = 0;
        Particle[i].y = 30 * i + 10;

        ParticleRect[i].x = Particle[i].x;
        ParticleRect[i].y = Particle[i].y;
        ParticleRect[i].w = Particle[i].w;
        ParticleRect[i].h = Particle[i].h;

        Particle[i].vx = 5 + 0.08 * (i+1);
        Particle[i].vy = 0;
    }
    // End of Rectangle

    // Main Loop
    while (isRunning)
    {
        // Get Ticks
        frameStart = (Uint32)SDL_GetTicks64();

        // Get Event
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;

        /* Keyboard event */
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                isRunning = false;
                break;
            case SDLK_SPACE:
                nPause += 1;
                break;
            }
            break;
        }

        // Update

        if (nPause % 2)
        {
            ;
        }
        else
        {
            // Move Particle
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                Particle[i].x += Particle[i].vx;
                Particle[i].y += Particle[i].vy;

                ParticleRect[i].x = (double)Particle[i].x;
                ParticleRect[i].y = (double)Particle[i].y;
            }

            // Check collision between Particle and Wall
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                Bounce(Particle[i]);
            }
        }

        // Render
        if (nPause % 2)
        {
            ;
        }
        else
        {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (Particle[i].bBounce)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 253, 85, 255);
                    SDL_RenderFillRect(renderer, &ParticleRect[i]);

                    SDL_SetRenderDrawColor(renderer, 255, 253, 85, 255);
                    SDL_RenderDrawRect(renderer, &ParticleRect[i]);

                    Particle[i].bBounce = false;
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &ParticleRect[i]);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &ParticleRect[i]);
                }
            }
            SDL_RenderPresent(renderer);
        }

        // Control iteration time for Specific FPS
        frameTime = (Uint32)SDL_GetTicks64() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
        if (frameTime > 0)
        {
            // Current FPS
            printf("FPS : %.0f\n", 1000.f/((Uint32)SDL_GetTicks64() - frameStart));
        }
    }

    // Close SDL
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

void Bounce(Entity& Particle)
{
    if (Particle.x <= 0)
    {
        Particle.vx *= -1;
        double delta = 0 - Particle.x;
        Particle.x += 2 * delta;
        Particle.bBounce = true;
    }
    else if (Particle.x + Particle.w >= WINDOW_W)
    {
        Particle.vx *= -1;
        double delta = WINDOW_W - (Particle.x + Particle.w);
        Particle.x += 2 * delta;
        Particle.bBounce = true;
    }

    if (Particle.y <= 0)
    {
        Particle.vy *= -1;
        double delta = 0 - Particle.y;
        Particle.y += 2 * delta;
        Particle.bBounce = true;
    }
    else if (Particle.y + Particle.h >= WINDOW_H)
    {
        Particle.vy *= -1;
        double delta = WINDOW_H - (Particle.y + Particle.h);
        Particle.y += 2 * delta;
        Particle.bBounce = true;
    }
}
