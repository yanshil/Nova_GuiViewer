// @yanshi
//#####################################################################
/// Class OGL_World
/// 
/// World.h glfw main windows and glfw IO bindings.
/// Also parse the Renderable Object pointer to its childrens.
//######################################################################
#ifndef OGL_WORLD_
#define OGL_WORLD_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Viewport.h"
// #include "ViewportManager.h"
#include "Sim_Object.h" // Something should be in Application Factory

namespace opengl_gui_viewer
{

/** Class World
 * 
 *  Manage glfwWindowm, viewports and IO control.
 * 
 */
class World
{
private:
  GLFWwindow *window;
  ViewportManager *viewports;
  int window_width, window_height;

  Sim_Object object;

  // Initialize
  void Initialize_Viewports();

public:
  World(int size_x, int size_y);
  ~World();

  void Initialize();
  void Main_Loop();

  // === Callback ===
  static void Close_Callback(GLFWwindow *window);
  static void Scroll_Callback(GLFWwindow *window, double xoffset, double yoffset);
  static void Reshape_Callback(GLFWwindow *window, int w, int h);
  static void Keyboard_Callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  static void Mouse_Button_Callback(GLFWwindow *window, int button, int action, int mods);
  static void Mouse_Position_Callback(GLFWwindow *window, double x, double y);
};

} // namespace opengl_gui_viewer
#endif