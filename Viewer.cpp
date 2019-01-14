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
    
    glBindVertexArray(guiWrapper.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time 
    if(guiWrapper.main_object->option_path)    DrawPath();
    UpdateTest(t);
    guiWrapper.ApplyDisplayOption();
    guiWrapper.Render();
    t++;
    //std::cout<<t<<std::endl;
}

void Viewer::UpdateTest(int t)
{
    guiWrapper.UpdateTest(t);
}
void Viewer::DrawPath()
{   
    int vertex_size = guiWrapper.geometry_centers.size();
    double vertices[3*vertex_size];
    
    for(int i = 0; i < vertex_size; i++)
    {
        
        for(int j = 0; j < 3; j++)
        {
            vertices[3*i+j] = guiWrapper.geometry_centers[i][j];   
            //std::cout<<guiWrapper.geometry_centers[i][0]<<","<<guiWrapper.geometry_centers[i][1]<<","<<guiWrapper.geometry_centers[i][2]<<std::endl;
        }
    }

    
    



    unsigned int indices[2*(vertex_size - 1)];
    
    for(int i = 0; i < vertex_size -1; i++)
    {
        indices[2*i] = i;
        indices[2*i+1] = i+1;
    }
    

    glBindVertexArray(guiWrapper.path_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, guiWrapper.path_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiWrapper.path_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    glBindVertexArray(guiWrapper.path_VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_LINES, 2*(vertex_size-1), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time 

}