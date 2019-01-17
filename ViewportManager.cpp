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