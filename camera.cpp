#pragma once

#include "camera.hpp"

Camera::Camera(glm::vec3 position) : Position(position)
{
    this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // initialization of the camera reference system
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix(void)
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::vec3 Camera::ProcessInput(GLFWwindow* window, GLfloat deltaTime) 
{
    glm::vec3 direction{ 0 };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += Forward;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -Forward;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -RightSide;       // add unit vector to final direction  

    //... right, up, down, diagonal, ... 
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += RightSide;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        direction.y += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (can_jump) {
            can_jump = false;
            jump_velocity = 1.5f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        direction.y += -1.0f;

    if (glm::length(direction) > 0.0f)
        direction = glm::normalize(direction) * MovementSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction *= SprintMultiplier;

    if (!can_jump) {
        direction.y += jump_velocity;
        jump_velocity -= gravity;
        if (Position.y <= 0.5f) {
            Position.y = 0.5f;
            can_jump = true;
        }
    }

    return direction * deltaTime;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch += yoffset;

    if (constraintPitch)
    {
        if (this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if (this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }

    this->updateCameraVectors();
}

void Camera::updateCameraVectors(void)
{
    glm::vec3 front;
    glm::vec3 forward;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    forward = front;
    forward.y = 0;

    this->Front = glm::normalize(front);
    this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    this->Forward = glm::normalize(forward);
    this->RightSide = glm::normalize(glm::cross(this->Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
}