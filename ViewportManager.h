//====================================================
// @Yanshi
// Modified from
// https://github.com/OrionQuest/Nova_Examples/blob/master/opengl/ViewportManager.h
//====================================================
#ifndef OGL_VIEWPORT_MANAGER_H__
#define OGL_VIEWPORT_MANAGER_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"
#include "Camera.h"
#include "ShaderManager.h"
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
    std::unique_ptr<ShaderManager> _shaderman;
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

    // ======== Callbak =======

    void Scroll_Callback(double yoffset);
    void Keyboard_Callback(GLFWwindow *window, int key, int action, int mode);
    void Mouse_Button_Callback(int button, int action, int mods);
    void Mouse_Position_Callback(double x, double y);

    // TODO: @Haozhe update test
    //====================================
    int t = 0;
    void UpdateTest(int t);
    void DrawPath();

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
    unsigned int _activeViewport;
};

} // namespace opengl_gui_viewer

#endif