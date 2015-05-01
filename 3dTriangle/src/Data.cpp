#include <fstream>
#include <SDL.h>
#include "Data.h"

std::string DataPathToFilePath(const char *path)
{
    static std::string dataPath;
    if (dataPath.empty()){
        char *basePath = SDL_GetBasePath();
        SDL_assert(basePath != nullptr);

        dataPath = basePath;
        SDL_free(basePath);
        
        //We replace the last bin/ with res/ to get the the resource path
        size_t pos = dataPath.rfind("bin");
        dataPath = dataPath.substr(0, pos) + "data/";
    }
    return dataPath + path;
}

std::string loadFile(const char* filePath)
{
    if (filePath == nullptr || filePath[0] == 0)
    {
        return "";
    }
    // Read the Vertex Shader code from the file
    std::string fileContents;
    std::ifstream fileStream(filePath, std::ios::in);

    // if the path is not absolute, try the data directory
    if (!fileStream.good())
    {
        fileStream = std::ifstream(DataPathToFilePath(filePath), std::ios::in);
    }

    // If we still can't find the file, something has gone wrong
    SDL_assert(fileStream.is_open());

    // Read the contents
    std::string Line = "";
    while (getline(fileStream, Line))
        fileContents += "\n" + Line;
    fileStream.close();

    return fileContents;
}
