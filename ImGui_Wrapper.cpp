// @yanshi
//#####################################################################
// Class ImGui_Wrapper
//######################################################################
#include "ImGui_Wrapper.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace opengl_gui_viewer
{

/**
 * Constructor
 */
ImGui_Wrapper::ImGui_Wrapper()
    :_window(NULL), main_object(NULL)
{
}

void ImGui_Wrapper::Initialize(GLFWwindow *window)
{
    //#####################################################################
    // Setup Dear ImGui
    //#####################################################################
    _window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(_window, true);

    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

}

/**
 * Destructor
 */
ImGui_Wrapper::~ImGui_Wrapper()
{
}

void ImGui_Wrapper::Render()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Wrapper::InitUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // NULL for no_close
    ImGui::Begin("Generate Object", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    DisplayUI();
    ImGui::End();
}

void ImGui_Wrapper::DisplayUI()
{
    DisplayCubeModule();
    ImGui::Separator();

    DisplayHoleModule();
    ImGui::Separator();

    DisplayGenerateModule();

    DisplayAnimation();
}

void ImGui_Wrapper::DisplayCubeModule()
{
    // Cube Configaration
    // ======================================================
    ImGui::Text("Enter cube size below (depth, width, height)"); // Display some text (you can use a format strings too)

    // // ?What the fourth element for?
    // TODO: Change Input Float!!!!! Because we want double rather than float
    static float vec4a[4] = {5.0f, 5.0f, 5.0f, 0.44f};
    ImGui::InputFloat3("Cube Size", vec4a);

    ImGui::Text("depth = %f, width = %f, height = %f", vec4a[0], vec4a[1], vec4a[2]);
}
void ImGui_Wrapper::DisplayHoleModule()
{
    // Basic columns
    if (ImGui::TreeNode("Hole List"))
    {
        ImGui::Columns(4, "mycolumns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("ID");
        ImGui::NextColumn();
        ImGui::Text("x");
        ImGui::NextColumn();
        ImGui::Text("y");
        ImGui::NextColumn();
        ImGui::Text("raduis");
        ImGui::NextColumn();
        ImGui::Separator();

        static int selected = -1;

        for (int i = 0; i < main_object->holes->size(); i++)
        {
            char holeID[32];
            sprintf(holeID, "%04d", main_object->holes->holes[i].id);

            if (ImGui::Selectable(holeID, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                selected = i;

            ImGui::NextColumn();

            ImGui::Text("%04f", main_object->holes->holes[i].x);
            ImGui::NextColumn();

            ImGui::Text("%04f", main_object->holes->holes[i].y);
            ImGui::NextColumn();

            ImGui::Text("%04f", main_object->holes->holes[i].radius);
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();

        if (ImGui::Button("Add"))
        {
            ImGui::OpenPopup("Add Hole");
        }

        if (ImGui::BeginPopupModal("Add Hole"))
        {
            ImGui::Text("Enter Hole Coordinate and Raduis (x, y, raduis)"); // Display some text (you can use a format strings too)

            // // ?What the fourth element for?
            static double temp_x = 0.0;
            ImGui::InputDouble("Hole X", &temp_x, 0.01f, 0.2f, "%.8f");
            static double temp_y = 0.0;
            ImGui::InputDouble("Hole Y", &temp_y, 0.01f, 0.2f, "%.8f");
            static double temp_raduis = 0.5;
            ImGui::InputDouble("Raduis", &temp_raduis, 0.01f, 0.2f, "%.8f");

            if (ImGui::Button("Add"))
            {
                // TODO : Change the validation std::out output to ImGui::Text
                Hole temp = Hole(temp_x, temp_y, temp_raduis);
                if (!main_object->Check_Out_Of_Boundary(temp, *(main_object->cube)))
                {
                    if (main_object->holes->AddHole(temp)) // != 0: return an ID
                    {
                        ImGui::OpenPopup("Success!");
                    }
                }
            }

            bool dummy_open = true;
            if (ImGui::BeginPopupModal("Success!", &dummy_open))
            {
                ImGui::Text("Insert a hole with x = %f, y = %f, raduis = %f", temp_x, temp_y, temp_raduis);

                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Modify"))
        {
            // Get the Index by ID fist, and then modify it
            // https://stackoverflow.com/questions/35787142/how-to-find-and-remove-an-object-from-a-vector
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            // https://stackoverflow.com/questions/35787142/how-to-find-and-remove-an-object-from-a-vector
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove All"))
        {
            main_object->holes->RemoveAllHole();
        }
        ImGui::Separator();
        ImGui::TreePop();
    }
}
void ImGui_Wrapper::DisplayGenerateModule()
{
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Generate")) // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        // TODO: called destructor for the previous cubes

        delete main_object->cube;

        static float vec4a[4] = {5.0f, 5.0f, 5.0f, 0.44f};
        ImGui::InputFloat3("Cube Size", vec4a);
        main_object->cube = new Cuboid(vec4a[0], vec4a[1], vec4a[2], 4);

        delete main_object->trimesh;
        main_object->trimesh = new TriMesh();
        
        main_object->trimesh->GenMesh(*(main_object->cube), (main_object->holes->holes));

        Draw();
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}
void ImGui_Wrapper::DisplayAnimation()
{
    ImGui::Checkbox("Wireframe", &(main_object->option_wireframe));

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void ImGui_Wrapper::Setup_Mesh()
{
    main_object = new Sim_Object();
    main_object->cube = new Cuboid(5, 5, 5, 4);
    main_object->holes = new HoleList();
    main_object->trimesh = new TriMesh();
    main_object->holes->AddHole(Hole(2, 1, 0.5));
    main_object->holes->AddHole(Hole(4, 4, 0.3));
    main_object->holes->AddHole(Hole(3, 3, 0.2));
    main_object->holes->AddHole(Hole(1, 1, 0.1));
    main_object->holes->AddHole(Hole(4.5, 4.5, 0.2));

    std::cout << "Any0?" << std::endl;
    std::cout <<main_object->holes->holes.size() <<std::endl;
    std::cout <<main_object->cube->edge_max <<std::endl;

    main_object->trimesh->GenMesh(*(main_object->cube), (main_object->holes->holes));

    std::cout << "Any?" << std::endl;

    //====================================================

    glGenVertexArrays(1, &VAO);
    std::cout << "Gen Buffer" << std::endl;
    glGenBuffers(1, &VBO);
    std::cout << "Gen Buffer" << std::endl;
    glGenBuffers(1, &EBO);
    std::cout << "Gen Buffer" << std::endl;
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    std::cout << "Bind VAO" << std::endl;

    // Draw();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    int vertex_size = main_object->trimesh->vertex_list.size();
    int triangle_size = main_object->trimesh->triangle_list.size();
    double vertices[3 * vertex_size];

    // TODO: Here use the cuboid.edge_max
    for (int i = 0; i < vertex_size; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[3 * i + j] = main_object->trimesh->vertex_list[i][j] / (double)main_object->cube->edge_max - 0.5;
        }

        std::cout << i << ": " << vertices[3 * i] << ", " << vertices[3 * i + 1] << ", " << vertices[3 * i + 2] << std::endl;
    }
    unsigned int indices[3 * triangle_size];
    for (int i = 0; i < triangle_size; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            indices[3 * i + j] = main_object->trimesh->triangle_list[i][j];
        }

        //std::cout<<i<<": "<<main_object->trimesh->triangle_list[i][0]<<", "<<main_object->trimesh->triangle_list[i][1]<<", "<<main_object->trimesh->triangle_list[i][2]<<std::endl;
    }

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 1.0f)};

    std::cout << vertex_size << "   " << triangle_size << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

}
void ImGui_Wrapper::Draw()
{
    //================= TODO ===================
    // Unbind and Redraw
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    int vertex_size = main_object->trimesh->vertex_list.size();
    int triangle_size = main_object->trimesh->triangle_list.size();
    double vertices[3 * vertex_size];

    // TODO: Here use the cuboDrawid.edge_max
    for (int i = 0; i < vertex_size; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[3 * i + j] = main_object->trimesh->vertex_list[i][j] / main_object->cube->edge_max - 0.5;
        }
    }

    unsigned int indices[3 * triangle_size];
    for (int i = 0; i < triangle_size; i++)
    {

        for (int j = 0; j < 3; j++)
        {
            indices[3 * i + j] = main_object->trimesh->triangle_list[i][j];
        }
    }

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 1.0f)};

    std::cout << vertex_size << "   " << triangle_size << std::endl;

    // unsigned int VBO, VAO, EBO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);
    // // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 3 * main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
}

} // namespace opengl_gui_viewer
