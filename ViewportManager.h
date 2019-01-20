#ifndef OGL_VIEWPORT_MANAGER_H__
#define OGL_VIEWPORT_MANAGER_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"
#include "Camera.h"
#include "shader.h"
#include "Sim_Object.h"

namespace opengl_gui_viewer
{

class ViewportManager
{
    struct Viewport;

  public:
    enum ViewportConfiguration
    {
        VM_NOT_CONFIGURED,
        VM_SINGLE_VIEWPORT,
        VM_DUAL_VIEWPORT
    }; //, VM_QUAD_VIEWPORT };

    std::vector<Viewport> _viewports;
    ImGui_Wrapper *gui;

    ViewportManager();
    ~ViewportManager();

    // Global Viewports Sate
    void ConfigureViewports(ViewportConfiguration vc);

    void SetWindowGeometry(int width, int height);
    
    unsigned int NumViewports() { return _viewports.size(); }
    unsigned int CurrentViewport();

    void UpdateViewportGeometry(unsigned int v);
    void SetRenderObject(Sim_Object *object);

    // ======= Frame Update  Logic ========
    void Update();

    void DrawFrame();

    // ========= ImGui =========
    void Gui_Initialize(GLFWwindow *window, Sim_Object *object);

  private:
    struct Viewport
    {
        Viewport() = default;
        Viewport(Viewport &&) = default;
        Viewport(const Viewport &) = delete;
        Viewport &operator=(const Viewport &) = delete;
        int x;
        int y;
        int width;
        int height;
        float clipNear;
        float clipFar;
        std::unique_ptr<Camera> camera;
        std::unique_ptr<Shader> shader;
        Sim_Object *object;
    };

    ViewportConfiguration _currentConfiguration;
    Viewport _global_viewport;
};

} // namespace opengl_gui_viewer

#endif