cmake_minimum_required(VERSION 2.8)
set(PROJ_NAME Hole_in_one_app)

project(${PROJ_NAME})

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_executable(${PROJ_NAME}
    ${SOURCES}
    ${HEADERS}
)

# GLFW, OpenGL
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    target_include_directories(${PROJ_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY} glfw)
endif()

# glew
find_package(GLEW)
if(NOT GLEW_FOUND)
    find_path(GLEW_INCLUDE_DIR GL/glew.h)
    find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s)
endif()
 
target_include_directories(${PROJ_NAME} PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})

# OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_include_directories(${PROJ_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${PROJ_NAME} ${OpenCV_LIBS})
endif()

