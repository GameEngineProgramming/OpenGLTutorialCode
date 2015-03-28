#include <SDL.h>
#include <GL/glew.h>
#include <glcorearb.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

void err_vfatalf(const char* format, va_list args)
{
    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    exit(1);
}
void err_fatalf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    err_vfatalf(format, args);
    va_end(args);
}

void err_checkSDL(const char* msg)
{
    const char* err = SDL_GetError();
    if(*err)
        err_fatalf("SDL: %s (%s)", msg, err);
}

int main(int, char**)
{
    GLenum glerr;

    SDL_Init(SDL_INIT_VIDEO);
    err_checkSDL("Unable to init SDL video");

    /* SDL2 overrides SIGINT, so we restore it.
     * This allows us to use Ctrl+C to close the program. */
    signal(SIGINT, SIG_DFL);

    // Request an OpenGl 4.5 core profile context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Turn on double buffering with a 32bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

    SDL_Window* window = SDL_CreateWindow("SDL_Window Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    err_checkSDL("Unable to open SDL window");

    SDL_GLContext glctx = SDL_GL_CreateContext(window);
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

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
