#include "Viewport.h"

using namespace opengl_gui_viewer;

Viewport::Viewport()
    : ox(0), oy(0), view_width(0), view_height(0),
      object(nullptr)
{
}

Viewport::~Viewport()
{
}

void Viewport::SetCamera(Camera *camera) { this->camera = camera; }
void Viewport::SetShader(Shader *shader) { this->shader = shader; }
void Viewport::SetRenderObject(Sim_Object *object) { this->object = object; }

void Viewport::Initialize()
{
    SetCamera(new Camera());
    // camera->SetSize(view_width, view_height);
    SetShader(new Shader());
    shader->initializeFromFile("camera.vs", "camera.fs");
    shader->use();
}

Shader *Viewport::GetShader()
{
    return shader;
}

Camera *Viewport::GetCamera()
{
    return camera;
}

void Viewport::Resize(const int ox, const int oy, const int w, const int h)
{
    this->ox = ox;
    this->oy = oy;
    this->view_width = w;
    this->view_height = h;
}

void Viewport::Update()
{
    // camera->test_modification();
    camera->Update();
}

void Viewport::DrawFrame()
{
    // set up camera
    shader->setMat4("projection", camera->GetProjectionMatrix());
    shader->setMat4("view", camera->GetViewMatrix());
    shader->setMat4("model", camera->GetModelMatrix());
    glViewport(ox, oy, view_width, view_height);

    glDrawElements(GL_TRIANGLES, 3 * object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
}

void ViewportManager::UpdateTest(int t)
{
    // gui->UpdateTest(t);
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

bool Viewport::InsideCurrView(glm::vec2 mouse_pos)
{
    if (mouse_pos.x < ox)
        return false;
    if (mouse_pos.x > ox + view_width)
        return false;
    if (mouse_pos.y < oy)
        return false;
    if (mouse_pos.y > oy + view_height)
        return false;

    return true;
}

/**
 * TODO:
 * Current for Double Viewport
 */
ViewportManager::ViewportManager(GLFWwindow *window)
    : _currconfig(DUAL_VIEWPORT), window(window), gui(nullptr)
{
}

ViewportManager::~ViewportManager()
{
    viewport_list.clear();
}

void ViewportManager::InitializeGui(GLFWwindow *window, Sim_Object *object)
{
    gui = new ImGui_Wrapper();

    // TODO: If local, initialize guiWrapper
    gui->SetRenderObject(object);
    gui->Initialize(window);
    gui->test_GenObject();
    gui->InitBuffer();
}

void ViewportManager::InitializeViewports(const int global_x, const int global_y)
{

    viewport_list.clear();

    switch (_currconfig)
    {
    case SINGLE_VIEWPORT:
        viewport_list.push_back(Viewport());
        break;
    case DUAL_VIEWPORT:
        viewport_list.push_back(Viewport());
        viewport_list.push_back(Viewport());
        break;

    default:
        break;
    }

    for (int i = 0; i < viewport_list.size(); i++)
    {
        viewport_list[i].Initialize();
    }
    SetWindowGeometry(global_x, global_y);
    switch (_currconfig)
    {
    case SINGLE_VIEWPORT:
        // Set Camera
        viewport_list[0].GetCamera()->ConfigureCamera(Camera::LOCAL_CAMERA);
        break;
    case DUAL_VIEWPORT:
        viewport_list[0].GetCamera()->ConfigureCamera(Camera::LOCAL_CAMERA);
        viewport_list[1].GetCamera()->ConfigureCamera(Camera::GLOBAL_CAMERA);
        break;

    default:
        break;
    }
}

unsigned int ViewportManager::NumViewports() { return viewport_list.size(); }

void ViewportManager::SetWindowGeometry(const int global_x, const int global_y)
{
    switch (_currconfig)
    {
    case SINGLE_VIEWPORT:
        viewport_list[0].Resize(0, 0, global_x, global_y);
        viewport_list[0].GetCamera()->SetSize(global_x, global_y);

        break;

    case DUAL_VIEWPORT:
        viewport_list[0].Resize(0, 0, global_x / 2, global_y);
        viewport_list[1].Resize(global_x / 2, 0, global_x / 2, global_y);

        viewport_list[0].GetCamera()->SetSize(global_x / 2, global_y);
        viewport_list[1].GetCamera()->SetSize(global_x / 2, global_y);

        break;

    default:
        break;
    }
}

Viewport &ViewportManager::GetCurrViewport()
{
    glm::vec2 mouse_pos = gui->GetMousePosition();

    for (int i = 0; i < viewport_list.size(); i++)
    {
        if (viewport_list[i].InsideCurrView(mouse_pos))
            return viewport_list[i];
    }

    return viewport_list[0];
}

void ViewportManager::Scroll_Callback(double yoffset)
{
    GetCurrViewport().GetCamera()->ProcessMouseScroll(yoffset);
}

void ViewportManager::ViewportSetting(Sim_Object *object)
{
    // Set Object

    for (int i = 0; i < viewport_list.size(); i++)
    {
        viewport_list[i].SetRenderObject(object);
    }
}

void ViewportManager::Update()
{
    for (int i = 0; i < viewport_list.size(); i++)
    {
        viewport_list[i].Update();
        // TODO: Should be put in Camera

        // if (viewport_list[i].GetCamera()->_config == Camera::LOCAL_CAMERA)
        //     viewport_list[i].GetCamera()->Follow(t, gui->main_object->cube->edge_max);
    }

    gui->UIFrame();
}

void ViewportManager::DrawFrame()
{
    for (int i = 0; i < viewport_list.size(); i++)
        viewport_list[i].DrawFrame();

    if (gui)
    {
        // if (gui->main_object->animation_test)
        // {
        //     UpdateTest(t);
        //     t++;
        //     if (gui->main_object->option_path)
        //         DrawPath();
        // }

        UpdateTest(t);

        if (gui->main_object->option_path)
            DrawPath();

        gui->ApplyDisplayOption();
        gui->Render();
    }
    t++;
}

Viewport &ViewportManager::GetViewport(int i)
{
    return viewport_list[i];
}

void ViewportManager::Mouse_Button_Callback(int key, int action, int mode)
{
    // Only Update the modifiable viewport
    glm::vec2 mouse_position = gui->GetMousePosition();
    viewport_list[0].GetCamera()->Set_Pos(key, action, mouse_position.x, mouse_position.y);
}

void ViewportManager::Mouse_Position_Callback(double x, double y)
{
    glm::vec2 mouse_position = gui->GetMousePosition();
    // Only Update the modifiable viewport
    viewport_list[0].GetCamera()->Move_2D(mouse_position.x, mouse_position.y);
}

void ViewportManager::Keyboard_Callback(int key, int action, int mode)
{
    float camVel = gui->GetIOFramerate() / 20000.0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        GetViewport(0).GetCamera()->Position += camVel * GetViewport(0).GetCamera()->Front;
        GetViewport(0).GetCamera()->Target += camVel * GetViewport(0).GetCamera()->Front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        GetViewport(0).GetCamera()->Position -= camVel * GetViewport(0).GetCamera()->Front;
        GetViewport(0).GetCamera()->Target -= camVel * GetViewport(0).GetCamera()->Front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        GetViewport(0).GetCamera()->Position -= glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
        GetViewport(0).GetCamera()->Target -= glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        GetViewport(0).GetCamera()->Position += glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
        GetViewport(0).GetCamera()->Target += glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
    }
}
