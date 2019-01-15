#include "World.h"

using namespace opengl_gui_viewer;

World::World(int size_x, int size_y)
    : window(nullptr),
      viewports(nullptr),
      window_width(size_x), window_height(size_y)
{
    object = new Sim_Object();
}

World::~World()
{
    if (window)
    {
        glfwTerminate();
        window = NULL;
    }
    if (viewports)
        delete viewports;
}

void World::Initialize()
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
    window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // disable vsync
    glfwSwapInterval(0);

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    glewExperimental = GL_TRUE; // Magic...Dude...
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        return;
    }

    // SET CALLBACK
    //---------------
    glfwSetWindowCloseCallback(window, Close_Callback);
    glfwSetFramebufferSizeCallback(window, Reshape_Callback);
    glfwSetKeyCallback(window, Keyboard_Callback);
    glfwSetMouseButtonCallback(window, Mouse_Button_Callback);
    glfwSetCursorPosCallback(window, Mouse_Position_Callback);
    glfwSetScrollCallback(window, Scroll_Callback);
    glfwGetFramebufferSize(window, &window_width, &window_height);

    glfwSetWindowUserPointer(window, this); // Wow This is a Magic!

    //---------------
}

void World::Main_Loop()
{
    viewports = new ViewportManager(window);
    viewports->InitializeViewports(window_width, window_height);
    viewports->ViewportSetting(object);

    Initialize_Viewer();


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //############## StateEnable ###########

        glEnable(GL_DEPTH_TEST);

        // Update Viewer & Camera
        viewports->Update();

        // TODO: Multiple Viewport Rendering From Nova

        // // For every viewport, update its camera and draw the scene.
        // for (int v = 0; v < viewport->NumViewports(); v++)
        // {
        //     viewport->SetViewport(v);
        //     viewport->Update();

        //     if (viewport->RenderStyle(ViewportManager::RS_SLICE))
        //     {
        //         GLStateEnable enableClip0(GL_CLIP_DISTANCE0);
        //         GLStateEnable enableClip1(GL_CLIP_DISTANCE1);
        //         _app.GetScene().Draw();
        //     }
        //     else
        //     {
        //         GLStateDisable enableClip0(GL_CLIP_DISTANCE0);
        //         GLStateDisable enableClip1(GL_CLIP_DISTANCE1);
        //         _app.GetScene().Draw();
        //     }
        // }

        //###################################################

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        viewports->DrawFrame();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }
}

void World::Initialize_Viewer()
{
    // Other Initialization has been implemented in constructor
    viewports->GetViewport(0).Initialize_Gui();
}

void World::Close_Callback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void World::Scroll_Callback(GLFWwindow *window, double xoffset, double yoffset)
{
    World *world = static_cast<World *>(glfwGetWindowUserPointer(window));

    world->viewports->GetCurrViewport().GetCamera()->ProcessMouseScroll(yoffset);
}

void World::Reshape_Callback(GLFWwindow *window, int w, int h)
{
    World *world = static_cast<World *>(glfwGetWindowUserPointer(window));
    world->window_width = w;
    world->window_height = h;

    world->viewports->SetWindowGeometry(w, h);
}

void World::Keyboard_Callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    World *world = static_cast<World *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    world->viewports->Mouse_Button_Callback(key, action, mode);
}

// TODO: Some bug with Camera->set pos
void World::Mouse_Button_Callback(GLFWwindow *window, int button, int action, int mods)
{
    World *world = static_cast<World *>(glfwGetWindowUserPointer(window));

    glm::vec2 mouse_position = world->viewports->GetViewport(0).guiWrapper->GetMousePosition();
    world->viewports->GetViewport(0).GetCamera()->Set_Pos(button, action, mouse_position.x, mouse_position.y);
}

void World::Mouse_Position_Callback(GLFWwindow *window, double x, double y)
{
    World *world = static_cast<World *>(glfwGetWindowUserPointer(window));

    glm::vec2 mouse_position = world->viewports->GetViewport(0).guiWrapper->GetMousePosition();
    world->viewports->GetViewport(0).GetCamera()->Move_2D(mouse_position.x, mouse_position.y);

}
