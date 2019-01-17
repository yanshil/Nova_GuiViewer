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
    ImGui_Wrapper *guiWrapper;

    ViewportManager()
        : _currentConfiguration(VM_NOT_CONFIGURED), guiWrapper(nullptr)
    {
    }
    ~ViewportManager()
    {
    }

    // Global Viewports Sate
    void ConfigureViewports(ViewportConfiguration vc)
    {
        // Setup viewports based on the specific configuration layout.

        int numOldViewports = 0;
        int numNewViewports = 0;

        switch (_currentConfiguration)
        {
        case VM_SINGLE_VIEWPORT:
            numOldViewports = 1;
            break;
        case VM_DUAL_VIEWPORT:
            numOldViewports = 2;
            break;
        default:
            numOldViewports = 0;
            break;
        }
        switch (vc)
        {
        case VM_SINGLE_VIEWPORT:
            numNewViewports = 1;
            break;
        case VM_DUAL_VIEWPORT:
            numNewViewports = 2;
            break;
        default:
            numNewViewports = 0;
            break;
        }

        for (int addedViewports = 0; addedViewports < std::max(0, numNewViewports - numOldViewports); addedViewports++)
        {
            Viewport viewport;
            // TODO: Also setting camera, shader and Object
            viewport.camera = std::unique_ptr<Camera>(new Camera());
            viewport.shader = std::unique_ptr<Shader>(new Shader());

            _viewports.push_back(std::move(viewport));
        }

        for (int removedViewports = 0; removedViewports < std::max(0, numOldViewports - numNewViewports); removedViewports++)
        {
            _viewports.pop_back();
        }

        _currentConfiguration = vc;

        std::cout << 222 << std::endl;

        std::cout <<_global_viewport.width << std::endl;
        // std::cout <<_global_viewport.height << std::endl;
    
        SetWindowGeometry(_global_viewport.width, _global_viewport.height);
    }
    void SetWindowGeometry(int width, int height)
    {
        std::cout << "What??" << std::endl;
        std::cout << _global_viewport.x << std::endl;
        // Update the global window viewport
        _global_viewport.x = 0;
        _global_viewport.y = 0;
        _global_viewport.width = width;
        _global_viewport.height = height;

        std::cout << "222" << std::endl;

        // Now update all viewports based on the active configuration
        switch (_currentConfiguration)
        {
        case VM_SINGLE_VIEWPORT:
        {
            Viewport &viewport = _viewports[0]; // This mode only has one viewport
            // ... and its pretty easy to do so.
            viewport.x = _global_viewport.x;
            viewport.y = _global_viewport.y;
            viewport.width = _global_viewport.width;
            viewport.height = _global_viewport.height;
        }
        break;
        case VM_DUAL_VIEWPORT:
        {
            Viewport &viewportA = _viewports[0];
            viewportA.x = _global_viewport.x;
            viewportA.y = _global_viewport.y;
            viewportA.width = _global_viewport.width / 2;
            viewportA.height = _global_viewport.height;

            Viewport &viewportB = _viewports[1];
            viewportB.x = _global_viewport.x + _global_viewport.width / 2;
            viewportB.y = _global_viewport.y;
            viewportB.width = _global_viewport.width / 2;
            viewportB.height = _global_viewport.height;
        }
        break;
        default:
            break;
        }

        for (int v = 0; v < this->_viewports.size(); v++)
            // Next, we need to manually update each controller based on its viewport's
            // unique geometry
            UpdateViewportGeometry(v);
    }
    unsigned int NumViewports() { return _viewports.size(); }
    unsigned int CurrentViewport();

    void UpdateViewportGeometry(unsigned int v)
    {
        Viewport &viewport = _viewports.at(v);
        std::cout << &viewport.camera << std::endl;
        viewport.camera->SetSize(viewport.width, viewport.height);

        viewport.shader->initializeFromFile("camera.vs", "camera.fs");
        viewport.shader->use();
    }
    void SetRenderObject(Sim_Object *object)
    {
        for (int v = 0; v < this->_viewports.size(); v++)
            _viewports[v].object = object;
    }

    // ======= Frame Update  Logic ========
    void Update()
    {
        for (int v = 0; v < this->_viewports.size(); v++)
            _viewports[v].camera->Update();

        if (guiWrapper)
            guiWrapper->UIFrame();
    }
    void DrawFrame()
    {
        for (int v = 0; v < this->_viewports.size(); v++)
        {
            _viewports[v].shader->setMat4("projection", _viewports[v].camera->GetProjectionMatrix());
            _viewports[v].shader->setMat4("view", _viewports[v].camera->GetViewMatrix());
            _viewports[v].shader->setMat4("model", _viewports[v].camera->GetModelMatrix());
            glViewport(_viewports[v].x, _viewports[v].y, _viewports[v].width, _viewports[v].height);

            glDrawElements(GL_TRIANGLES, 3 * _viewports[v].object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
        }

        if (guiWrapper)
        {
            guiWrapper->ApplyDisplayOption();
            guiWrapper->Render();
        }
    }

    // ========= ImGui =========
    void Gui_Setting(GLFWwindow *window, Sim_Object *object)
    {
        
        guiWrapper = new ImGui_Wrapper();
        std::cout << "Initialize Gui" << std::endl;
        guiWrapper->SetRenderObject(object);
        guiWrapper->Initialize(window);
        guiWrapper->test_GenObject();
        guiWrapper->InitBuffer();
    }

  private:
    struct Viewport
    {
        Viewport() = default;
        Viewport(Viewport &&) = default;
        Viewport(const Viewport &) = delete;
        Viewport &operator=(const Viewport &) = delete;
        int x = 0;
        int y = 0;
        int width = 800;
        int height = 600;
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