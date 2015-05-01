#ifndef __ShaderLoader_h__
#define __ShaderLoader_h__

#include <GL/glew.h>
#include <GL/GL.h>

// Load a fragment shader from a file
GLuint LoadFragmentShaderFile(const char* filePath);
// Load a fragment shader from source
GLuint LoadFragmentShaderSource(const char* source);

// Load a vertex shader from a file
GLuint LoadVertexShaderFile(const char* filePath);
// Load a vertex shader from source
GLuint LoadVertexShaderSource(const char* source);

// Load a shader program from the source files of its component shaders
// Shaders will be deleted by OpenGL upon deletion of the returned shader program
GLuint LoadShaderProgramFile(const char* fragmentFilePath, const char* vertexFilePath);
// Load a shader program from the source of its component shaders
// Shaders will be deleted by OpenGL upon deletion of the returned shader program
GLuint LoadShaderProgramSource(const char* fragmentSource, const char* vertexSource);
// Load a shader program from the its component shaders
// Users are expected to handle their own shader deletion logic
GLuint LoadShaderProgram(GLuint fragmentShader, GLuint vertexShader);

#endif
