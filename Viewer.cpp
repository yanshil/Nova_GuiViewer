#include "Viewer.h"

using namespace opengl_gui_viewer;

Viewer::Viewer(GLFWwindow *window, const int size_x, const int size_y)
    : window(window), view_width(size_x), view_height(size_y)
{
    this->global_camera = new Camera(view_width*0.5, view_height);
    this->global_camera->SetAsGlobal();
    this->local_camera = new Camera(view_width*0.5,view_height);
    this->local_camera->SetAsLocal();
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

Camera* Viewer::GetGlobalCamera()
{
    return global_camera;
}

Camera* Viewer::GetLocalCamera()
{
    return local_camera;
}

void Viewer::Resize(const int w, const int h)
{
    view_width = w;
    view_height = h;
}

void Viewer::Update()
{
    guiWrapper.UIFrame();

    global_camera->Update();
    //global_camera->test_modification();

    local_camera->Update();
    local_camera->Follow(t,guiWrapper.main_object->cube->edge_max);
    //local_camera->test_modification();
}

void Viewer::DrawFrame()
{
    // set up local camera
    shader.setMat4("projection", local_camera->GetProjectionMatrix());
    shader.setMat4("view", local_camera->GetViewMatrix());
    shader.setMat4("model", local_camera->GetModelMatrix());
    glViewport(0, 0, 0.5*view_width, view_height);
    glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
    
    // set up global camera
    shader.setMat4("projection", global_camera->GetProjectionMatrix());
    shader.setMat4("view", global_camera->GetViewMatrix());
    shader.setMat4("model", global_camera->GetModelMatrix());
    glViewport(0.5*view_width, 0, 0.5*view_width, view_height);
    glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
   // if(true)   DrawPath();
    UpdateTest(t);
    guiWrapper.ApplyDisplayOption();
    guiWrapper.Render();
    t++;
}

void Viewer::UpdateTest(int t)
{
    guiWrapper.UpdateTest(t);
}
void Viewer::DrawPath()
{
    float vertices[] = {
        0.0f, 0.0f, 3.0f,
        0.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0,1,
        1,2,
        2,3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time 
 
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------

}