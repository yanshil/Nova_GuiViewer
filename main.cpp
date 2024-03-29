#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <math.h>

#include "TriMesh.h"
#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//#####################################################################
// Global Setting
//#####################################################################

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

bool Check_Out_Of_Boundary(Hole h_, Cuboid c_);
bool Ascend(Hole ha, Hole hb) { return (ha.x < hb.x); }

//#####################################################################
// ImGui Display
//#####################################################################
static void ShowWindowLayout(bool *p_open);
static void ImGui_HoleList(HoleList &holeLists);
static bool show_gui_layout = true;
static bool show_wireframe = true;

//#####################################################################
// The Global Object for the cutted holes mesh
//#####################################################################
TriMesh tri_mesh;
Cuboid cube;
HoleList holeLists;
unsigned int VBO, VAO, EBO;

int vertex_size;
int triangle_size;

//#####################################################################
// main
//#####################################################################
int main()
{
    cube = Cuboid(5, 5, 5, 4);

    // TODO: Seg Fault when Buffer Size is too small..... So I render 5 holes in the first round
    Hole temp = Hole(2, 1, 0.5);
    if(!Check_Out_Of_Boundary(temp,cube))
        holeLists.AddHole(temp);    // will return an ID here

    Hole temp2 = Hole(4, 4, 0.3);
    if(!Check_Out_Of_Boundary(temp2,cube))
        holeLists.AddHole(temp2);    // will return an ID here

    Hole temp3 = Hole(3, 3, 0.2);
    if(!Check_Out_Of_Boundary(temp3,cube))
        holeLists.AddHole(temp3);    // will return an ID here

    Hole temp4 = Hole(1, 1, 0.1);
    if(!Check_Out_Of_Boundary(temp4,cube))
        holeLists.AddHole(temp4);    // will return an ID here

    Hole temp5 = Hole(4.5, 4.5, 0.02);
    if(!Check_Out_Of_Boundary(temp5,cube))
        holeLists.AddHole(temp5);    // will return an ID here

    tri_mesh.GenMesh(cube, holeLists.holes);

    //#####################################################################
    // glfw Windows
    //#####################################################################
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //#####################################################################
    // Setup Dear ImGui
    //#####################################################################

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
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    //---------------------------------------------------------------------

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    glewExperimental = GL_TRUE; // Dude...
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    Shader ourShader("camera.vs", "camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    vertex_size = tri_mesh.vertex_list.size();
    triangle_size = tri_mesh.triangle_list.size();
    double vertices[3*vertex_size];

    // TODO: Here use the cuboid.edge_max
    for (int i = 0; i < vertex_size; i++)
    {   
        for(int j = 0; j < 3; j++)
        {
            vertices[3*i+j] = tri_mesh.vertex_list[i][j] / (double)cube.edge_max - 0.5;
            
        }
        
        std::cout<<i<<": "<<vertices[3*i]<<", "<<vertices[3*i+1]<<", "<<vertices[3*i+2]<<std::endl;
    }    
    unsigned int indices[3*triangle_size];
    for (int i = 0; i < triangle_size; i++)
    {    
        for(int j = 0; j < 3; j++)
        {
            indices[3*i+j] = tri_mesh.triangle_list[i][j];   
        }

        //std::cout<<i<<": "<<tri_mesh.triangle_list[i][0]<<", "<<tri_mesh.triangle_list[i][1]<<", "<<tri_mesh.triangle_list[i][2]<<std::endl;
    }    

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 1.0f)};

    std::cout << vertex_size << "   " << triangle_size << std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //############# Dear  ImGui Frame ######################

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

        if (show_gui_layout)
            ShowWindowLayout(&show_gui_layout);

        //###################################################

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);
        // glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, cubePositions[0]);
        float angle = 20.0f * 0;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        ourShader.setMat4("model", model);
        // draw our first triangle
        //glUseProgram(shaderProgram);
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        
        glDrawElements(GL_TRIANGLES, triangle_size*3, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind iverticest every time

        //###################################################
        // -------------------------------------------------------------------------------
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Display Setting
        // -------------------------------------------------------------------------------
        if (show_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //###################################################

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}

bool Check_Out_Of_Boundary(Hole h_, Cuboid c_)
{

    if ((h_.x - h_.radius) > 0 &&
        (h_.x + h_.radius) < c_.depth &&
        (h_.y - h_.radius) > 0 &&
        (h_.y + h_.radius) < c_.width)
        return false;
    else
    {
        std::cout << "Out of boundary!" << std::endl;
        return true;
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

static void ShowWindowLayout(bool *p_open)
{

    // NULL for no_close
    if (!ImGui::Begin("Generate Object", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    // Cube Configaration
    // ======================================================
    // static float f = 0.0f;
    ImGui::Text("Enter cube size below (depth, width, height)"); // Display some text (you can use a format strings too)

    // // ?What the fourth element for?
    // TODO: Change Input Float!!!!! Because we want double rather than float
    static float vec4a[4] = {5.0f, 5.0f, 5.0f, 0.44f};
    ImGui::InputFloat3("Cube Size", vec4a);

    ImGui::Text("depth = %f, width = %f, height = %f", vec4a[0], vec4a[1], vec4a[2]);

    // Cylinder
    // ======================================================
    ImGui::Separator();
    ImGui_HoleList(holeLists);

    // ======================================================
    ImGui::Separator();
    // ======================================================
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Generate")) // Buttons return true when clicked (most widgets return true when edited/activated)
    {

        // TODO: called destructor for the previous cubes

        cube = Cuboid(vec4a[0], vec4a[1], vec4a[2], 4);
        // Temp Hole for test
        
        // Hole temp3 = Hole(3, 3, 0.5);

        // if(!Check_Out_Of_Boundary(temp3,cube))
        //     holeLists.AddHole(temp3);

        // tri_mesh.GenMesh(cube, holes);
        // TODO: TriMesh cannot set as global because lack of destructor when regenerate mesh
        TriMesh tri_mesh2;
        tri_mesh2.GenMesh(cube, holeLists.holes);

        //================= TODO ===================
        // Unbind and Redraw
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        vertex_size = tri_mesh2.vertex_list.size();
        triangle_size = tri_mesh2.triangle_list.size();
        double vertices[3*vertex_size];

        // TODO: Here use the cuboid.edge_max
        for (int i = 0; i < vertex_size; i++)
        {
            for(int j = 0; j < 3; j++)
            {
            vertices[3*i+j] = tri_mesh2.vertex_list[i][j] / cube.edge_max - 0.5;
            }
        }    
            

        unsigned int indices[3*triangle_size];
        for (int i = 0; i < triangle_size; i++)
        {
            
            for(int j = 0; j < 3; j++)
            {
                indices[3*i+j] = tri_mesh2.triangle_list[i][j];
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

        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, triangle_size, GL_UNSIGNED_INT, 0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        // glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        // glBindVertexArray(0);

    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    ImGui::Checkbox("Wireframe", &show_wireframe);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}

static void ImGui_HoleList(HoleList &holeLists)
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

        for(int i = 0; i < holeLists.size(); i++)
        {
            char holeID[32];
            sprintf(holeID, "%04d", holeLists.holes[i].id);
            
            if (ImGui::Selectable(holeID, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                selected = i;

            ImGui::NextColumn();
            
            ImGui::Text("%04f", holeLists.holes[i].x);
            ImGui::NextColumn();

            ImGui::Text("%04f", holeLists.holes[i].y);
            ImGui::NextColumn();

            ImGui::Text("%04f", holeLists.holes[i].radius);
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
                if(!Check_Out_Of_Boundary(temp,cube))
                {
                    if(holeLists.AddHole(temp)) // != 0: return an ID
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
            holeLists.RemoveAllHole();
        }
        ImGui::Separator();
        ImGui::TreePop();
    }

}
