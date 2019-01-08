//!#####################################################################
//! \file ObjectRenderable.h
//!#####################################################################
// Class ObjectRenderable
//######################################################################
#ifndef OBJECTRENDERABLE_H_
#define OBJECTRENDERABLE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "../../RenderableManager.h"
#include "../../ApplicationFactory.h"
#include "../../Shader.h"

#include "Sim_Object.h"

namespace Nova
{
class ObjectRenderable : public Renderable
{
    typedef float T;
    typedef Vector<T, 3> TV;
    typedef Vector<int, 3> T_INDEX;
    using BASE = Renderable;

    std::array<glm::vec3, 8> VoxelVertices;
    std::array<GLuint, 24> VoxelIndices;
    std::vector<glm::vec3> VoxelPositions;

    unsigned int VAO, VBO, EBO;

    opengl_gui_viewer::Sim_Object *main_object;
    GLFWwindow *_window;

  public:
    ObjectRenderable(ApplicationFactory &app)
        : BASE(app)
    {
    }

    virtual ~ObjectRenderable() {}

    virtual void test_GenObject(std::string path)
    {
        main_object = new Sim_Object();
        main_object->cube = new Cuboid(5, 5, 5);
        main_object->holes = new HoleList();
        main_object->trimesh = new TriMesh();
        main_object->holes->AddHole(Hole(2, 1, 0.5));
        main_object->holes->AddHole(Hole(4, 4, 0.3));
        main_object->holes->AddHole(Hole(3, 3, 0.2));
        main_object->holes->AddHole(Hole(1, 1, 0.1));
        main_object->holes->AddHole(Hole(4.5, 4.5, 0.2));

        main_object->trimesh->GenMesh(*(main_object->cube), (main_object->holes->holes));
    }

    virtual void initializeBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // First setup all buffers and attributes for the single voxel prototype
        // Setup buffer for Voxel Vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), &VoxelVertices[0], GL_STATIC_DRAW);
        // Setup buffer for Voxel Index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), &VoxelIndices[0], GL_STATIC_DRAW);
        // Setup Attribute for Voxel Vertex data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    virtual void draw()
    {
        glm::mat4 projection, view, model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto slicePlanes = _app.GetWorld().Get_Slice_Planes();

        auto shader = _app.GetShaderManager().GetShader("InstancedGrid");
        shader->SetMatrix4("projection", projection);
        shader->SetMatrix4("view", view);
        shader->SetMatrix4("model", model);
        shader->SetVector4f("slice_plane0", slicePlanes[0]);
        shader->SetVector4f("slice_plane1", slicePlanes[1]);

        glEnable(GL_PRIMITIVE_RESTART);




        glDisable(GL_PRIMITIVE_RESTART);
    }

};

class ObjectRenderableFactory : public RenderableFactory
{
    using BASE = RenderableFactory;

  public:
    ObjectRenderableFactory()
        : BASE()
    {
    }

    virtual ~ObjectRenderableFactory() {}

    virtual std::unique_ptr<Renderable> Create(ApplicationFactory &app, std::string path)
    {
        ObjectRenderable *renderable = new ObjectRenderable(app);
        renderable->test_GenObject(path);
        renderable->initializeBuffers();
        return std::unique_ptr<Renderable>(renderable);
    }

    virtual bool AcceptExtension(std::string ext) const
    {
        if (ext == "grid")
            return true;
        else
            return false;
    }
};
} // namespace Nova

extern "C" void registerPlugin(Nova::ApplicationFactory &app)
{
    app.GetRenderableManager().AddFactory(std::move(std::unique_ptr<Nova::RenderableFactory>(new Nova::ObjectRenderableFactory())));
}

extern "C" int getEngineVersion()
{
    return Nova::API_VERSION;
}