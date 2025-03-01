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
    //budeme mazat
    GLuint shader_prog_ID{ 0 };
	GLuint vao_ID{ 0 };
	GLuint vbo_ID{ 0 };
    GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };
    std::vector<vertex> triangle_vertices =
    {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };
    //konec mazání


    void getInfo(GLenum, const std::string&, bool);
    void getFPS();
    void initGlew(void);
    void init_assets(void);

    static void key_callback(GLFWwindow*, int, int, int, int);
    static void scroll_callback(GLFWwindow*, double, double);
    
};

