#pragma once
#include <chrono>
#include <stack>
#include <random>
#include <fstream>
#include <string>

// OpenCV (does not depend on GL)
#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW __MUST__ be first.
#include <GLFW/glfw3.h>

// OpenGL math (and other additional GL libraries, at the end)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera
{
public:

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right; 
    glm::vec3 Forward;
    glm::vec3 RightSide;
    glm::vec3 Up; // camera local UP vector
    glm::vec3 WorldUp;

	GLfloat Height = 1.0f;

    GLfloat Yaw = -90.0f;
    GLfloat Pitch =  0.0f;
    GLfloat Roll = 0.0f;
    
    // Camera options
    GLfloat MovementSpeed = 1.2f;
    GLfloat SprintMultiplier = 2.0f;
    GLfloat MouseSensitivity = 0.25f;

    Camera(glm::vec3 position);

    glm::mat4 GetViewMatrix(void);

    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime);

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE);
    
    void updateCameraVectors(void);
};
