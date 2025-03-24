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
#include "camera.hpp"

class App {
public:
    App();

    bool init(void);
    int run(void);

    bool vsync;

    ~App();
private:
	ShaderProgram globalShader;
    int frames;
    std::chrono::steady_clock::time_point frame_time;
    GLFWwindow* window;

    void getInfo(GLenum, const std::string&, bool);
    void getFPS();
    void initGlew(void);
    void init_assets(void);
    void update_projection_matrix(void);
    GLuint textureInit(const std::filesystem::path&);
    GLuint gen_tex(cv::Mat&);
    uchar getmap(cv::Mat& map, int x, int y);
    void genLabyrinth(cv::Mat& map);

    static void key_callback(GLFWwindow*, int, int, int, int);
    static void scroll_callback(GLFWwindow*, double, double);
	static void fbsize_callback(GLFWwindow*, int, int);
    static void cursor_position_callback(GLFWwindow*, double, double);
    // camera related 
    Camera camera{ glm::vec3(0, 0, 1000) };
    // remember last cursor position, move relative to that in the next frame
    double cursorLastX{ 0 };
    double cursorLastY{ 0 };
    cv::Mat mapa = cv::Mat(10, 25, CV_8U);
protected:
    // projection related variables    
    int width{ 0 }, height{ 0 };
    float fov = 60.0f;
    // store projection matrix here, update only on callbacks
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();



    // all objects of the scene
    std::unordered_map<std::string, Model> scene;

};

