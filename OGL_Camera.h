
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
          Position_Delta(glm::vec3(0.0f, 0.0f, 0.0f)),
          MovementSpeed(1.0f), Move_Camera(false),
          Theta(0.0f), Phi(0.0f), //Spherical Coordinate
          Yaw(-90.0f), Pitch(0.0f), Fov(45.0f), Heading(0.0f),
          Screen_Width(0), Screen_Height(0), _config(NOT_CONFIG)
    {
        Front = -glm::normalize(Position - Target);
        Right = glm::normalize(glm::cross(Front, Up));
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
    glm::vec3 Position, Target, Up, Front, Right;
    glm::vec3 Position_Delta, Mouse_Position;

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Theta, Phi;

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

    void Follow(int t, double max)
    {
        Position += glm::vec3(1.0f * 0.00015f * cos(t * 0.00015) / max, 2.0f * 0.00025f * cos(t * 0.00025) / max, -5.0f * 0.0005f * cos(t * 0.0005) / max);
    }

    void Update()
    {
        if (_config == LOCAL_CAMERA & Move_Camera)
        // if (Move_Camera)
        {
            glm::vec3 offset = Position - Target;
            float R = std::sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
            Position.x = Target.x + R * cos(Theta) * sin(Phi);
            Position.y = Target.y + R * sin(Theta) * sin(Phi);
            Position.z = Target.z + R * cos(Phi);
        }

        Front = -glm::normalize(Position - Target);
        Right = glm::normalize(glm::cross(Front, Up));

        // view = glm::lookAt(Position, Position + Front, Up);
        view = glm::lookAt(Position, Target, Up);
        projection = glm::perspective(glm::radians(Fov), (float)Screen_Width / (float)Screen_Height, 0.1f, 100.0f);
        model = glm::mat4(1.0f);

        MVP = projection * view * model;
    }

    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Fov >= 1.0f && this->Fov <= 90.0f)
            this->Fov -= yoffset;
        if (this->Fov <= 1.0f)
            this->Fov = 1.0f;
        if (this->Fov >= 90.0f)
            this->Fov = 90.0f;
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

        this->Phi = Yaw / 180 * M_PI;
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

        this->Theta = Heading / 180 * M_PI;
    }

    void Move_2D(int x, int y)
    {
        // compute the mouse delta from the previous mouse position
        glm::vec3 Mouse_Delta = Mouse_Position - glm::vec3(x, y, 0);
        // if the camera is moving, meaning that the mouse was clicked and dragged,
        // change the pitch and heading
        if (Move_Camera)
        {
            Change_Heading(Mouse_Delta.x * 10);
            Change_Yaw(Mouse_Delta.y);

            Update();
        }
        Mouse_Position = glm::vec3(x, y, 0);
    }

    void Set_Pos(int button, int state, int x, int y)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
            Move_Camera = true;
        else if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE)
            Move_Camera = false;

        Mouse_Position = glm::vec3(x, y, 0);
    }
};
} // namespace opengl_gui_viewer

#endif