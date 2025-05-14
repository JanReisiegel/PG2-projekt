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
    GLfloat Pitch =  0.0f;;
    GLfloat Roll = 0.0f;
    
    // Camera options
    GLfloat MovementSpeed = 1.2f;
    GLfloat SprintMultiplier = 2.0f;
    GLfloat MouseSensitivity = 0.25f;

    Camera(glm::vec3 position):Position(position)
    {
        this->WorldUp = glm::vec3(0.0f,1.0f,0.0f);
        // initialization of the camera reference system
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime)
    {
        glm::vec3 direction{0};
          
        if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
            direction += Forward;

        if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
            direction += -Forward;

        if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
            direction += -RightSide;       // add unit vector to final direction  

        //... right, up, down, diagonal, ... 
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += RightSide;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            direction.y += 1.0f;

        /*if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

        }*/


        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            direction.y += -1.0f;

        direction = glm::normalize(direction);

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            direction *= SprintMultiplier;

        return direction * MovementSpeed * deltaTime;
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw   += xoffset;
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

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        glm::vec3 forward;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        forward = front;
        forward.y = 0;

        this->Front = glm::normalize(front);
        this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f,1.0f,0.0f)));
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
        this->Forward = glm::normalize(forward);
        this->RightSide = glm::normalize(glm::cross(this->Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
};
