#include <memory>
#include <signal.h>
#include <stdio.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

#include "ErrorHandling.h"
#include "ShaderLoader.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

typedef struct {
    GLuint count;
    GLuint primCount;
    GLuint first;
    GLuint baseInstance;
} DrawArraysIndirectCommand;

typedef struct {
    GLuint count;
    GLuint primCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
} DrawElementsIndirectCommand;

#ifdef _WIN32
int wmain(int, char**)
#else
int main(int, char**)
#endif
{
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
        SDL_CreateWindow("3d Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
        SDL_DestroyWindow);
    err_checkSDL("Unable to open SDL window");

    std::unique_ptr<void, void(*)(void *)> context(
        SDL_GL_CreateContext(window.get()),
        SDL_GL_DeleteContext);
    err_checkSDL("Unable to create OpenGL context");
    
    SDL_GL_MakeCurrent(window.get(), context.get());
    err_checkSDL("Unable to set the current OpenGL context");

    glewExperimental = true; // Needed in core profile 
    GLenum glerr = glewInit();
    if(glerr != GLEW_OK)
        err_fatalf("Failed to initialize GLEW: %s", glewGetErrorString(glerr));
    else if(!GLEW_VERSION_4_5)
        err_fatalf("Failed to initialize GLEW: OpenGL 4.5 extensions not supported/loaded");

    err_clearGL();

    GLuint VertexArrayID;
    glCreateVertexArrays(1, &VertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    // This will identify our vertex buffer
    GLuint vertexbuffer;

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glCreateBuffers(1, &vertexbuffer);

    // Give our vertices to OpenGL.
    glNamedBufferData(vertexbuffer, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    // 1rst attribute buffer : vertices
    glEnableVertexArrayAttrib(VertexArrayID, 0);
    glVertexArrayAttribBinding(VertexArrayID, 0, 0);
    glVertexArrayAttribFormat(VertexArrayID,
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0                   // relative offset
        );
    glVertexArrayVertexBuffer(VertexArrayID, 0, vertexbuffer, 0 /* offset */, 3 * sizeof(GLfloat) /* stride */);
    err_checkGL("Loading Triangle");

    GLuint indices[3] = { 0, 1, 2 };
    // Generate a buffer for the indices
    GLuint elementbuffer;
    glCreateBuffers(1, &elementbuffer);
    glNamedBufferData(elementbuffer, 3 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    err_checkGL("Loading Element Buffer");

    DrawElementsIndirectCommand elementsCommand = {
        3, // count
        1, // primcount
        0, // firstIndex
        0, // Specifies a constant that should be added to each element of indices when chosing elements from the enabled vertex arrays
        0  // Number to start from for InstanceId
    };

    GLuint elementCommandBuffer;
    glCreateBuffers(1, &elementCommandBuffer);
    glNamedBufferData(elementCommandBuffer, sizeof(DrawElementsIndirectCommand), &elementsCommand, GL_STATIC_DRAW);

    err_checkGL("Loading Command Buffer");

    GLuint programID = LoadShaderProgramFile("basic.frag", "basic.vert");

    SDL_Event event;
    bool done = false;
    while(!done)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, elementCommandBuffer);
        glMultiDrawElementsIndirect(
            GL_TRIANGLES,                        // type of primitive to render
            GL_UNSIGNED_INT,                     // data type in the GL_ELEMENT_ARRAY_BUFFER
            0,                                   // offset in the GL_DRAW_INDIRECT_BUFFER to start at
            1,                                   // how many commands to draw from the GL_DRAW_INDIRECT_BUFFER
            sizeof(DrawElementsIndirectCommand)  // relative offset in the GL_DRAW_INDIRECT_BUFFER between command structures
        );
        err_checkGL("Triangle via glDrawArraysIndirect");

        SDL_GL_SwapWindow(window.get());

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
    
    return 0;
}
