#include "ViewportManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"
#include "Camera.h"
#include "shader.h"
#include "Sim_Object.h"

using namespace opengl_gui_viewer;

ViewportManager::ViewportManager()
    : _currentConfiguration(VM_NOT_CONFIGURED), gui(nullptr)
{
}

ViewportManager::~ViewportManager()
{
}

void ViewportManager::ConfigureViewports(ViewportConfiguration vc)
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

    SetWindowGeometry(_global_viewport.width, _global_viewport.height);
}

void ViewportManager::SetWindowGeometry(int width, int height)
{
    // Update the global window viewport
    _global_viewport.x = 0;
    _global_viewport.y = 0;
    _global_viewport.width = width;
    _global_viewport.height = height;

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

void ViewportManager::UpdateViewportGeometry(unsigned int v)
{
    Viewport &viewport = _viewports.at(v);
    viewport.camera->SetSize(viewport.width, viewport.height);

    // TODO
    viewport.shader->initializeFromFile("camera.vs", "camera.fs");
    viewport.shader->use();
}
void ViewportManager::SetRenderObject(Sim_Object *object)
{
    for (int v = 0; v < this->_viewports.size(); v++)
        _viewports[v].object = object;
}

void ViewportManager::Update()
{
    for (int v = 0; v < this->_viewports.size(); v++)
        _viewports[v].camera->Update();

    if (gui)
        gui->UIFrame();
}

void ViewportManager::DrawFrame()
{
    for (int v = 0; v < this->_viewports.size(); v++)
    {
        _viewports[v].shader->setMat4("projection", _viewports[v].camera->GetProjectionMatrix());
        _viewports[v].shader->setMat4("view", _viewports[v].camera->GetViewMatrix());
        _viewports[v].shader->setMat4("model", _viewports[v].camera->GetModelMatrix());
        glViewport(_viewports[v].x, _viewports[v].y, _viewports[v].width, _viewports[v].height);

        glDrawElements(GL_TRIANGLES, 3 * _viewports[v].object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
    }

    if (gui)
    {
        gui->ApplyDisplayOption();
        gui->Render();
    }
}

void ViewportManager::Gui_Initialize(GLFWwindow *window, Sim_Object *object)
{

    gui = new ImGui_Wrapper();
    gui->SetRenderObject(object);
    gui->Initialize(window);
    gui->test_GenObject();
    gui->InitBuffer();
}