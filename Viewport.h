// @yanshi
//#####################################################################
/// Class OGL_Viewport
///
/// Viewport Manager to mange different viewport's Initialize, Update, DrawFrame.
/// * For ImGui Wrapper (Cannot have multiple ImGui Rendering): Always Initialize and Render in viewport[0].
/// * A pointer of the Renderable Object is parsed from World->viewport(s)->ImGui(Interactive)
/// * Every viewport has its own camera. Currently all viewport share 
///   a single orject (by parsing hte object's pointer)
//######################################################################
#ifndef OGL_Viewport_
#define OGL_Viewport_

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

/** Viewport
 * 
 *  A Single Viewport with its camera control.
 */
class Viewport
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
  Viewport(GLFWwindow *window, const int ox, const int oy, const int size_x, const int size_y);
  Viewport(GLFWwindow *window, const int size_x, const int size_y); // For origin = (0,0)
  ~Viewport();

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

  bool InsideCurrView();

}; // class Viewport

/** Class ViewportManager
 * 
 * 
 */
class ViewportManager
{
private:
  GLFWwindow * window;
public:
  enum ViewportConfiguration {SINGLE_VIEWPORT, DUAL_VIEWPORT};
  ViewportConfiguration _currconfig;

  std::vector<Viewport> viewport_list;

  ViewportManager(GLFWwindow * window);
  ~ViewportManager();

  void SetWindowGeometry(const int global_x, const int global_y);
  void InitializeViewports(const int global_x, const int global_y);
  void ViewportSetting(Sim_Object * object);
  void Update();
  void DrawFrame();

  Viewport & GetViewport(int i);

  unsigned int NumViewports();

  // ============ Control ==============
  Viewport & GetCurrViewport();
  void Scroll_Callback(double yoffset);
  // void Reshape_Callback();
  void Keyboard_Callback();
  void Mouse_Button_Callback(int key, int action, int mode);
  void Mouse_Position_Callback();
};

} // namespace opengl_gui_viewer

#endif