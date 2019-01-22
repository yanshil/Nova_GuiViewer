## IMGUI
include_directories(imgui)
include_directories(imgui/examples)

set(IMGUI_HEADER
  imgui/imgui.h
  imgui/imconfig.h
  imgui/imgui_internal.h
  imgui/imstb_rectpack.h
  imgui/imstb_textedit.h
  imgui/imstb_truetype.h
  imgui/examples/imgui_impl_glfw.h
  imgui/examples/imgui_impl_opengl3.h
)
set(IMGUI_SOURCE
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/examples/imgui_impl_glfw.cpp
  imgui/examples/imgui_impl_opengl3.cpp
)

add_library(imgui
  ${IMGUI_HEADER}
  ${IMGUI_SOURCE}
)
