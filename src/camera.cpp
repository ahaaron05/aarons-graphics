#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0, 0, -1)), fov(FOV), movement_speed(DESIRED_CAMERA_SPEED), mouse_sensitivity(SENSITIVITY)
{
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;
    update_camera_vectors();
}

Camera::Camera(float x_pos, float y_pos, float z_pos, float up_x, float up_y, float up_z, float yaw, float pitch)
    : front(glm::vec3(0, 0, -1)), fov(FOV), movement_speed(DESIRED_CAMERA_SPEED), mouse_sensitivity(SENSITIVITY)
{
    this->position = glm::vec3(x_pos, y_pos, z_pos);
    this->up = glm::vec3(up_x, up_y, up_z);
    this->yaw = yaw;
    this->pitch = pitch;
    update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::process_keyboard_input(Camera_Movement direction, float delta_time)
{
    float velocity = movement_speed * delta_time;
    if(direction == FORWARD)
    	position += front * velocity;
    if(direction == LEFT)
        position -= right * velocity;
    if(direction == BACKWARD)
        position -= front * velocity;
    if(direction == RIGHT)
        position += right * velocity;
}

void Camera::process_mouse_input(float x_offset, float y_offset)
{
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;
    yaw += x_offset;
    pitch += y_offset;

    if(pitch > 89.0f)
	    pitch = 89.0f;
    if(pitch < -89.0f)
	    pitch = -89.0f;
    
    update_camera_vectors();
}

void Camera::process_mouse_scroll(float y_offset)
{
    fov -= (float)y_offset;
    if(fov < 1.0f)
        fov = 1.0f;
    if(fov > FOV)
        fov = FOV;
}

void Camera::update_camera_vectors()
{
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y =  sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(new_front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}