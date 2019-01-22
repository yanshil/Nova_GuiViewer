set(ENABLE_GUIVIEWER_ORIGIN OFF CACHE BOOL "Enable Gui 0")

if(ENABLE_GUIVIEWER_ORIGIN)
project(gui_viewer CXX)
set(CMAKE_CXX_STANDARD 11)

# configure_file(./camera.fs ${CMAKE_BINARY_DIR}/bin/camera.fs COPYONLY)
# configure_file(./camera.vs ${CMAKE_BINARY_DIR}/bin/camera.vs COPYONLY)

configure_file(./camera.frag ${CMAKE_BINARY_DIR}/bin/camera.frag COPYONLY)
configure_file(./camera.vert ${CMAKE_BINARY_DIR}/bin/camera.vert COPYONLY)

add_definitions(
  -DIMGUI_IMPL_OPENGL_LOADER_GLEW
)

## Submodule: ImGUi, ...
add_subdirectory(externals)

add_executable(${PROJECT_NAME} 
    main.cpp
    OGL_World.cpp
    OGL_ViewportManager.cpp
    triangle.cpp
    TriMesh.cpp
    ImGui_Wrapper.cpp
    Sim_Object.cpp
    ShaderManager.cpp   # Nova
    ResourceManager.cpp # Nova
    Shader.cpp      # Nova
    )

target_link_libraries(${PROJECT_NAME}
    nova::tools
    nova::geometry
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
# include_directories(${CMAKE_CURRENT_LIST_DIR}/externals/imgui)
# include_directories(${CMAKE_CURRENT_LIST_DIR}/externals/imgui/examples)

target_link_libraries(${PROJECT_NAME}
    imgui
    glfw 
    GLEW
    ${OPENGL_LIBRARIES}    
    )

endif()
