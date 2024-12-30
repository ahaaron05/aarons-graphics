#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement    // Used as abstraction to stay away from glfw-specific input methods.
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Defualt camera values
const float FOV = 80.0f;
const float YAW     = -90.0f;
const float PITCH   =  0.0f;
const glm::vec3 DEFAULT_POS = glm::vec3(0);
const float DESIRED_CAMERA_SPEED = 6.0f;
const float SENSITIVITY = 0.08f;

class Camera 
{
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float fov;
    float movement_speed;
    float mouse_sensitivity;

    // constructor with vectors
    Camera(glm::vec3 position = DEFAULT_POS, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw=YAW, float pitch=PITCH);
    // constructor with scalars
    Camera(float x_pos, float y_pos, float z_pos, float up_x, float up_y, float up_z, float yaw=YAW, float pitch=PITCH);

    // returns the view matrix calculated using Euler angles and the lookAt matrix
    glm::mat4 get_view_matrix() const;

    // processes input recieved from keyboard-like input system. Abstracted thru ENUM
    void process_keyboard_input(Camera_Movement direction, float delta_time);

    // processes input recieved from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_input(float x_offset, float y_offset);

    // changes the FOV based on mouse scroll wheel input.
    void process_mouse_scroll(float y_offset);

private:
    // Calculates the front vector from the camera's (updated) Euler Angles
    void update_camera_vectors();
};