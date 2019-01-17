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
  Camera *camera;
  Sim_Object *object;
  Shader *shader;
  bool draw_path;
  int ox, oy; // Origin of viewport (x, y)
  int view_width, view_height;

public:
  Viewport();
  ~Viewport();

  // ImGui_Wrapper *guiWrapper;

  // ======== Getter ============
  Shader *GetShader();
  Camera *GetCamera();

  // ========== Setter ================
  void SetCamera(Camera *camera);
  void SetShader(Shader *shader);
  void SetRenderObject(Sim_Object *object);
  void Resize(const int ox, const int oy, const int w, const int h);

  // ========== Initializer ============
  void Initialize();

  // ======= Frame Update  Logic ========
  void Update();
  void DrawFrame();

  // ====== Controller Callback =============
  bool InsideCurrView(glm::vec2 mouse_pos);

}; // class Viewport

/** Class ViewportManager
 * 
 * 
 */
class ViewportManager
{
private:
  GLFWwindow *window;
  int t = 0;

public:
  enum ViewportConfiguration
  {
    SINGLE_VIEWPORT,
    DUAL_VIEWPORT
  };
  ViewportConfiguration _currconfig;

  std::vector<Viewport> viewport_list;
  ImGui_Wrapper * gui;

  ViewportManager(GLFWwindow *window);
  ~ViewportManager();

  void SetWindowGeometry(const int global_x, const int global_y);
  void InitializeViewports(const int global_x, const int global_y);
  void InitializeGui(GLFWwindow *window, Sim_Object *object);
  void ViewportSetting(Sim_Object *object);
  void Update();
  void DrawFrame();

  Viewport &GetViewport(int i);

  unsigned int NumViewports();

  // ============ Control ==============
  Viewport &GetCurrViewport();
  void Scroll_Callback(double yoffset);
  // void Reshape_Callback();
  void Keyboard_Callback();
  void Mouse_Button_Callback(int key, int action, int mode);
  void Mouse_Position_Callback();

  //====================================
  void UpdateTest(int t);
  void DrawPath();

};

} // namespace opengl_gui_viewer

#endif