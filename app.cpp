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

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &cursorLastX, &cursorLastY);

		glm::vec4 ourRGBA = { 0.3f, 1.0f, 0.6f, 1.0f };

        update_projection_matrix();
        //glViewport(0, 0, width, height);

        camera.Position = glm::vec3(0, 0, 0.2);
        double last_frame_time = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
             
            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            //########## react to user  ##########
            double delta_t = glfwGetTime() - last_frame_time;// render time of the last frame 
            last_frame_time = glfwGetTime();
            camera.ProcessInput(window, delta_t); // process keys etc.

            //glm::mat4 v_m = glm::lookAt(
            //    glm::vec3(0, 0, 1000), // position of camera
            //    glm::vec3(0, 0, 0),    // where to look
            //    glm::vec3(0, 1, 0)     // up direction
            //);

            for (auto model : scene) {
                model.second.shader.setUniform("uV_m", camera.GetViewMatrix());
                //model.second.shader.setUniform("uP_m", projection_matrix);
                //model.second.shader.setUniform("uV_m", v_m);

                //model.second.shader.setUniform("ucolor", ourRGBA);
                model.second.draw();
                //model.second.draw(glm::vec3(0.0f),
                    //glm::vec3(0.0f, glm::radians(static_cast<float>(360 * glfwGetTime())), 0.0f));
			}

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
            
            //fps count
            App::getFPS();
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
    ShaderProgram my_shader_program = ShaderProgram("resources/tex.vert", "resources/tex.frag");

	globalShader = my_shader_program;
	
    Model my_model = Model("resources/cube_triangles_vnt.obj", my_shader_program);

    GLuint mytex = textureInit("resources/box_rgb888.png");
	my_model.meshes[0].texture_id = mytex;
    //my_model.texture_id = mytex;

    scene.emplace("our_first_object", my_model);
    
    cv::Mat mapa = cv::Mat(10, 25, CV_8U);
}

GLuint App::textureInit(const std::filesystem::path& filepath)
{
    cv::Mat image = cv::imread(filepath.string(), cv::IMREAD_UNCHANGED);  // Read with (potential) Alpha
    if (image.empty()) {
        throw std::runtime_error("No texture in file: " + filepath.string());
    }

    // or print warning, and generate synthetic image with checkerboard pattern 
    // using OpenCV and use as a texture replacement 

    GLuint texture = gen_tex(image);

    return texture;
}

void App::update_projection_matrix(void) {
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

GLuint App::gen_tex(cv::Mat& image)
{
    GLuint ID = 0;

    if (image.empty()) {
        throw std::runtime_error("Image empty?\n");
    }

    // Generates an OpenGL texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    switch (image.channels()) {
    case 3:
        // Create and clear space for data - immutable format
        glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
        // Assigns the image to the OpenGL Texture object
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case 4:
        glTextureStorage2D(ID, 1, GL_RGBA8, image.cols, image.rows);
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
        break;
    default:
        throw std::runtime_error("unsupported channel cnt. in texture:" + std::to_string(image.channels()));
    }

    // Configures the type of algorithm that is used to make the image smaller or bigger
    // nearest neighbor - ugly & fast 
    //glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    //glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // bilinear - nicer & slower
    //glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    //glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear minifying
    glGenerateTextureMipmap(ID);  //Generate mipmaps now.

    // Configures the way the texture repeats
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return ID;
}