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
#include "imgui_internal.h"

namespace opengl_gui_viewer
{

/**
 * Constructor
 */
ImGui_Wrapper::ImGui_Wrapper()
    : _window(NULL), main_object(NULL)
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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

    static double tmp_d = 5.0, tmp_w = 5.0, tmp_h = 5.0;
    ImGui::InputDouble("Depth", &tmp_d, 0.01f, 0.2f, "%.4f");
    ImGui::InputDouble("Width", &tmp_w, 0.01f, 0.2f, "%.4f");
    ImGui::InputDouble("Height", &tmp_h, 0.01f, 0.2f, "%.4f");

    this->tmpParas_cube[0] = tmp_d;
    this->tmpParas_cube[1] = tmp_w;
    this->tmpParas_cube[2] = tmp_h;

    ImGui::Text("depth = %f, width = %f, height = %f", tmp_d, tmp_w, tmp_h);
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

            static double temp_x = 0.0;
            ImGui::InputDouble("Hole X", &temp_x, 0.01f, 0.2f, "%.8f");
            static double temp_y = 0.0;
            ImGui::InputDouble("Hole Y", &temp_y, 0.01f, 0.2f, "%.8f");
            static double temp_raduis = 0.5;
            ImGui::InputDouble("Raduis", &temp_raduis, 0.01f, 0.2f, "%.8f");

            if (ImGui::Button("Add"))
            {
                Hole temp = Hole(temp_x, temp_y, temp_raduis);
                if (!main_object->Check_Out_Of_Boundary(temp, *(main_object->cube)))
                {
                    if (main_object->holes->AddHole(temp)) // != 0: return an ID
                    {
                        ImGui::OpenPopup("Success!");
                    }
                    else
                    {
                        ImGui::OpenPopup("Hole Overlapped with Previous Holes");
                    }
                }
                else
                {
                    ImGui::OpenPopup("Hole Out of Boundary");
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

            if (ImGui::BeginPopupModal("Hole Out of Boundary", &dummy_open))
            {
                ImGui::Text("You are trying to add an out-of-boundary hole.");

                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Hole Overlapped with Previous Holes", &dummy_open))
            {
                ImGui::Text("You are trying to add an overlap hole.");

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

        // // TODO: ImGUI Bug for selected object & popup
        // // https://github.com/ocornut/imgui/issues/2200
        // if (ImGui::Button("Modify"))
        // {
        //     Hole selected_hole;
        //     // Get the Index by ID fist, and then modify it
        //     // https://stackoverflow.com/questions/35787142/how-to-find-and-remove-an-object-from-a-vector
        //     // ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
        //     if (selected != -1)
        //     {
        //         selected_hole = main_object->holes->holes[selected];
        //         int holeID = selected_hole.id;
        //         ImGui::OpenPopup("Modify Hole");
        //         std::cout << "Try tp Modify " << holeID << std::endl;
        //     }
        //     // ImGui::PopItemFlag();

        //     if (ImGui::BeginPopupModal("Modify Hole"))
        //     {
        //         std::cout << "..." << std::endl;

        //         ImGui::Text("Enter Hole Coordinate and Raduis (x, y, raduis)");

        //         static double temp_x = selected_hole.x;
        //         ImGui::InputDouble("Hole X", &temp_x, 0.01f, 0.2f, "%.8f");
        //         static double temp_y = selected_hole.y;
        //         ImGui::InputDouble("Hole Y", &temp_y, 0.01f, 0.2f, "%.8f");
        //         static double temp_raduis = selected_hole.radius;
        //         ImGui::InputDouble("Raduis", &temp_raduis, 0.01f, 0.2f, "%.8f");

        //         if (ImGui::Button("Modify"))
        //         {
        //             Hole temp = Hole(temp_x, temp_y, temp_raduis);
        //             if (!main_object->Check_Out_Of_Boundary(temp, *(main_object->cube)))
        //             {
        //                 selected_hole.x = temp_x;
        //                 selected_hole.y = temp_y;
        //                 selected_hole.radius = temp_raduis;
        //                 selected_hole.initVertex();

        //                 ImGui::OpenPopup("success!");
        //             }
        //         }

        //         // bool dummy_open = true;
        //         // if (ImGui::BeginPopupModal(".!", &dummy_open))
        //         // {
        //         //     ImGui::Text("Update the hole with x = %f, y = %f, raduis = %f", temp_x, temp_y, temp_raduis);

        //         //     if (ImGui::Button("Close"))
        //         //         ImGui::CloseCurrentPopup();

        //         //     ImGui::EndPopup();
        //         // }

        //         // ImGui::SameLine();

        //         // if (ImGui::Button("Close"))
        //         //     ImGui::CloseCurrentPopup();
        //         ImGui::EndPopup();
        //     }
        // }
        // ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            // https://stackoverflow.com/questions/35787142/how-to-find-and-remove-an-object-from-a-vector
            if (selected != -1)
            {
                int holeID = main_object->holes->holes[selected].id;
                for (int i = 0; i < main_object->holes->size(); i++)
                {
                    if (holeID == main_object->holes->holes[i].id)
                    {
                        main_object->holes->holes.erase(main_object->holes->holes.begin() + i);
                        break;
                    }
                }
            }
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

void ImGui_Wrapper::DisplayOptionModule()
{
    ImGui::Checkbox("Wireframe", &main_object->option_wireframe);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void ImGui_Wrapper::ApplyDisplayOption()
{
    // WireFrame
    if (main_object->option_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ImGui_Wrapper::DisplayGenerateModule()
{
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Generate")) // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        delete main_object->cube;
        main_object->cube = new Cuboid(tmpParas_cube[0], tmpParas_cube[1], tmpParas_cube[2]);

        delete main_object->trimesh;
        main_object->trimesh = new TriMesh();

        main_object->trimesh->GenMesh(*(main_object->cube), (main_object->holes->holes));

        NewBuffer();
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
    main_object->cube = new Cuboid(5, 5, 5);
    main_object->holes = new HoleList();
    main_object->trimesh = new TriMesh();
    main_object->holes->AddHole(Hole(2, 1, 0.5));
    main_object->holes->AddHole(Hole(4, 4, 0.3));
    main_object->holes->AddHole(Hole(3, 3, 0.2));
    main_object->holes->AddHole(Hole(1, 1, 0.1));
    main_object->holes->AddHole(Hole(4.5, 4.5, 0.2));

    main_object->trimesh->GenMesh(*(main_object->cube), (main_object->holes->holes));

    //====================================================

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    NewBuffer();
}

void ImGui_Wrapper::NewBuffer()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    int vertex_size = main_object->trimesh->vertex_list.size();
    int triangle_size = main_object->trimesh->triangle_list.size();
    double vertices[3 * vertex_size];

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

    // // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

    ApplyDisplayOption();

    // glNewBufferElements(GL_TRIANGLES, 3 * main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
}

} // namespace opengl_gui_viewer
