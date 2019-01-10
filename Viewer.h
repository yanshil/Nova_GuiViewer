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

class Viewer
{
  private:
    // Window
    GLFWwindow *window;
    
    
    const unsigned int SCR_WIDTH;
    const unsigned int SCR_HEIGHT;

    // Orbit Control (Only works for Object view?)
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    bool firstMouse;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float fov;

    // timing
    float deltaTime;
    float lastFrame;

    Shader shader;

  public:
    Viewer();
    ~Viewer();

    ImGui_Wrapper guiWrapper;

    int Initialize();
    void Main_Loop();
    void Terminate();
    void MouseWheelScrollCallback(const float y_offset);
    void MouseDragCallback(const float x_pos, const float y_pos);
    void KeyboardCallback(const int key, const int action);

}; // namespace opengl_gui_viewer

} // namespace opengl_gui_viewer

#endif