cmake_minimum_required(VERSION 3.1)

project(foo CXX)
set(CMAKE_CXX_STANDARD 11)

#add_subdirectory(externals)

add_executable(${PROJECT_NAME} 
    main.cpp
    triangle.cpp
    TriMesh.cpp
    )

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# find_package(GLM)
if(NOT GLM_FOUND)
    include_directories(libs/glm/glm)
endif(NOT GLM_FOUND)


## Target Link Libraries
target_link_libraries(${PROJECT_NAME} 
    glfw 
    GLEW 
    ${OPENGL_LIBRARIES}
    )