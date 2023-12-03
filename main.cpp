#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

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
static int audio_open = 0;
static Mix_Chunk* g_wave = NULL;

static void CleanUp(int exitcode)
{
    if (g_wave) {
        Mix_FreeChunk(g_wave);
        g_wave = NULL;
    }
    if (audio_open) {
        Mix_CloseAudio();
        audio_open = 0;
    }

}

static int still_playing(void)
{
#ifdef TEST_MIX_CHANNELFINISHED
    return(!channel_is_done);
#else
    return(Mix_Playing(0));
#endif
}

int main(int argc, char* argv[])
{
    // Audio
    SDL_AudioSpec spec;
    int loops = 0;

    /* Initialize variables */
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;
    
    /* Initialize the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        return(255);
    }

    /* Open the audio device */
    if (Mix_OpenAudio(0, spec.format, 2, 2048) < 0) {
        SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
        CleanUp(2);
    }
    else {
        int ch = (int)spec.channels;
        Mix_QuerySpec(&spec.freq, &spec.format, &ch);
        spec.channels = ch;
        SDL_Log("Opened audio at %d Hz %d bit%s %s", spec.freq,
            (spec.format & 0xFF),
            (SDL_AUDIO_ISFLOAT(spec.format) ? " (float)" : ""),
            (spec.channels > 2) ? "surround" :
            (spec.channels > 1) ? "stereo" : "mono");
        if (loops) {
            SDL_Log(" (looping)\n");
        }
        else {
            putchar('\n');
        }
    }
    audio_open = 1;

    const char* file_sound = "sounds/hit_enemy.wav";
    g_wave = Mix_LoadWAV(file_sound);
    if (g_wave == NULL)
    {
        SDL_Log("Couldn't load %s: %s\n", file_sound, SDL_GetError());
    }

    Mix_PlayChannel(0, g_wave, loops);
    while (still_playing())
    {

    }
    CleanUp(0);

    // End of Audio

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
            // printf("FPS : %.0f\n", 1000.f/((Uint32)SDL_GetTicks64() - frameStart));
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
