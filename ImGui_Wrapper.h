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

  void UIFrame();
  void DisplayUI();

  void test_GenObject();
  void InitBuffer();
  void NewBuffer();
  // Listen for every frame
  void ApplyDisplayOption();

  // ImGui Orbit Control
  void MouseWheelScrollCallback(float yoffset);

private:
  // Mouse states.
  bool _mouse_pressed_[3];
  float _mouse_wheel_;

  // Cube Paras
  double tmpParas_cube[3];

  void DisplayCubeModule();
  void DisplayHoleModule();
  void DisplayOptionModule();
  void DisplayGenerateModule();
  void DisplayAnimation();
};

} // namespace opengl_gui_viewer

#endif // IMGUI_WRAPPER_H_
