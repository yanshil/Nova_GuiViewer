set(ENABLE_GUIVIEWER OFF CACHE BOOL "Enable Gui Modular")
if(ENABLE_GUIVIEWER)
  add_library(plugin_GuiViewer_1
    main.cpp
    ../triangle.cpp
    ../TriMesh.cpp
    ../ImGui_Wrapper.cpp
    ../Sim_Object.cpp
    #../Viewer.cpp
    )

  if(WIN32)
    target_compile_definitions(plugin_GuiViewer_1 PUBLIC OPENGL_3D_WIN32=1)
  endif()
  
  if(UNIX)
    target_compile_definitions(plugin_GuiViewer_1 PUBLIC OPENGL_3D_LINUX=1)
  endif()

  add_definitions(
  -DIMGUI_IMPL_OPENGL_LOADER_GLEW
  )

  target_include_directories(plugin_GuiViewer_1
                             PUBLIC ${VISUALIZER_KERNEL_INCLUDES})
  
  
  target_link_libraries(plugin_GuiViewer_1
    nova::tools
    nova::geometry
    )
  
  target_compile_features(plugin_GuiViewer_1
    PRIVATE
    cxx_auto_type
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
    include_directories(../externals/imgui)
    include_directories(../externals/imgui/examples)
    
    add_library(imgui ../externals/imgui/imgui.cpp ../externals/imgui/imgui_demo.cpp ../externals/imgui/imgui_draw.cpp ../externals/imgui/imgui_widgets.cpp ../externals/imgui/examples/imgui_impl_glfw.cpp ../externals/imgui/examples/imgui_impl_opengl3.cpp )
    
    ## Target Link Libraries
    target_link_libraries(plugin_GuiViewer_1
        imgui
        glfw 
        GLEW
        ${OPENGL_LIBRARIES}    
        )

endif()
