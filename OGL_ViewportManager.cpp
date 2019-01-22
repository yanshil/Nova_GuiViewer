#include "OGL_ViewportManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGui_Wrapper.h"
#include "OGL_Camera.h"
#include "Sim_Object.h"

using namespace opengl_gui_viewer;

ViewportManager::ViewportManager()
    : _currentConfiguration(VM_NOT_CONFIGURED), gui(nullptr)
{
    _shaderman = std::unique_ptr<ShaderManager>(new ShaderManager());
    _shaderman->PrependSearchPaths("./bin");
    _shaderman->LoadFromFiles("camera");
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
        viewport.shader = _shaderman->GetShader("camera");
        // viewport.shader = std::unique_ptr<Shader>(new Shader());

        _viewports.push_back(std::move(viewport));
    }

    for (int removedViewports = 0; removedViewports < std::max(0, numOldViewports - numNewViewports); removedViewports++)
    {
        _viewports.pop_back();
    }

    _currentConfiguration = vc;
    _activeViewport = 0;

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

    // TODO
    _viewports[0].camera->ConfigureCamera(Camera::LOCAL_CAMERA);
    if (_viewports.size() > 1)
        _viewports[1].camera->ConfigureCamera(Camera::GLOBAL_CAMERA);
}

unsigned int ViewportManager::CurrentViewport()
{
    glm::vec2 mouse_pos = gui->GetMousePosition();

    for (int i = 0; i < _viewports.size(); i++)
    {
        Viewport &viewport = _viewports.at(i);
        if ((mouse_pos.x >= viewport.x) &
            (mouse_pos.x <= viewport.x + viewport.width) &
            (mouse_pos.y >= viewport.y) &
            (mouse_pos.y <= viewport.y + viewport.height))
            _activeViewport = i;
    }
    return _activeViewport;
}

void ViewportManager::UpdateViewportGeometry(unsigned int v)
{
    Viewport &viewport = _viewports.at(v);
    viewport.camera->SetSize(viewport.width, viewport.height);

    // TODO
    // viewport.shader->initializeFromFile("camera.vs", "camera.fs");
    // viewport.shader->use();
    viewport.shader->Use();
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
        // _viewports[v].shader->setMat4("projection", _viewports[v].camera->GetProjectionMatrix());
        // _viewports[v].shader->setMat4("view", _viewports[v].camera->GetViewMatrix());
        // _viewports[v].shader->setMat4("model", _viewports[v].camera->GetModelMatrix());
        _viewports[v].shader->SetMatrix4("projection", _viewports[v].camera->GetProjectionMatrix());
        _viewports[v].shader->SetMatrix4("view", _viewports[v].camera->GetViewMatrix());
        _viewports[v].shader->SetMatrix4("model", _viewports[v].camera->GetModelMatrix());
        glViewport(_viewports[v].x, _viewports[v].y, _viewports[v].width, _viewports[v].height);

        glDrawElements(GL_TRIANGLES, 3 * _viewports[v].object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
    }

    // TODO: updatetest
    if (gui)
    {
        UpdateTest(t);

        if (gui->main_object->option_path)
            DrawPath();
    }

    t++;

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

void ViewportManager::Scroll_Callback(double yoffset)
{
    CurrentViewport();
    _viewports[_activeViewport].camera->ProcessMouseScroll(yoffset);
}

void ViewportManager::Keyboard_Callback(GLFWwindow *window, int key, int action, int mode)
{
    CurrentViewport();

    float camVel = gui->GetIOFramerate() / 20000.0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        _viewports[_activeViewport].camera->Position += camVel * _viewports[_activeViewport].camera->Front;
        _viewports[_activeViewport].camera->Target += camVel * _viewports[_activeViewport].camera->Front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        _viewports[_activeViewport].camera->Position -= camVel * _viewports[_activeViewport].camera->Front;
        _viewports[_activeViewport].camera->Target -= camVel * _viewports[_activeViewport].camera->Front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        _viewports[_activeViewport].camera->Position -= glm::normalize(glm::cross(_viewports[_activeViewport].camera->Front, _viewports[_activeViewport].camera->Up)) * camVel;
        _viewports[_activeViewport].camera->Target -= glm::normalize(glm::cross(_viewports[_activeViewport].camera->Front, _viewports[_activeViewport].camera->Up)) * camVel;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        _viewports[_activeViewport].camera->Position += glm::normalize(glm::cross(_viewports[_activeViewport].camera->Front, _viewports[_activeViewport].camera->Up)) * camVel;
        _viewports[_activeViewport].camera->Target += glm::normalize(glm::cross(_viewports[_activeViewport].camera->Front, _viewports[_activeViewport].camera->Up)) * camVel;
    }
}

void ViewportManager::Mouse_Button_Callback(int button, int action, int mods)
{
    CurrentViewport();
    //Only Update the modifiable viewport (Based on Camera Configuration)
    glm::vec2 mouse_position = gui->GetMousePosition();
    _viewports[_activeViewport].camera->Set_Pos(button, action, mouse_position.x, mouse_position.y);
}
void ViewportManager::Mouse_Position_Callback(double x, double y)
{
    CurrentViewport();
    glm::vec2 mouse_position = gui->GetMousePosition();
    _viewports[_activeViewport].camera->Move_2D(mouse_position.x, mouse_position.y);
}

// TODO: @Haozhe
//============ update test===============

void ViewportManager::UpdateTest(int t)
{
    gui->UpdateTest(t);
}
void ViewportManager::DrawPath()
{
    int vertex_size = gui->geometry_centers.size();
    double vertices[3 * vertex_size];

    for (int i = 0; i < vertex_size; i++)
    {

        for (int j = 0; j < 3; j++)
        {
            vertices[3 * i + j] = gui->geometry_centers[i][j];
        }
    }

    unsigned int indices[2 * (vertex_size - 1)];

    for (int i = 0; i < vertex_size - 1; i++)
    {
        indices[2 * i] = i;
        indices[2 * i + 1] = i + 1;
    }

    glBindVertexArray(gui->path_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, gui->path_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->path_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    glBindVertexArray(gui->path_VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_LINES, 2 * (vertex_size - 1), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time
}