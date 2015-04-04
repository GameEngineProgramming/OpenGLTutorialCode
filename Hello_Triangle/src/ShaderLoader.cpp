#include <string>
#include "Data.h"
#include "ErrorHandling.h"
#include "ShaderLoader.h"

GLuint CompileShader(const char* shaderSource, GLuint shaderType)
{
    err_checkGL("Before compiling shader");

    if (shaderSource[0] == 0) return 0;
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::string InfoLog(InfoLogLength <= 0 ? 1 : InfoLogLength, '\0');
    glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &InfoLog[0]);
    if (Result == GL_TRUE) // compiler warnings
        fprintf(stdout, "%s\n", &InfoLog[0]);
    else // compiler errors
        err_fatalf("%s\n", &InfoLog[0]);

    err_checkGL("Compiling shader");

    return shaderId;
}

GLuint LoadFragmentShaderFile(const char* filePath)
{
    std::string fileContents = loadFile(filePath);
    return LoadFragmentShaderSource(fileContents.c_str());
}

GLuint LoadFragmentShaderSource(const char* source)
{
    return CompileShader(source, GL_FRAGMENT_SHADER);
}

GLuint LoadVertexShaderFile(const char* filePath)
{
    std::string fileContents = loadFile(filePath);
    return LoadVertexShaderSource(fileContents.c_str());
}

GLuint LoadVertexShaderSource(const char* source)
{
    return CompileShader(source, GL_VERTEX_SHADER);
}

GLuint LoadShaderProgramFile(const char* fragmentFilePath, const char* vertexFilePath)
{
    std::string fragmentFileContents = loadFile(fragmentFilePath);
    std::string vertexFileContents = loadFile(vertexFilePath);
    return LoadShaderProgramSource(fragmentFileContents.c_str(), vertexFileContents.c_str());
}

GLuint LoadShaderProgramSource(const char* fragmentSource, const char* vertexSource)
{
    GLuint fragmentShader = LoadFragmentShaderSource(fragmentSource);
    GLuint vertexShader = LoadVertexShaderSource(vertexSource);
    GLuint program = LoadShaderProgram(fragmentShader, vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    err_checkGL("Marking shader for deletion");
    return program;
}

GLuint LoadShaderProgram(GLuint fragmentShader, GLuint vertexShader)
{
    err_checkGL("Before linking shader program");

    GLuint ProgramId = glCreateProgram();
    glAttachShader(ProgramId, fragmentShader);
    glAttachShader(ProgramId, vertexShader);
    glLinkProgram(ProgramId);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::string InfoLog(InfoLogLength <= 0 ? 1 : InfoLogLength, '\0');
    glGetProgramInfoLog(ProgramId, InfoLogLength, NULL, &InfoLog[0]);
    if (Result == GL_TRUE) // linker warnings
        fprintf(stdout, "%s\n", &InfoLog[0]);
    else // linker errors
        err_fatalf("%s\n", &InfoLog[0]);

    err_checkGL("Linking shader program");

    return ProgramId;
}
