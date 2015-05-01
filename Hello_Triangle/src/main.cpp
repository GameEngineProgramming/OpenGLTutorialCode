#include <memory>
#include <signal.h>
#include <stdio.h>
#include <ctime>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

using namespace glm;

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

    SDL_SetRelativeMouseMode(SDL_TRUE);
    err_checkSDL("Unable to set relative mouse mode");

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
    // Get a handle for our "MVP" uniform.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Model matrix : an identity matrix (model will be at the origin)
    mat4 Model = translate(mat4(1.0), vec3(2, 0, 0)) * rotate(mat4(1.0), 3.14f / 2, vec3(0, 0, 1)) * scale(mat4(1.0), vec3(0.5, 0.5, 0.5));  // Changes for each model !

    // position
    vec3 position = glm::vec3(0, 0, 5);
    // horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // vertical angle : 0, look at the horizon
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.1f;

    // Camera matrix
    mat4 View = lookAt(
        position,
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
    mat4 Projection = perspective(
        3.14f / 4,   // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
        0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        100.0f       // Far clipping plane. Keep as little as possible.
        );

    SDL_Event event;
    clock_t lastCall = clock();
    bool upKey = 0, downKey = 0, leftKey = 0, rightKey = 0;
    bool done = false;
    while(!done)
    {
        // Frame timer stuff
        clock_t thisCall = clock();
        float deltaTime = ((float)(thisCall - lastCall)) / (float)CLOCKS_PER_SEC;
        lastCall = thisCall;

        // Direction : Spherical coordinates to Cartesian coordinates conversion
        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
            );

        float componentSpeed = (float)((upKey || downKey) && (leftKey || rightKey) ? speed / sqrt(2) : speed);

        if (downKey) position -= direction * componentSpeed * deltaTime;
        if (upKey) position += direction * componentSpeed * deltaTime;

        vec3 right = vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
            );

        if (leftKey) position -= right * componentSpeed * deltaTime;
        if (rightKey) position += right * componentSpeed * deltaTime;

        // Up is both perpendicular to direction and right
        vec3 up = cross(right, direction);

        Projection = perspective(
            3.14f / 4,  // Field of view
            4.0f / 3.0f,// Aspect ratio
            0.1f,       // Near-Plane clipping distance
            100.0f      // Far-Plane clipping distance
        );

        // View/Camera matrix
        View = glm::lookAt(
            position,           // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
            );

        // Make our Model View Projection matrix
        mat4 MVP = Projection * View * Model;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);
        // Send our transformation to the currently bound shader
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    done = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    leftKey = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    rightKey = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    upKey = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    downKey = event.type == SDL_KEYDOWN;
                    break;
                default: break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (event.motion.xrel == event.motion.x && event.motion.yrel == event.motion.y) break;

                // change the direction based on mouse movement
                horizontalAngle -= mouseSpeed * deltaTime * event.motion.xrel;
                verticalAngle -= mouseSpeed * deltaTime * event.motion.yrel;
                break;
            case SDL_QUIT:
                done = true;
                break;
            default: break;
            }
        }
    }
    
    return 0;
}
