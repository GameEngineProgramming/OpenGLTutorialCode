#include <stdio.h>
#include <sstream>
#include <GL/glew.h>
#include <SDL.h>
#include "ErrorHandling.h"

void err_vfatalf(const char* format, va_list args)
{
    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    getchar();
    exit(1);
}

void err_fatalf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    err_vfatalf(format, args);
    va_end(args);
}

void err_checkGL(const char* msg)
{
    GLenum err = glGetError();

    if (err == GL_NO_ERROR) return;

    std::stringstream errorMessage;
    errorMessage << msg;
    while (err != GL_NO_ERROR)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            errorMessage << "\r\nGL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_INVALID_VALUE:
            errorMessage << "\r\nGL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_INVALID_OPERATION:
            errorMessage << "\r\nGL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorMessage << "\r\nGL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_OUT_OF_MEMORY:
            errorMessage << "\r\nGL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
            break;
        case GL_STACK_UNDERFLOW:
            errorMessage << "\r\nGL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
            break;
        case GL_STACK_OVERFLOW:
            errorMessage << "\r\nGL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
            break;
        default:
            errorMessage << "\r\nGL Unknown Error: glGetError has returned an error code not defined by the ARB as of OpenGL 4.5. Please update the err_checkGL function.";
        }
        err = glGetError();
    }
    err_fatalf(errorMessage.str().c_str());
}

bool err_clearGL()
{
    bool errorFound = false;
    while (glGetError() != GL_NO_ERROR)
    {
        errorFound = true;
    }
    return errorFound;
}

void err_checkSDL(const char* msg)
{
    const char* err = SDL_GetError();
    if (*err)
        err_fatalf("%s\r\nbSDL: %s", msg, err);
}

bool err_clearSDL()
{
    bool errorFound = SDL_GetError() != nullptr;
    SDL_ClearError();
    return errorFound;
}
