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
		glUseProgram(shader_prog_ID);

		GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
		if (uniform_color_location == -1)
			throw std::runtime_error("uniform_Color not found!");

        while (!glfwWindowShouldClose(window))
        {
            // ... do_something();
            // 
			
             
            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			glUniform4f(uniform_color_location, App::r, App::g, App::b, App::a);
			glBindVertexArray(vao_ID);
			glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());

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
	glDeleteProgram(shader_prog_ID);
	glDeleteVertexArrays(1, &vao_ID);
	glDeleteBuffers(1, &vbo_ID);
    // clean-up
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
    ShaderProgram my_shader_program = ShaderProgram("resources/basic.vert", "resources/basic.frag");

    Model my_model = Model("resources/plane_tri_vnt.obj", my_shader_program);

    scene.emplace("our_first_object", my_model);

 //   //odsud dolu mažu
 //   const char* vertex_shader =
 //       "#version 460 core"
 //       "in vec3 aPos;"
 //       "in vec3 aColor;"
 //       "out vec3 color;"
 //       "void main(){"
 //       "   gl_Position = vec4(aPos, 1.0);"
 //       "   color = aColor;"
 //       "}";
	//	/*"#version 460 core\n"
	//	"in vec3 attribute_Position;"
	//	"void main() {"
	//	"  gl_Position = vec4(attribute_Position, 1.0);"
	//	"}";*/

 //   const char* fragment_shader =
 //       "#version 460 core"
 //       "in vec3 color;"
 //       "out vec4 FragColor;"
 //       "void main(){"
 //       "   FragColor = vec4(color, 1.0f);"
 //       "}";
	//	/*"#version 460 core\n"
	//	"uniform vec4 uniform_Color;"
	//	"out vec4 FragColor;"
	//	"void main() {"
	//	"  FragColor = uniform_Color;"
	//	"}";*/

	//GLuint vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertex_shader_ID, 1, &vertex_shader, NULL);
	//glCompileShader(vertex_shader_ID);

	//GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragment_shader_ID, 1, &fragment_shader, NULL);
	//glCompileShader(fragment_shader_ID);

	//shader_prog_ID = glCreateProgram();
	//glAttachShader(shader_prog_ID, vertex_shader_ID);
	//glAttachShader(shader_prog_ID, fragment_shader_ID);
	//glLinkProgram(shader_prog_ID);

	//glDetachShader(shader_prog_ID, vertex_shader_ID);
	//glDetachShader(shader_prog_ID, fragment_shader_ID);
	//glDeleteShader(vertex_shader_ID);
	//glDeleteShader(fragment_shader_ID);

	//glCreateVertexArrays(1, &vao_ID);

	//GLint position_attribute_location = glGetAttribLocation(shader_prog_ID, "attribute_Position");
 //   
	//glEnableVertexArrayAttrib(vao_ID, position_attribute_location);
	//glVertexArrayAttribFormat(vao_ID, position_attribute_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	//glVertexArrayAttribBinding(vao_ID, position_attribute_location, 0);

	//glCreateBuffers(1, &vbo_ID);
	//glNamedBufferData(vbo_ID, triangle_vertices.size() * sizeof(vertex), triangle_vertices.data(), GL_STATIC_DRAW);

	//glVertexArrayVertexBuffer(vao_ID, 0, vbo_ID, 0, sizeof(vertex));
}