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

  bool InsideCurrView();

}; // class Viewer

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

  std::vector<Viewer> viewport_list;

  ViewportManager(GLFWwindow * window);
  ~ViewportManager();

  void SetWindowGeometry(const int global_x, const int global_y);
  void InitializeViewports(const int global_x, const int global_y);
  void ViewportSetting(Sim_Object * object);
  void Update();
  void DrawFrame();

  Viewer & GetViewer(int i);

  unsigned int NumViewports();

  // ============ Control ==============
  Viewer & GetCurrViewport();
  void Scroll_Callback(double yoffset);
  // void Reshape_Callback();
  void Keyboard_Callback();
  void Mouse_Button_Callback(int key, int action, int mode);
  void Mouse_Position_Callback();
};

} // namespace opengl_gui_viewer

#endif