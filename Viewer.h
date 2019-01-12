#ifndef OGL_VIEWER_
#define OGL_VIEWER_

#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"

namespace opengl_gui_viewer
{

class Camera
{
public:
  Camera()
      : Position(glm::vec3(0.0f, 0.0f, 3.0f)),
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        Up(glm::vec3(0.0f, 1.0f, 0.0f)),
        WorldUp(glm::vec3(0.0f, 0.0f, 1.0f)),
        Position_Delta(glm::vec3(0.0f, 0.0f, 0.0f)),
        MovementSpeed(1.0f),
        Yaw(-90.0f), Pitch(0.0f), Fov(45.0f),
        Screen_Width(800), Screen_Height(600)
  { }
  ~Camera()
  { }

  glm::vec3 Position, Front, Up, Right;
  glm::vec3 Position_Delta, Mouse_Position;
  glm::vec3 WorldUp;

  GLfloat Yaw;
  GLfloat Pitch;

  GLfloat deltaTime, lastFrame, currentFrame;
  GLfloat MovementSpeed;
  GLfloat Fov;

  glm::mat4 projection, view, model, MVP;

  int Screen_Width, Screen_Height;

  glm::mat4 GetViewMatrix() { return view; }
  glm::mat4 GetProjectionMatrix() { return projection; }
  glm::mat4 GetModelMatrix() { return model; }
  glm::mat4 GetMatrix() { return MVP; }

  // TODO
  void test_modification()
  {
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 1.0f)};
    model = glm::translate(model, cubePositions[0]);
    float angle = 20.0f * 0;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
  }

  GLfloat GetCameraSpeed()
  {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    return MovementSpeed * deltaTime;
  }

  void Update()
  {
    view = glm::lookAt(Position, Position + Front, Up);
    projection = glm::perspective(glm::radians(Fov), (float)Screen_Width / (float)Screen_Height, 0.1f, 100.0f);
    model = glm::mat4(1.0f);

    MVP = projection * view * model;
  }

  void ProcessMouseScroll(GLfloat yoffset)
  {
    if (this->Fov >= 1.0f && this->Fov <= 45.0f)
      this->Fov -= yoffset;
    if (this->Fov <= 1.0f)
      this->Fov = 1.0f;
    if (this->Fov >= 45.0f)
      this->Fov = 45.0f;
  }

};

class Viewer
{
private:
  // Window
  GLFWwindow *window;
  Shader shader;

  // Static data member.
  static Viewer viewer_;

  const unsigned int SCR_WIDTH;
  const unsigned int SCR_HEIGHT;

  Camera cam;

  // // timing
  // float deltaTime;
  // float lastFrame;

public:
  Viewer();
  ~Viewer();

  static Viewer &GetViewer();

  ImGui_Wrapper guiWrapper;

  int Initialize();
  void Main_Loop();
  void Terminate();
  void MouseWheelScrollCallback(const float y_offset);
  void MouseButtonCallback(const int button, const int action);
  void KeyboardCallback(const int key, const int action);

}; // namespace opengl_gui_viewer

} // namespace opengl_gui_viewer

#endif