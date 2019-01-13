// @yanshi
//#####################################################################
// Class OGL_Viewer
//######################################################################
#ifndef OGL_VIEWER_
#define OGL_VIEWER_

#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"
#include "Camera.h"

namespace opengl_gui_viewer
{

class Viewer
{
private:
  // Window
  GLFWwindow *window;
  Camera * camera;

  int view_width;
  int view_height;

public:
  // Viewer();
  Viewer(GLFWwindow *window, const int size_x, const int size_y);
  ~Viewer();

  ImGui_Wrapper guiWrapper;
  Shader shader;

  Shader &GetShader();
  Camera * GetCamera();

  int Initialize();

  void Update();
  void DrawFrame();
}; // namespace opengl_gui_viewer

} // namespace opengl_gui_viewer

#endif