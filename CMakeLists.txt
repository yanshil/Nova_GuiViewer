cmake_minimum_required(VERSION 3.1)

project(foo CXX)
set(CMAKE_CXX_STANDARD 11)

## Currently not working???? undefined reference.....
#add_subdirectory(externals)

add_definitions(
  -DIMGUI_IMPL_OPENGL_LOADER_GLEW
)

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

### imgui
include_directories(./externals/imgui)
include_directories(./externals/imgui/examples)

add_library(imgui ./externals/imgui/imgui.cpp ./externals/imgui/imgui_demo.cpp ./externals/imgui/imgui_draw.cpp ./externals/imgui/imgui_widgets.cpp ./externals/imgui/examples/imgui_impl_glfw.cpp ./externals/imgui/examples/imgui_impl_opengl3.cpp)

## Target Link Libraries
target_link_libraries(${PROJECT_NAME}
    imgui
    glfw 
    GLEW
    ${OPENGL_LIBRARIES}    
    )
