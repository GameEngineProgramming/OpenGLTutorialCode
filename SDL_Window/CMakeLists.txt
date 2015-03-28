cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMakeModules/")

project(SDL_Window)

# we'll use SDL2, GLEW and OpenGL
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS GL)

# find every source file in the `src` and `ext` directories
file(GLOB_RECURSE sources src/*.c src/*.cpp ext/*.c ext/*.cpp)

include_directories(ext ${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

# we'll make a new executable file, named the same as the project
add_executable(${PROJECT_NAME} ${sources})
# link with SDL and OpenGL
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY})

# in most Unix-y systems, we'll also need to link with libm
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} m)
endif()

# copy the data over
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
