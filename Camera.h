
// @yanshi
//#####################################################################
// Class OGL_Camera
//######################################################################
#ifndef OGL_CAMERA_
#define OGL_CAMERA_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

namespace opengl_gui_viewer
{

// class Viewer;

class Camera
{
  public:
    Camera()
        : Position(glm::vec3(0.0f, 5.0f, 0.0f)),
          Target(glm::vec3(0.0f, 0.0f, 0.0f)),
          Front(glm::vec3(0.0f, -1.0f, 0.0f)),
          Up(glm::vec3(0.0f, 0.0f, 1.0f)),
          WorldUp(glm::vec3(0.0f, 0.0f, 1.0f)),
          Position_Delta(glm::vec3(0.0f, 0.0f, 0.0f)),
          MovementSpeed(1.0f), Move_Camera(false),
          Yaw(-90.0f), Pitch(0.0f), Fov(45.0f), Heading(0.0f),
          Screen_Width(0), Screen_Height(0), _config(NOT_CONFIG)
    {
        Right = glm::normalize(glm::cross(Front, Up));
        printf("Right = %f, %f, %f", Right.x, Right.y, Right.z);
    }
    ~Camera()
    {
    }

    enum CameraConfiguration
    {
        LOCAL_CAMERA,
        GLOBAL_CAMERA,
        NOT_CONFIG
    };

    CameraConfiguration _config;
    glm::vec3 Position, Front, Up, Right, Target;
    glm::vec3 Position_Delta, Mouse_Position;
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Theta, Phi;

    GLfloat deltaTime, lastFrame, currentFrame;
    GLfloat MovementSpeed;
    GLfloat Fov;
    GLfloat Heading;

    bool Move_Camera;

    glm::mat4 projection, view, model, MVP;

    int Screen_Width, Screen_Height;

    glm::mat4 GetViewMatrix() { return view; }
    glm::mat4 GetProjectionMatrix() { return projection; }
    glm::mat4 GetModelMatrix() { return model; }
    glm::mat4 GetMatrix() { return MVP; }

    void ConfigureCamera(CameraConfiguration camera_config)
    {
        _config = camera_config;
        switch (camera_config)
        {
        case LOCAL_CAMERA:
            Position = glm::vec3(0.0f, -3.0f, 0.0f);
            Front = glm::vec3(0.0f, 1.0f, 0.0f);
            Up = glm::vec3(0.0f, 0.0f, 1.0f);
            break;

        case GLOBAL_CAMERA:
            Position = glm::vec3(0.0f, -10.0f, 0.0f);
            Front = glm::vec3(0.0f, 1.0f, 0.0f);
            Up = glm::vec3(0.0f, 0.0f, 1.0f);
            break;

        default:
            break;
        }
    }

    void SetSize(const int x, const int y)
    {
        Screen_Width = x;
        Screen_Height = y;
    }

    // TODO
    void test_modification()
    {
        Target = glm::vec3(0.0f, 0.0f, 0.0f);
        // Front = Opposite of Direction ?
        Front = -glm::normalize(Position - Target);
    }

    void Follow(int t, double max)
    {
        Position += glm::vec3(1.0f * 0.00015f * cos(t * 0.00015) / max, 2.0f * 0.00025f * cos(t * 0.00025) / max, -5.0f * 0.0005f * cos(t * 0.0005) / max);
    }

    void Update()
    {
        glm::vec3 offset = Position - Target;
        float R = std::sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);

        if (_config == LOCAL_CAMERA)
        {
            Position.x = Target.x + R * cos(Heading / 180 * M_PI) * sin(Yaw / 180 * M_PI);
            Position.y = Target.y + R * sin(Heading / 180 * M_PI) * sin(Yaw / 180 * M_PI);
            Position.z = Target.z + R * cos(Yaw / 180 * M_PI);
            // printf("Position = %f, %f, %f\n", Position.x, Position.y, Position.z);
        }

        // view = glm::lookAt(Position, Position + Front, Up);
        view = glm::lookAt(Position, Target, Up);
        projection = glm::perspective(glm::radians(Fov), (float)Screen_Width / (float)Screen_Height, 0.1f, 100.0f);
        model = glm::mat4(1.0f);

        MVP = projection * view * model;
    }

    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Fov >= 1.0f && this->Fov <= 45.0f)
            this->Fov -= yoffset;
        if (this->Fov <= 1.0f)
            this->Fov = 1.0f;
        if (this->Fov >= 45.0f)
            this->Fov = 45.0f;
    }

    // TODO: In fact not sure what should Yaw, Pitch used for....
    void Change_Yaw(float degrees)
    {
        // check bounds with the max pitch rate so that we are not moving too fast
        if (degrees < -180)
            degrees = -180;
        else if (degrees > 180)
            degrees = 180;

        this->Yaw += degrees * 0.02f;

        // check bounds for the camera pitch
        if (this->Yaw > 360.0f)
            this->Yaw -= 360.0f;
        else if (this->Yaw < -360.0f)
            this->Yaw += 360.0f;
    }

    //#####################################################################
    // Change_Heading
    //#####################################################################
    void Change_Heading(float degrees)
    {
        // check bounds with the max heading rate so that we are not moving too fast
        if (degrees < -180)
            degrees = -180;
        else if (degrees > 180)
            degrees = 180;

        // this controls how the heading changes if the camera is pointed straight up or down
        // the heading delta direction changes
        if ((this->Pitch > 90 && this->Pitch < 270) || (this->Pitch < -90 && this->Pitch > -270))
            this->Heading -= degrees * 0.02f;
        else
            this->Heading += degrees * 0.02f;

        // check bounds for the camera heading
        if (this->Heading > 360.0f)
            this->Heading -= 360.0f;
        else if (this->Heading < -360.0f)
            this->Heading += 360.0f;
    }

    void Move_2D(int x, int y)
    {
        // compute the mouse delta from the previous mouse position
        glm::vec3 Mouse_Delta = Mouse_Position - glm::vec3(x, y, 0);
        // if the camera is moving, meaning that the mouse was clicked and dragged,
        // change the pitch and heading
        if (Move_Camera)
        {
            // std::cout << Mouse_Delta.x << "\t" << Mouse_Delta.y << std::endl;

            Change_Heading(Mouse_Delta.x * 10);
            // Change_Pitch(Mouse_Delta.y * 10);
            Change_Yaw(Mouse_Delta.y);

            Update();
        }
        Mouse_Position = glm::vec3(x, y, 0);
    }

    void Set_Pos(int button, int state, int x, int y)
    {
        if (button == 3 && state == GLFW_PRESS)
            this->Position_Delta += this->Up * .05f;
        else if (button == 4 && state == GLFW_PRESS)
            this->Position_Delta -= this->Up * .05f;
        else if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
            Move_Camera = true;
        else if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE)
            Move_Camera = false;

        Mouse_Position = glm::vec3(x, y, 0);
    }
};
} // namespace opengl_gui_viewer

#endif