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

class App {
public:
    App();

    bool init(void);
    int run(void);

    ~App();
private:
    void getInfo(GLenum, const std::string&, bool);
    int frames;
    std::chrono::steady_clock::time_point frame_time;
    void getFPS();
	void initGlew(void);

};

