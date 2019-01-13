#include "Viewer.h"

using namespace opengl_gui_viewer;

Viewer::Viewer(GLFWwindow *window, const int size_x, const int size_y)
    : window(window), view_width(size_x), view_height(size_y)
{
    this->camera = new Camera(view_width, view_height);
}

Viewer::~Viewer()
{
}

int Viewer::Initialize()
{
    // Shader
    shader.initializeFromFile("camera.vs", "camera.fs");

    guiWrapper.Initialize(window);
    guiWrapper.test_GenObject();
    guiWrapper.InitBuffer();

    shader.use();
}

Shader &Viewer::GetShader()
{
    return shader;
}

Camera * Viewer::GetCamera()
{
    return camera;
}

void Viewer::Update()
{
    guiWrapper.UIFrame();

    camera->Update();
    camera->test_modification();
}

void Viewer::DrawFrame()
{
    shader.setMat4("projection", camera->GetProjectionMatrix());
    shader.setMat4("view", camera->GetViewMatrix());
    shader.setMat4("model", camera->GetModelMatrix());

    glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);

    guiWrapper.ApplyDisplayOption();
    guiWrapper.Render();
}