#ifndef OGL_VIEWER_
#define OGL_VIEWER_

#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"

namespace opengl_gui_viewer
{

class Viewer
{
private:
  // Window
  GLFWwindow *window;
  Shader shader;

  // Static data member.
  static Viewer viewer_;

  unsigned int SCR_WIDTH;
  unsigned int SCR_HEIGHT;

  // Orbit Control (Only works for Object view?)
  glm::vec3 local_camera_pos;
  glm::vec3 local_camera_front;
  glm::vec3 local_camera_up;

  glm::vec3 global_camera_pos;
  glm::vec3 global_camera_front;
  glm::vec3 global_camera_up;

  bool firstMouse;
  float yaw;
  float pitch;
  float lastX;
  float lastY;
  float fov;

  // timing
  float deltaTime;
  float lastFrame;
  int t = 0;
  int t_copy = 0;
public:
  Viewer();
  ~Viewer();

  static Viewer &GetViewer();

  ImGui_Wrapper guiWrapper;

  int Initialize();
  void Main_Loop();
  void Terminate();
  void MouseWheelScrollCallback(const float y_offset);
  void MouseDragCallback(const float x_pos, const float y_pos);
  void KeyboardCallback(const int key, const int action);
  void SizeCallback(const int width, const int height);

  void LinearUpdateTest(int t);


}; // namespace opengl_gui_viewer

} // namespace opengl_gui_viewer

#endif