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
#include "Sim_Object.h"

namespace opengl_gui_viewer
{

class Viewer
{
private:
  // Window
  GLFWwindow *window;
  Camera *camera;
  // bool    draw_path;
  int ox, oy; // Origin of viewport (x, y)
  int view_width, view_height;
  int t = 0;

public:
  Viewer(GLFWwindow *window, const int ox, const int oy, const int size_x, const int size_y);
  Viewer(GLFWwindow *window, const int size_x, const int size_y); // For origin = (0,0)
  ~Viewer();

  ImGui_Wrapper *guiWrapper;
  Sim_Object *object;
  Shader shader;

  Shader &GetShader();
  Camera *GetCamera();

  void SetRenderObject(Sim_Object *object);
  void Initialize();
  void Initialize_Gui();

  void Update();
  void Resize(const int ox, const int oy, const int w, const int h);
  void DrawFrame();
  void DrawPath();
  void UpdateTest(int t);

  void Scroll_Callback();
  void Reshape_Callback();
  void Keyboard_Callback();
  void Mouse_Button_Callback();
  void Mouse_Position_Callback();

}; // class Viewer

class ViewportManager
{
private:
  int numViewport;

public:
  /**
   * Get current Viewport from mouse position 
   */
  Viewer GetCurrViewport();
};

} // namespace opengl_gui_viewer

#endif