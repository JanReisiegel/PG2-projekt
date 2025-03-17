// icp.cpp 
// author: JJ

#pragma once

#include <chrono>
#include <stack>
#include <random>

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

#include "gl_err_callback.hpp"
#include "callbacks.hpp"
#include "assets.hpp"
#include "Model.hpp"

class App {
public:
    App();

    bool init(void);
    int run(void);

    bool vsync;

    ~App();
private:
    int frames;
    std::chrono::steady_clock::time_point frame_time;
    GLFWwindow* window;

    void getInfo(GLenum, const std::string&, bool);
    void getFPS();
    void initGlew(void);
    void init_assets(void);
    void update_projection_matrix(void);

    static void key_callback(GLFWwindow*, int, int, int, int);
    static void scroll_callback(GLFWwindow*, double, double);
	static void fbsize_callback(GLFWwindow*, int, int);
protected:
    std::unordered_map<std::string, Model> scene;
    glm::mat4 projectionMatrix = glm::identity<glm::mat4>();
    int width{ 0 }, height{ 0 };
	float fieldOfView{ 60.0f };
	ShaderProgram globalShader;
};

