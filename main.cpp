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

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

double Distance(double x1_, double y1_, double x2_, double y2_);
bool Check_Overlap(std::vector<Hole> hs_, Hole h_);
bool Check_Out_Of_Boundary(Hole h_, Cuboid c_);
bool Ascend(Hole ha, Hole hb) { return (ha.x < hb.x); }

//#####################################################################
// ImGui Display
//#####################################################################
static void ShowWindowLayout(bool *p_open);
static bool show_gui_layout = true;

//#####################################################################
// main
//#####################################################################
int main()
{
    Cuboid cuboid;
    std::vector<Hole> holes; // TODO: New class for HoleList (move validation into the list)
    // std::vector<Vertex> boundary_vertices;
    Hole temp;
    bool correct_entry;

    // double tmp_x,tmp_y,tmp_radius;
    // int number_of_holes;hole(1,
    //holes.push_back(temp);

    // std::cout<<"Enter depth, width and height of the cuboid: "<<std::endl;
    // double tmp_depth, tmp_width, tmp_height;
    // std::cin>>tmp_depth>>tmp_width>>tmp_height;
    cuboid = Cuboid(5, 5, 5, 4);

    // std::cout<<"Enter number of holes: "<<std::endl;
    // std::cin>>number_of_holes;

    // for(int i=0;i<number_of_holes;i++)
    // {
    //     do{
    // 	std::cout<<"Enter location  of #"<<i+1<<" hole(p.s.: (x,y)): "<<std::endl;
    // 	std::cin>>tmp_x>>tmp_y;
    // 	std::cout<<"Enter radius of #"<<i+1<<" hole: "<<std::endl;
    // 	std::cin>>tmp_radius;
    // 	temp.Set(tmp_x,tmp_y,tmp_radius);
    // 	correct_entry=!Check_Overlap(holes,temp)&&!Check_Out_Of_Boundary(temp,cuboid);
    // 	if(!correct_entry)
    // 	std::cout<<"Invaid input! Enter again"<<std::endl;
    //      }while(!correct_entry);
    // 	holes.push_back(temp);
    // }

    // TODO: temp 1
    temp = Hole(1, 1, 0.5);
    // Check validation in HoleList class
    holes.push_back(temp);

    // // Cuboid::Cuboid(double depth, double width, double height, int n_segments)
    // Cuboid cube(5, 5, 5, 4);
    // // Hole::Hole(double x, double y, double radius)
    // Hole hole1(1, 1, 0.5);

    // // TODO: check overlap
    // holes.push_back(hole1);

    //#####################################################################
    //
    //#####################################################################
    TriMesh tri_mesh;
    tri_mesh.GenMesh(cuboid, holes);

    // double max = cuboid.depth>(cuboid.width>cuboid.height?cuboid.width:cuboid.height)?cuboid.depth:(cuboid.width>cuboid.height?cuboid.width:cuboid.height);
    // double max = cuboid.edge_max;

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
    // glfwSetCursorPosCallback(window, mouse_callback);
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
    ImGui::StyleColorsDark();
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
    Shader ourShader("7.3.camera.vs", "7.3.camera.fs");
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const int vertex_size = tri_mesh.vertex_list.size();
    const int triangle_size = tri_mesh.triangle_list.size();
    double vertices[vertex_size];

    // TODO: Here use the cuboid.edge_max
    for (int i = 0; i < vertex_size; i++)
        vertices[i] = tri_mesh.vertex_list[i] / cuboid.edge_max - 0.5;

    unsigned int indices[triangle_size];
    for (int i = 0; i < triangle_size; i++)
        indices[i] = tri_mesh.triangle_list[i];
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 1.0f)};

    std::cout << vertex_size << "   " << triangle_size << std::endl;

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

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

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

        // static float f = 0.0f;
        // static int counter = 0;

        // ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        // ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        // if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //     counter++;
        // ImGui::SameLine();
        // ImGui::Text("counter = %d", counter);

        // // ?What the fourth element for?
        // static float vec4a[4] = { 5.0f, 5.0f, 5.0f, 0.44f };
        // ImGui::InputFloat3("input float3", vec4a);

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        // ImGui::End();

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
        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, cubePositions[0]);
        float angle = 20.0f * 0;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        ourShader.setMat4("model", model);
        // draw our first triangle
        //glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, triangle_size, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind iverticest every time

        // -------------------------------------------------------------------------------
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // -------------------------------------------------------------------------------

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

bool Check_Overlap(std::vector<Hole> hs_, Hole h_)
{
    for (int i = 0; i < hs_.size(); i++)
    {
        std::cout << "We are here" << std::endl;
        std::cout << "r1:" << hs_[i].radius << ", r2:" << h_.radius << ", d:" << Distance(hs_[i].x, hs_[i].y, h_.x, h_.y) << std::endl;
        if (Distance(hs_[i].x, hs_[i].y, h_.x, h_.y) <= (hs_[i].radius + h_.radius))
        {
            std::cout << "r1:" << hs_[i].radius << ", r2:" << h_.radius << ", d:" << Distance(hs_[i].x, hs_[i].y, h_.x, h_.y) << std::endl;
            std::cout << "Overlapped with #" << i + 1 << "!" << std::endl;
            return true;
        }
    }
    return false;
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

double Distance(double x1_, double y1_, double x2_, double y2_)
{

    return sqrt((x1_ - x2_) * (x1_ - x2_) + (y1_ - y2_) * (y1_ - y2_));
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
    static float vec4a[4] = {5.0f, 5.0f, 5.0f, 0.44f};
    ImGui::InputFloat3("Cube Size", vec4a);

    ImGui::Text("depth = %f, width = %f, height = %f", vec4a[0], vec4a[1], vec4a[2]);

    // Cylinder
    // ======================================================
    ImGui::Separator();
    // ======================================================
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
        const char *hole_x[3] = {"1.2", "2.2", "1.1"};
        const char *hole_y[3] = {"1.0", "1.33", "2.2"};
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = ImGui::IsItemHovered();
            ImGui::NextColumn();
            ImGui::Text(hole_x[i]);
            ImGui::NextColumn();
            ImGui::Text(hole_y[i]);
            ImGui::NextColumn();
            ImGui::Text("%d", hovered);
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
        if (ImGui::Button("Add"))
        {
        }
        ImGui::SameLine();
        if (ImGui::Button("Modify"))
        {
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
        }
        ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Basic trees"))
    {
        for (int i = 0; i < 5; i++)
            if (ImGui::TreeNode((void *)(intptr_t)i, "Child %d", i))
            {
                ImGui::Text("blah blah");
                ImGui::SameLine();
                if (ImGui::SmallButton("button"))
                {
                };
                ImGui::TreePop();
            }
        ImGui::TreePop();
    }

    // ======================================================
    ImGui::Separator();
    // ======================================================
    ImGui::PushID(1);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Generate")) // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        Cuboid cube = Cuboid(vec4a[0], vec4a[1], vec4a[2], 4);

        // Temp Hole for test
        Hole temp = Hole(2, 2, 0.4);
        // Check validation in HoleList class
        std::vector<Hole> holes;
        holes.push_back(temp);
        TriMesh tri_mesh;
        tri_mesh.GenMesh(cube, holes);

        //================= TODO ===================
        // Unbind and Redraw


    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // static int lines = 10;
    // ImGui::Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
    // ImGui::SliderInt("Number of lines", &lines, 1, 20);
    // for (int i = 0; i < lines; i++)
    //     ImGui::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    ImGui::End();
}