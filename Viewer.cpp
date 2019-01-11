#include "Viewer.h"

using namespace opengl_gui_viewer;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// void processInput(GLFWwindow *window);

Viewer Viewer::viewer_ = Viewer();

Viewer::Viewer()
    : window(NULL), SCR_WIDTH(800), SCR_HEIGHT(600)
{
    // Orbit Control (Only works for Object view?)
    local_camera_pos = glm::vec3(0.0f, -0.5f, 0.0f);
    local_camera_front = glm::vec3(0.0f, 3.0f, 0.0f);
    local_camera_up = glm::vec3(0.0f, 0.0f, 1.0f);


    global_camera_pos = glm::vec3(0.0f, -3.0f, -0.0f);
    global_camera_front = glm::vec3(0.0f, 1.0f, 0.0f);
    global_camera_up = glm::vec3(0.0f, 0.0f, 1.0f);

    firstMouse = true;
    yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    pitch = 0.0f;
    lastX = 800.0f / 2.0;
    lastY = 600.0 / 2.0;
    fov = 45.0f;

    // timing
    deltaTime = 0.0f; // time between current frame and last frame
    lastFrame = 0.0f;
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

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    glewExperimental = GL_TRUE; // Dude...
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // ====== Shader =======
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

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // float cameraSpeed = 2.5 * deltaTime;
        // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        //     cameraPos += cameraSpeed * cameraFront;
        // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        //     cameraPos -= cameraSpeed * cameraFront;
        // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        //     cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        //     cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //cameraPos += glm::vec3(0.0f,0.001f,0.0f);
        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        //local_camera_pos = glm::vec3(0.0f,-3.0f+0.3f*cos(t_copy/2000.0f),- t * 0.0001f+0.3f*sin(t_copy/2000.0f));

        local_camera_pos = glm::vec3(0.0f + 3.0f * sin(t_copy/5000.0f)  , -3.0f*cos(t_copy/5000.0f),  0.5 * sin(t * 0.0001f)/guiWrapper.main_object->cube->edge_max );
        //std::cout<<"AAAAA:"<<sin(30)<<std::endl;
        glm::vec3 center = glm::vec3(0.0f, 0.0f, -sin(t*0.0001f)/guiWrapper.main_object->cube->edge_max);

        glm::vec3 front = center - local_camera_pos;
        local_camera_front = glm::normalize(front);
        // camera/view transformation
        glm::mat4 local_view = glm::lookAt(local_camera_pos, local_camera_pos + local_camera_front, local_camera_up);
        shader.setMat4("view", local_view);
        // glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

        // TODO Cube Positions Pass
        glm::vec3 cubePositions[] = {
            glm::vec3(0.5f, 0.5f, 0.0f)};

        model = glm::translate(model, cubePositions[0]);
        float angle = 0;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);

        glViewport(0,0,0.5*SCR_WIDTH,SCR_HEIGHT);
        glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);


        glm::mat4 global_view = glm::lookAt(global_camera_pos, global_camera_pos + global_camera_front, global_camera_up);
        shader.setMat4("view", global_view);
        glViewport(0.5*SCR_WIDTH,0,0.5*SCR_WIDTH,SCR_HEIGHT);
        glDrawElements(GL_TRIANGLES, 3 * guiWrapper.main_object->trimesh->triangle_list.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind iverticest every time

        guiWrapper.ApplyDisplayOption();
        guiWrapper.Render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        LinearUpdateTest(t);

        
        if ((t_copy/6280)%2 == 0) {
            t += 1;
        }
        else
            t -= 1;
        
        t_copy += 1;        
        //std::cout<<t<<std::endl;
    }
}

void Viewer::KeyboardCallback(const int key, const int action)
{
}
void Viewer::MouseWheelScrollCallback(const float y_offset)
{
   if (fov >= 1.0f && fov <= 45.0f)
        fov -= y_offset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f; 

}

void Viewer::SizeCallback(const int width, const int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    std::cout<<SCR_WIDTH<<"*"<<SCR_HEIGHT<<std::endl;
}


  void Viewer::LinearUpdateTest(int t)
  {
      guiWrapper.LinearUpdateTest(t);
  }




void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    Viewer::GetViewer().SizeCallback(width,height);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Viewer::GetViewer().MouseWheelScrollCallback(static_cast<float>(yoffset));
}
// void processInput(GLFWwindow *window)
// {
// }