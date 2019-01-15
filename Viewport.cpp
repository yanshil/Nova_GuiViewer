#include "Viewport.h"

using namespace opengl_gui_viewer;

Viewport::Viewport(GLFWwindow *window, const int size_x, const int size_y)
    : window(window), ox(0), oy(0), object(nullptr),
      view_width(size_x), view_height(size_y), guiWrapper(nullptr)
{
    this->camera = new Camera(view_width, view_height);
}

Viewport::Viewport(GLFWwindow *window, const int ox, const int oy, const int size_x, const int size_y)
    : window(window), ox(ox), oy(oy), object(nullptr),
      view_width(size_x), view_height(size_y), guiWrapper(nullptr)
{
    this->camera = new Camera(view_width, view_height);
}

Viewport::~Viewport()
{
}

void Viewport::SetRenderObject(Sim_Object *object)
{
    this->object = object;
}

void Viewport::Initialize()
{
    // Shader
    shader.initializeFromFile("camera.vs", "camera.fs");

    shader.use();
}

void Viewport::Initialize_Gui()
{
    // object = new Sim_Object();
    guiWrapper = new ImGui_Wrapper(object);
    // TODO: If local, initialize guiWrapper
    guiWrapper->Initialize(window);
    guiWrapper->test_GenObject();
    guiWrapper->InitBuffer();
}

Shader &Viewport::GetShader()
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
    if (guiWrapper)
        guiWrapper->UIFrame();

    camera->Update();
    //global_camera->test_modification();

    // TODO: If local, follow the object
    // local_camera->Update();
    // local_camera->Follow(t, guiWrapper->main_object->cube->edge_max);
    //local_camera->test_modification();
}

void Viewport::DrawFrame()
{
    // set up camera
    shader.setMat4("projection", camera->GetProjectionMatrix());
    shader.setMat4("view", camera->GetViewMatrix());
    shader.setMat4("model", camera->GetModelMatrix());
    glViewport(ox, oy, view_width, view_height);

    glDrawElements(GL_TRIANGLES, 3 * object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);

    // // set up global camera
    // shader.setMat4("projection", global_camera->GetProjectionMatrix());
    // shader.setMat4("view", global_camera->GetViewMatrix());
    // shader.setMat4("model", global_camera->GetModelMatrix());
    // glViewport(0.5 * view_width, 0, 0.5 * view_width, view_height);

    // if (guiWrapper->main_object->option_path)
    //     DrawPath();
    // UpdateTest(t);
    if (guiWrapper)
    {
        guiWrapper->ApplyDisplayOption();
        guiWrapper->Render();
    }

    t++;
    //std::cout<<t<<std::endl;
}

void Viewport::UpdateTest(int t)
{
    guiWrapper->UpdateTest(t);
}
void Viewport::DrawPath()
{
    int vertex_size = guiWrapper->geometry_centers.size();
    double vertices[3 * vertex_size];

    for (int i = 0; i < vertex_size; i++)
    {

        for (int j = 0; j < 3; j++)
        {
            vertices[3 * i + j] = guiWrapper->geometry_centers[i][j];
            //std::cout<<guiWrapper->geometry_centers[i][0]<<","<<guiWrapper->geometry_centers[i][1]<<","<<guiWrapper->geometry_centers[i][2]<<std::endl;
        }
    }

    unsigned int indices[2 * (vertex_size - 1)];

    for (int i = 0; i < vertex_size - 1; i++)
    {
        indices[2 * i] = i;
        indices[2 * i + 1] = i + 1;
    }

    glBindVertexArray(guiWrapper->path_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, guiWrapper->path_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiWrapper->path_EBO);
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
    glBindVertexArray(guiWrapper->path_VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_LINES, 2 * (vertex_size - 1), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time
}

bool Viewport::InsideCurrView()
{
    glm::vec2 mouse_pos = guiWrapper->GetMousePosition();

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
    : _currconfig(DUAL_VIEWPORT), window(window)
{
}

ViewportManager::~ViewportManager()
{
    viewport_list.clear();
}

void ViewportManager::InitializeViewports(const int global_x, const int global_y)
{

    viewport_list.clear();

    switch (_currconfig)
    {
    case SINGLE_VIEWPORT:
        viewport_list.push_back(Viewport(window, global_x, global_y));
        break;
    case DUAL_VIEWPORT:
        viewport_list.push_back(Viewport(window, 0, 0, global_x / 2, global_y));
        viewport_list.push_back(Viewport(window, global_x / 2, 0, global_x / 2, global_y));
        break;

    default:
        break;
    }

    for (int i = 0; i < viewport_list.size(); i++)
    {
        viewport_list[i].Initialize();
    }
}

unsigned int ViewportManager::NumViewports() { return viewport_list.size(); }

void ViewportManager::SetWindowGeometry(const int global_x, const int global_y)
{
    switch (_currconfig)
    {
    case SINGLE_VIEWPORT:
        viewport_list[0].Resize(0, 0, global_x, global_y);
        break;

    case DUAL_VIEWPORT:
        viewport_list[0].Resize(0, 0, global_x / 2, global_y);
        viewport_list[1].Resize(global_x / 2, 0, global_x / 2, global_y);
        break;

    default:
        break;
    }
}

Viewport &ViewportManager::GetCurrViewport()
{
    for (int i = 0; i < viewport_list.size(); i++)
    {
        if (viewport_list[i].InsideCurrView())
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
    // Local
    viewport_list[0].GetCamera()->SetAsLocal();
    viewport_list[1].GetCamera()->SetAsGlobal();

    viewport_list[0].SetRenderObject(object);
    viewport_list[1].SetRenderObject(object);
}

void ViewportManager::Update()
{
    for (int i = 0; i < viewport_list.size(); i++)
        viewport_list[i].Update();
}

void ViewportManager::DrawFrame()
{
    for (int i = 0; i < viewport_list.size(); i++)
        viewport_list[i].DrawFrame();
}

Viewport &ViewportManager::GetViewport(int i)
{
    return viewport_list[i];
}

void ViewportManager::Mouse_Button_Callback(int key, int action, int mode)
{
    float camVel = GetViewport(0).guiWrapper->GetIOFramerate() / 50000.0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        GetViewport(0).GetCamera()->Position += camVel * GetViewport(0).GetCamera()->Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        GetViewport(0).GetCamera()->Position -= camVel * GetViewport(0).GetCamera()->Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        GetViewport(0).GetCamera()->Position -= glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        GetViewport(0).GetCamera()->Position += glm::normalize(glm::cross(GetViewport(0).GetCamera()->Front, GetViewport(0).GetCamera()->Up)) * camVel;
}
