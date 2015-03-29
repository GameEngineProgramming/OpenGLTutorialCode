#include <string>
#include <fstream>
#include "ErrorHandling.h"
#include "ShaderLoader.h"

std::string loadFile(const char* filePath)
{
    if (filePath == nullptr || filePath[0] == 0)
    {
        return "";
    }
    // Read the Vertex Shader code from the file
    std::string fileContents;
    std::ifstream fileStream(filePath, std::ios::in);
    if (fileStream.is_open())
    {
        std::string Line = "";
        while (getline(fileStream, Line))
            fileContents += "\n" + Line;
        fileStream.close();
    }

    return fileContents;
}

GLuint CompileShader(const char* shaderSource, GLuint shaderType)
{
    if (shaderSource[0] == 0) return 0;
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::string ErrorMessage(InfoLogLength <= 0 ? 1 : InfoLogLength, '\0');
    glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &ErrorMessage[0]);
    fprintf(stdout, "%s\n", &ErrorMessage[0]);

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
    return program;
}

GLuint LoadShaderProgram(GLuint fragmentShader, GLuint vertexShader)
{
    GLuint ProgramId = glCreateProgram();
    glAttachShader(ProgramId, fragmentShader);
    glAttachShader(ProgramId, vertexShader);
    glLinkProgram(ProgramId);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::string ProgramErrorMessage(InfoLogLength <= 0 ? 1 : InfoLogLength, '\0');
    glGetProgramInfoLog(ProgramId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    return ProgramId;
}
