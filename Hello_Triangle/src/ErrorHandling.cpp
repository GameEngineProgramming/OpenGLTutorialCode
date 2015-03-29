#include <stdio.h>
#include <SDL.h>
#include "ErrorHandling.h"

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
    if (*err)
        err_fatalf("SDL: %s (%s)", msg, err);
}
