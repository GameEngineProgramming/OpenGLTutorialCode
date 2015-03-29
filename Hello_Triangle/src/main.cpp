#include <memory>
#include <signal.h>

#include <GL/glew.h> // GLEW must always come before glcorearb
#include <glcorearb.h>
#include <glm/glm.hpp>
#include <SDL.h>

#include "ErrorHandling.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

#ifdef _WIN32
int wmain(int, char**)
#else
int main(int, char**)
#endif
{
    GLenum glerr;

    SDL_Init(SDL_INIT_VIDEO);
    err_checkSDL("Unable to init SDL video");
    atexit(SDL_Quit);

    /* SDL2 overrides SIGINT, so we restore it.
     * This allows us to use Ctrl+C to close the program. */
    signal(SIGINT, SIG_DFL);

    // Request an OpenGl 4.5 core profile context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    std::unique_ptr<SDL_Window, void(*)(SDL_Window *)> window(
        SDL_CreateWindow("Hello Triangle!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
        SDL_DestroyWindow);
    err_checkSDL("Unable to open SDL window");

    std::unique_ptr<void, void(*)(void *)> context(
        SDL_GL_CreateContext(window.get()),
        SDL_GL_DeleteContext);
    err_checkSDL("Unable to create OpenGL context");

    glerr = glewInit();
    if(glerr != GLEW_OK)
        err_fatalf("Failed to initialize GLEW: %s", glewGetErrorString(glerr));
    else if(!GLEW_VERSION_4_5)
        err_fatalf("Failed to initialize GLEW: OpenGL 4.5 extensions not supported/loaded");

    SDL_Event event;
    bool done = false;
    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    done = true;
                    break;
                default: break;
                }
            case SDL_QUIT:
                done = true;
                break;
            default: break;
            }
        }
    }
    
    SDL_GL_SwapWindow(window.get());
    return 0;
}
