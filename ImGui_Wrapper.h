// Copyright 2019 Yanshi
//#####################################################################
// Class ImGui_Wrapper
//######################################################################
#ifndef IMGUI_WRAPPER_H_
#define IMGUI_WRAPPER_H_

#include "Sim_Object.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace opengl_gui_viewer
{

class ImGui_Wrapper
{
public:
  ImGui_Wrapper();
  ~ImGui_Wrapper();

  void Initialize(GLFWwindow *window);
  void Render();

  Sim_Object *main_object;
  GLFWwindow *_window;

  GLuint VBO, VAO, EBO;

  void InitUI();
  void DisplayUI();

  void Setup_Mesh();
  void Draw();

private:
  void DisplayCubeModule();
  void DisplayHoleModule();
  void DisplayGenerateModule();
  void DisplayAnimation();
};

} // namespace opengl_gui_viewer

#endif // IMGUI_WRAPPER_H_
