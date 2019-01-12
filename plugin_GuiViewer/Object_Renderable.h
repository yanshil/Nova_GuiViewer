#ifndef OBJECT_RENDERABLE_
#define OBJECT_RENDERABLE_

#include "../ImGui_Wrapper.h"

using namespace opengl_gui_viewer;

#include "plugins/Simulation/Simulation_Renderable.h"

namespace Nova
{
template <class T, int d>
class Object_Renderable : public Simulation_Renderable<T,d>
{
    using Base = Simulation_Renderable<T, d>;
    using Base::directory_name;using Base::active_frame;using Base::_app;
    
    // unsigned int VAO, VBO;
    ImGui_Wrapper guiWrapper;

  public:
    Object_Renderable(ApplicationFactory &app, const std::string &directory_name, int max_frame)
        : Base(app, directory_name, max_frame)
    {
        guiWrapper.Initialize(app.GetWorld().Get_Window());
        guiWrapper.test_GenObject();
        guiWrapper.InitBuffer();
    }
    ~Object_Renderable();

    void Clear_Buffers()
    {
        // if(VAO) glDeleteVertexArrays(1,&VAO);
        // if(VBO) glDeleteBuffers(1,&VBO);
    }

    virtual void Initialize_Buffers()
    {
    }

    virtual void Load_Active_Frame() override
    {
        Clear_Buffers();
        // Reload Buffers
        Initialize_Buffers();
    }

    virtual void draw() override
    {
        Base::draw();
        // draw something else
        guiWrapper.UIFrame();


        glm::mat4 projection, view, model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();

        auto shader = _app.GetShaderManager().GetShader("Camera");
        shader->SetMatrix4("projection", projection);
        shader->SetMatrix4("view", view);
        shader->SetMatrix4("model", model);

        glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind iverticest every time

        guiWrapper.ApplyDisplayOption();
        guiWrapper.Render();
    }
};

} // namespace Nova

#endif