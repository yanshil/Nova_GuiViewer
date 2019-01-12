#include "Viewer.h"

using namespace opengl_gui_viewer;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, int button, int action,
                    int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action,
  int mods);

Viewer Viewer::viewer_ = Viewer();

Viewer::Viewer()
    : window(NULL), SCR_WIDTH(800), SCR_HEIGHT(600)
{
    // // timing
    // deltaTime = 0.0f; // time between current frame and last frame
    // lastFrame = 0.0f;
}

Viewer::~Viewer()
{
}

Viewer &Viewer::GetViewer()
{
    return viewer_;
}

void Viewer::Terminate()
{
    if (window)
    {
        glfwTerminate();
        window = NULL;
    }
}

int Viewer::Initialize()
{
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    glewExperimental = GL_TRUE; // Dude...
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Shader
    shader.initializeFromFile("camera.vs", "camera.fs");

    guiWrapper.Initialize(window);
    guiWrapper.test_GenObject();
    guiWrapper.InitBuffer();
}

void Viewer::Main_Loop()
{
    shader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //############# Dear  ImGui Frame ######################
        guiWrapper.UIFrame();

        //###################################################

        float camVel = cam.GetCameraSpeed();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam.Position += camVel * cam.Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.Position -= camVel * cam.Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam.Position -= glm::normalize(glm::cross(cam.Front, cam.Up)) * camVel;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam.Position += glm::normalize(glm::cross(cam.Front, cam.Up)) * camVel;

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        cam.Update();

        shader.setMat4("projection", cam.GetProjectionMatrix());
        shader.setMat4("view", cam.GetViewMatrix());
        cam.test_modification();
        shader.setMat4("model", cam.GetModelMatrix());

        glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind iverticest every time

        guiWrapper.ApplyDisplayOption();
        guiWrapper.Render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }
}

void Viewer::KeyboardCallback(const int key, const int action)
{
    guiWrapper.KeyboardCallback(key, action);
}
void Viewer::MouseWheelScrollCallback(const float y_offset)
{
    guiWrapper.MouseWheelScrollCallback(y_offset);
}
void Viewer::MouseButtonCallback(const int button, const int action)
{
    guiWrapper.MouseButtonCallback(button, action);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, int button, int action,
                    int mods)
{
    Viewer::GetViewer().MouseButtonCallback(button, action);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Viewer::GetViewer().MouseWheelScrollCallback(static_cast<float>(yoffset));
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action,
  int mods)
{
    Viewer::GetViewer().KeyboardCallback(key, action);
}
// void processInput(GLFWwindow *window)
// {
// }