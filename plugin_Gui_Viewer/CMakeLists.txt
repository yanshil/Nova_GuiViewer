cmake_minimum_required(VERSION 3.1)

# project(gui_viewer CXX)
set(CMAKE_CXX_STANDARD 11)

## Currently not working????undefined reference.....
#add_subdirectory(externals)

# configure_file(./camera.fs $ {CMAKE_BINARY_DIR}/bin/camera.fs COPYONLY)
# configure_file(./camera.vs $ {CMAKE_BINARY_DIR}/bin/camera.vs COPYONLY)

# add_definitions(
#     -DIMGUI_IMPL_OPENGL_LOADER_GLEW
#     )

add_library(plugin_Gui_Viewer
    # main.cpp
    triangle.cpp
    TriMesh.cpp
    ImGui_Wrapper.cpp
    Sim_Object.cpp
    )

### imgui
include_directories(./externals/imgui)
include_directories(./externals/imgui/examples)

target_compile_definitions(plugin_Gui_Viewer PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
target_compile_definitions(plugin_Gui_Viewer PUBLIC OPENGL_3D_LINUX = 1)


target_include_directories(plugin_Gui_Viewer
                           PUBLIC $ {VISUALIZER_KERNEL_INCLUDES})

target_link_libraries(plugin_Gui_Viewer
    nova::tools
    nova::geometry
)

add_library(imgui ./externals/imgui/imgui.cpp ./externals/imgui/imgui_demo.cpp ./externals/imgui/imgui_draw.cpp ./externals/imgui/imgui_widgets.cpp ./externals/imgui/examples/imgui_impl_glfw.cpp ./externals/imgui/examples/imgui_impl_opengl3.cpp)

## Target Link Libraries
target_link_libraries(plugin_Gui_Viewer
    imgui
    glfw 
    GLEW
    $ {OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories($ {GLFW_INCLUDE_DIRS})

# find_package(GLM)
if (NOT GLM_FOUND)
include_directories(libs/glm/glm)
endif(NOT GLM_FOUND)
