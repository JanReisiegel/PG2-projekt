// icp.cpp 
// author: JJ

#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"

GLFWwindow* window = nullptr; // move App class 

App::App()
{
    // default constructor
    // nothing to do here (so far...)
    std::cout << "Constructed...\n";
}

bool App::init()
{
    try {
        // initialization code
        //...

        // init glfw
        // https://www.glfw.org/documentation.html
        glfwSetErrorCallback(error_callback);
        glfwInit();

        // open window (GL canvas) with no special properties
        // https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(800, 600, "OpenGL context", NULL, NULL);
        glfwMakeContextCurrent(window);


        // init glew
        // http://glew.sourceforge.net/basic.html
        App::initGlew(); //glewInit();
        wglewInit();


        // print info:
		App::getInfo(GL_VENDOR, "Vendor", false);
		App::getInfo(GL_RENDERER, "Renderer", false);
		App::getInfo(GL_VERSION, "Version", false);
		App::getInfo(GL_SHADING_LANGUAGE_VERSION, "Shading Language Version", false);
		App::getInfo(GL_MAJOR_VERSION, "Major Version", true);
		App::getInfo(GL_MINOR_VERSION, "Minor Version", true);
		App::getInfo(GL_CONTEXT_PROFILE_MASK, "Context Profile Mask", true);
		App::getInfo(GL_CONTEXT_FLAGS, "Context Flags", true);
		App::getInfo(GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT, "Context Flag Forwards Compatible Bit", true);
		App::getInfo(GL_CONTEXT_FLAG_DEBUG_BIT, "Context Flag Debug Bit", true);
		App::getInfo(GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT, "Context Flag Robust Access Bit", true);
		App::getInfo(GL_CONTEXT_FLAG_NO_ERROR_BIT, "Context Flag No Error Bit", true);


        //fps count
        
        App::frames = 0;
        App::frame_time = std::chrono::steady_clock::now();
        // some init
        // if (not_success)
        //  throw std::runtime_error("something went bad");
		

        if (GLEW_ARB_debug_output) {
            glDebugMessageCallback(MessageCallback, 0);
            glEnable(GL_DEBUG_OUTPUT);
            //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            std::cout << "GL_DEBUG enabled." << std::endl;
        }
        else
            throw std::runtime_error("GL_DEBUG NOT SUPPORTED!");
            //std::cout << "GL_DEBUG NOT SUPPORTED!" << std::endl;

		glfwSwapInterval(vsync ? 1 : 0);
		std::cout << "Vsync " << (!vsync ? "enabled" : "disabled") << std::endl;

		glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, fbsize_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

		if (!GLEW_ARB_direct_state_access)
			throw std::runtime_error("DSA not supported!");
        
        glEnable(GL_DEPTH_TEST);
        App::init_assets();
    }
    catch (std::exception const& e) {
        throw std::runtime_error(e.what());
        //std::cerr << "Init failed : " << e.what() << std::endl;
        //throw;
    }
    std::cout << "Initialized...\n";
    return true;
}

int App::run(void)
{
    try {
        // app code
        //...


        /*GLfloat r, g, b, a;
		r = b = a = 1.0f;
		g = 0.0f;*/
		//glUseProgram(shader_prog_ID);

		//GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
		glm::vec4 ourRGBA = { 0.3f, 1.0f, 0.6f, 1.0f };
		/*if (uniform_color_location == -1)
			throw std::runtime_error("uniform_Color not found!");*/
            //
        
        // Create and set projection matrix
        // You can only set uniforms AFTER shader compile 
        //
        
        //int width, height;
        //glfwGetFramebufferSize(window, &width, &height);    // Get GL framebuffer size	

        //if (height <= 0) // avoid division by 0
        //    height = 1;

        ////float ratio = static_cast<float>(width) / height;

        ////projectionMatrix = glm::perspective(
        ////    glm::radians(60.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
        ////    ratio,			     // Aspect Ratio. Depends on the size of your window.
        ////    0.5f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        ////    200.0f              // Far clipping plane. Keep as little as possible.
        ////);
        ////
        //// set viewport
        ////
        //glViewport(0, 0, width, height);

        /*int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        fbsize_callback(window, width, height);

        std::cout << "Projection Matrix in app:" << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "| ";
            for (int j = 0; j < 4; j++) {
                std::cout << projectionMatrix[i][j] << " ";
            }
            std::cout << "|" << std::endl;
        }*/

        update_projection_matrix();
        //glViewport(0, 0, width, height);

        while (!glfwWindowShouldClose(window))
        {
            // ... do_something();
            // 
			
             
            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 v_m = glm::lookAt(
                glm::vec3(0, 0, 1000), // position of camera
                glm::vec3(0, 0, 0),    // where to look
                glm::vec3(0, 1, 0)     // up direction
            );

            for (auto model : scene) {
                //glm::mat4 m = projectionMatrix;
                //std::stringstream ss;
                //ss << std::fixed << std::setprecision(4);
                //for (int i = 0; i < 4; ++i) {
                //    ss << "| ";
                //    for (int j = 0; j < 4; ++j) {
                //        ss << m[j][i] << " ";  // GLM is column-major
                //    }
                //    ss << "|\n";
                //}
                //std::cout << ss.str();
                //model.second.shader.setUniform("uV_m", v_m);
                //model.second.shader.setUniform("uP_m", projection_matrix);
                model.second.shader.setUniform("ucolor", ourRGBA);
                //model.second.draw();
                model.second.draw(glm::vec3(0.0f),
                    glm::vec3(0, glm::radians(static_cast<float>(360 * glfwGetTime())), 0.0f));
			}

			//glUniform4f(uniform_color_location, App::r, App::g, App::b, App::a);
			//glBindVertexArray(vao_ID);
			//glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
            
            //fps count
            App::getFPS();

            //throw std::runtime_error("Message");
        }
    }
    catch (std::exception const& e) {
        throw std::runtime_error(e.what());
        //std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

App::~App()
{
	//glDeleteProgram(shader_prog_ID);
	//glDeleteVertexArrays(1, &vao_ID);
	//glDeleteBuffers(1, &vbo_ID);
    // clean-up
    for (auto model : scene) {
        model.second.shader.clear();
    }
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
    //cv::destroyAllWindows();
    std::cout << "Bye...\n";
}

void App::initGlew(void) {
    // Initialize all valid generic GL extensions with GLEW.
    // Usable AFTER creating GL context! (create with glfwInit(), glfwCreateWindow(), glfwMakeContextCurrent())
    GLenum glew_ret = glewInit();
    if (glew_ret != GLEW_OK) {
        throw std::runtime_error(std::string("GLEW failed with error: ")
            + reinterpret_cast<const char*>(glewGetErrorString(glew_ret)));
    }
    else {
        std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION) << std::endl;
    }

    // Platform specific part.
    glew_ret = wglewInit();
    if (glew_ret != GLEW_OK) {
        throw std::runtime_error(std::string("WGLEW failed with error: ")
            + reinterpret_cast<const char*>(glewGetErrorString(glew_ret)));
    }
    else {
        std::cout << "WGLEW successfully initialized platform specific functions." << std::endl;
    }

    // If you want, you can get extensions and list...
    /*GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    std::cout << "We have " << n << " extensions.\n";
    for (int i = 0; i < n; i++) {
        const char* extension_name = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        std::cout << extension_name << '\n';
    }*/
}

void App::getInfo(GLenum type, const std::string& display_name, bool numeric)
{
    if (numeric)
    {
        GLint my_int;
		glGetIntegerv(type, &my_int);
        if (type == GL_CONTEXT_PROFILE_MASK) {
            if (my_int & GL_CONTEXT_CORE_PROFILE_BIT) {
                std::cout << "We are using CORE profile\n";
            }
            else {
                if (my_int & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
                    std::cout << "We are using COMPATIBILITY profile\n";
                }
            }
        }
		std::cout << display_name + ": " << my_int << std::endl;
        return;
    }
	const char* my_string = (const char*)glGetString(type);

	if (my_string)
		std::cout << display_name + ": " << my_string << std::endl;
	else
		std::cerr << display_name + " is not available" << std::endl;
}

void App::getFPS() {
    frames++;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - frame_time;
    if (elapsed_seconds.count() >= 1)
    {
        std::string vsync_mode = App::vsync ? "enabled" : "disabled";
		std::string title = std::string("FPS: ").append(std::to_string(frames)).append(std::string(" VSync ")).append(vsync_mode);
        glfwSetWindowTitle(window, title.c_str());
        frames = 0;
        frame_time = now;
    }
}

void App::init_assets() {
    ShaderProgram my_shader_program = ShaderProgram("resources/basic_core.vert", "resources/basic_uniform.frag");
    my_shader_program.activate();
	
    Model my_model = Model("resources/triangle.obj", my_shader_program);

    scene.emplace("our_first_object", my_model);
    
}

void App::update_projection_matrix(void)
{
    if (height < 1)
        height = 1;   // avoid division by 0

    float ratio = static_cast<float>(width) / height;

    projection_matrix = glm::perspective(
        glm::radians(fov),   // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}