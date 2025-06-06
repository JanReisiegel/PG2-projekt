﻿// icp.cpp 
// author: JJ

#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"
#include "json.hpp"
#include "imgui_toggle/imgui_toggle.h"
#include "imgui_toggle/imgui_toggle_presets.h"
using json = nlohmann::json;

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
        
		initGLFW();

        

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
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            std::cout << "GL_DEBUG enabled." << std::endl;
        }
        else
            throw std::runtime_error("GL_DEBUG NOT SUPPORTED!");
            //std::cout << "GL_DEBUG NOT SUPPORTED!" << std::endl;

        glfwSwapInterval(vsync ? 1 : 0);
        std::cout << "Vsync " << (!vsync ? "enabled" : "disabled") << std::endl;

		if (!GLEW_ARB_direct_state_access)
			throw std::runtime_error("DSA not supported!");
        
        glEnable(GL_DEPTH_TEST);
        App::init_assets();
		App::init_imgui();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);
		std::string title = std::string("OpenGL context: ").append(std::to_string(width)).append("x").append(std::to_string(height));
        glfwSetWindowTitle(window, title.c_str());
    }
    catch (std::exception const& e) {
        throw std::runtime_error(e.what());
        //std::cerr << "Init failed : " << e.what() << std::endl;
        //throw;
    }
    std::cout << "Initialized...\n";
	glfwShowWindow(window); // show window after all is set up
    return true;
}

void App::initGLFW(void) {
    // init glfw
    // https://www.glfw.org/documentation.html
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // window is not visible until created

	//load attributes from JSON file
    load_json("config.json");

    window = glfwCreateWindow(width, height, "OpenGL context", NULL, NULL);

    if (!window) {
		throw std::runtime_error("Failed to create GLFW window!");
    }

	glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);

    //disable mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mouseCursorIsCatched = true;

	//Callback functions for GLFW
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, fbsize_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void App::load_json(const std::filesystem::path& json_file) {
	// load json file
	std::ifstream file(json_file);
    if (!file.is_open()) {
        std::cerr << "Nepodařilo se otevřít JSON soubor!" << std::endl;
		height = 400;
		width = 800;
		vsync = false;
        glfwWindowHint(GLFW_SAMPLES, 0);
        std::cout << "Antialiasing disabled" << std::endl;
        return;
    }

    json j;
    file >> j;

    width = j["window"]["width"];
    height = j["window"]["height"];

	bool antialiasing = j["antialiasing"]["enabled"];
    int antialaising_level = j["antialiasing"]["samples"];
    
	if (antialiasing) {
        if (antialaising_level <= 1)
        {
			std::cout << "Antialiasing level too low: <1" << std::endl;
        }
		else if (antialaising_level > 8)
		{
			std::cout << "Antialiasing level too high: >8" << std::endl;
		}
		glfwWindowHint(GLFW_SAMPLES, antialaising_level); // 4x antialiasing
		std::cout << "Antialiasing enabled" << std::endl;
	}
	else {
		glfwWindowHint(GLFW_SAMPLES, 0);
		std::cout << "Antialiasing disabled" << std::endl;
	}
	vsync = j["vsync"];
	file.close();

}

void App::render_imgui(void) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    if (App::show_imgui) {
        //ImGui::ShowDemoWindow(); // Enable mouse when using Demo!
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(300, 100));

        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("V-Sync: %s", vsync ? "ON" : "OFF");
        ImGui::Text("FPS: %d", FPS);
		ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
		ImGui::Text("Score: %d", score);
        ImGui::End();
    }
    if (App::pause) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		ImVec2 imguiSize = ImVec2(width/2, height/2);
		ImVec2 imguiPos = ImVec2((width - imguiSize.x) / 2, (height - imguiSize.y) / 2);

        ImGui::SetNextWindowSize(imguiSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(imguiPos, ImGuiCond_Always);

        float fullWidth = (width/2)-15;
        float buttonHeight = 25.0f;

        ImGui::Begin("Pause", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

        ImGui::Text("Game is paused.");
        if (ImGui::Button("Continue", ImVec2(fullWidth, buttonHeight)))
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseCursorIsCatched = true;
            pause = false;
        }

        if (ImGui::Button("End game", ImVec2(fullWidth, buttonHeight)))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        ImGui::NewLine();
        float toggle_width = buttonHeight * 3.0f;

		float centerX = (width/2 - toggle_width) * 0.5f;

        ImGuiToggleConfig toggle_config = ImGuiTogglePresets::MinecraftStyle();
		toggle_config.Size.x = toggle_width;
		toggle_config.Size.y = buttonHeight;
		std::string label = "V-Sync";
        
        ImGui::SetCursorPosX(centerX);
        if (ImGui::Toggle(label.c_str(), &vsync, toggle_config)) {
			glfwSwapInterval(vsync ? 1 : 0);
        }

        ImGui::End();
    }
}

int App::run(void)
{
    try {
        // app code

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &cursorLastX, &cursorLastY);
        //glClearColor(0.529f, 0.808f, 0.922f, 1.0f);


		/*glm::vec4 ourRGBA = { 0.0f, 0.0f, 1.0f, 0.5f };
        glm::vec4 ourRGB = { 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec4 green = { 0.0f, 1.0f, 0.0f, 0.5f };*/

        update_projection_matrix();
        glViewport(0, 0, width, height);

        srand(static_cast<unsigned int>(time(0)));

        //camera.Position = glm::vec3(0, 0, 5);
        double last_frame_time = glfwGetTime();

        s_lights lights = App::initLights();

        //sun lightning
        float angle = 0.0f;

        while (!glfwWindowShouldClose(window))
        {
            render_imgui();

            double current_time = glfwGetTime();
            double delta_t = current_time - last_frame_time;// render time of the last frame 
            last_frame_time = current_time;


            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			

            //########## react to user  ##########
            if (!pause) {
                glm::vec3 move = camera.ProcessInput(window, delta_t);
                if (glm::length(move) > 0)
                {
                    //collision
                    float radius = 0.15f;
                    float newZ = camera.Position.z + move.z;
                    float checkZ = newZ + (move.z > 0 ? radius : -radius);
                    char labyrinth_object = getmap(mapa, camera.Position.x + 0.5, checkZ + 0.5);
                    if (labyrinth_object != '#' && labyrinth_object != '@') {
                        camera.Position.z = newZ;

                    }
                    float newX = camera.Position.x + move.x;
                    float checkX = newX + (move.x > 0 ? radius : -radius);
                    labyrinth_object = getmap(mapa, checkX + 0.5, camera.Position.z + 0.5);
                    if (labyrinth_object != '#' && labyrinth_object != '@') {
                        camera.Position.x = newX;
                    }
                    camera.Position.y += move.y;
                }
            }

            std::vector<Model*> transparent;
            transparent.reserve(scene.size());
            std::string to_erase;
            for (auto& [name, model] : scene) {
                model.shader.setUniform("uV_m", camera.GetViewMatrix());
                model.shader.setUniform("uP_m", projection_matrix);
                if (!model.transparent)
                {
                    model.draw();
                }
                else {
                    transparent.emplace_back(&model);
                }
                if (!pause) {
                    if (name == "sun") {
                        model.origin.z = std::cos(angle) * 22.0f / model.scale.x;
                        model.origin.y = std::sin(angle) * 22.0f / model.scale.x;
                    }
                    lights.position[0].z = std::cos(angle) * 20.0f;
                    lights.position[0].y = std::sin(angle) * 20.0f;
                    lights.position[5] = glm::vec4(camera.Position, 1.0);
                    //lights.spot_direction[5] = camera.Front;
                    model.shader.setUniform("lights.position", lights.position);
                    angle += 0.003f * delta_t;
                    //glm::vec3(0.0f, glm::radians(static_cast<float>(360 * glfwGetTime())), 0.0f));
                    if (name.starts_with("treasure")) {
                        if (intersect(camera.Position, model.meshes[0])) {
                            score++;
                            model.clear();
                            to_erase = name;
                        }
                        if (model.can_jump and model.jump_timer <= 0.0f) {
                            model.can_jump = false;
                            model.jump_velocity = 0.012f;
                            model.jump_timer = 0.6f + static_cast<float>(rand()) / RAND_MAX * 4.0f;
                        }
                        model.jump(delta_t);
                        model.jump_timer -= delta_t;
                    }

                }
			}
            if (!to_erase.empty()) {
                scene.erase(to_erase);
            }

            std::sort(transparent.begin(), transparent.end(), [&](Model const* a, Model const* b) {
                glm::vec3 translation_a = glm::vec3(a->local_model_matrix[3]);  // get 3 values from last column of model matrix = translation
                glm::vec3 translation_b = glm::vec3(b->local_model_matrix[3]);  // dtto for model B
                return glm::distance(camera.Position, translation_a) > glm::distance(camera.Position, translation_b); // sort by distance from camera
                });

            glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);

            // draw sorted transparent
            for (auto p : transparent) {
                //p->shader.setUniform("mycolor", ourRGBA);
                if (p->additional_rotation != glm::vec3(0.0f)) {
                    p->draw(glm::vec3(0.0f), p->additional_rotation);
                }
                else {
                    p->draw();
                }
            }

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);

            if (show_imgui) {
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    //clean up ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // clean-up
    for (auto model : scene) {
        model.second.clear();
    }
    globalShader.clear();
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
    // cv::destroyAllWindows();
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

s_lights App::initLights(void) {
    s_lights lights = s_lights();

    // Positions
    lights.position[0] = glm::vec4(0, 0, 0, 1); //sun
    lights.position[1] = glm::vec4(0.3, 1, 0.3, 0.0); //directional, sky
    lights.position[2] = glm::vec4(2, 2, 5, 1);
    lights.position[3] = glm::vec4(23, 2, 7, 1);
    lights.position[4] = glm::vec4(23, 2, 2, 1);
    lights.position[5] = glm::vec4(0, 0, 0, 1); //camera

    // Colors
    lights.color[0] = glm::vec3(1.0, 1.0, 0.0);
    lights.color[1] = glm::vec3(0.4, 0.4, 0.4);
    lights.color[2] = glm::vec3(0.3, 0.0, 0.8);
    lights.color[3] = glm::vec3(0.8, 0.4, 0.4);
    lights.color[4] = glm::vec3(0.8, 0.4, 0.8);
    lights.color[5] = glm::vec3(0.6, 0.8, 1.0);

    // Ambient
    lights.ambient_intensity[0] = glm::vec3(0.3, 0.3, 0.25);
    lights.ambient_intensity[1] = glm::vec3(0.2, 0.2, 0.2);
    lights.ambient_intensity[2] = glm::vec3(0.15, 0.15, 0.2);
    lights.ambient_intensity[3] = glm::vec3(0.2, 0.18, 0.15);
    lights.ambient_intensity[4] = glm::vec3(0.18, 0.2, 0.25);
    lights.ambient_intensity[5] = glm::vec3(0.1, 0.15, 0.3);

    // Diffuse
    lights.diffuse_intensity[0] = glm::vec3(0.9, 0.9, 0.8);
    lights.diffuse_intensity[1] = glm::vec3(0.7, 0.7, 0.7);
    lights.diffuse_intensity[2] = glm::vec3(0.6, 0.6, 0.7);
    lights.diffuse_intensity[3] = glm::vec3(0.8, 0.7, 0.5);
    lights.diffuse_intensity[4] = glm::vec3(0.5, 0.6, 0.8);
    lights.diffuse_intensity[5] = glm::vec3(0.8, 0.8, 1.0);

    // Specular
    lights.specular_intensity[0] = glm::vec3(1.0, 1.0, 1.0);
    lights.specular_intensity[1] = glm::vec3(0.7, 0.7, 0.7);
    lights.specular_intensity[2] = glm::vec3(0.6, 0.6, 0.7);
    lights.specular_intensity[3] = glm::vec3(0.9, 0.8, 0.6);
    lights.specular_intensity[4] = glm::vec3(0.7, 0.8, 0.9);
    lights.specular_intensity[5] = glm::vec3(1.0, 1.0, 1.0);

    // Ambient Material
    for (int i = 0; i < 6; ++i)
        lights.ambient_material[i] = glm::vec3(0.2, 0.2, 0.2);

    // Diffuse Material
    for (int i = 0; i < 6; ++i)
        lights.diffuse_material[i] = glm::vec3(1.0, 1.0, 1.0);

    // Specular Material
    lights.specular_material[0] = glm::vec3(0.6, 0.6, 0.5);
    lights.specular_material[1] = glm::vec3(0.5, 0.5, 0.5);
    lights.specular_material[2] = glm::vec3(0.4, 0.4, 0.5);
    lights.specular_material[3] = glm::vec3(0.6, 0.5, 0.4);
    lights.specular_material[4] = glm::vec3(0.5, 0.5, 0.6);
    lights.specular_material[5] = glm::vec3(0.6, 0.7, 1.0);

    // Shininess
    lights.specular_shinines[0] = 34.0f;
    lights.specular_shinines[1] = 12.0f;
    lights.specular_shinines[2] = 14.0f;
    lights.specular_shinines[3] = 28.0f;
    lights.specular_shinines[4] = 20.0f;
    lights.specular_shinines[5] = 66.0f;

    // Spotlight (doesn't work)
    lights.cos_cutoff[0] = 180.0f;
    lights.cos_cutoff[1] = 180.0f;
    lights.cos_cutoff[2] = 180.0f;
    lights.cos_cutoff[3] = 180.0f;
    lights.cos_cutoff[4] = 180.0f;
    lights.cos_cutoff[5] = 180.0f; //std::cos(glm::radians(static_cast<float>(30.0f)));

    // Attenuation
    // sun
    lights.constant[0] = 1.0f;
    lights.linear[0] = 0.014f;
    lights.quadratic[0] = 0.0007f;
    // lights
    for (int i = 1; i < 5; ++i) {
        lights.constant[i] = 1.0f;
        lights.linear[i] = 0.12f;
        lights.quadratic[i] = 0.02f;
    }
    // flashlight
    lights.constant[5] = 1.0f;
    lights.linear[5] = 0.24f;
    lights.quadratic[5] = 0.09f;


    for (auto& [name, model] : scene) {
        model.shader.setUniform("lights.position", lights.position);
        model.shader.setUniform("lights.color", lights.color);
        model.shader.setUniform("lights.ambient_intensity", lights.ambient_intensity);
        model.shader.setUniform("lights.diffuse_intensity", lights.diffuse_intensity);
        model.shader.setUniform("lights.specular_intensity", lights.specular_intensity);
        model.shader.setUniform("lights.ambient_material", lights.ambient_material);
        model.shader.setUniform("lights.diffuse_material", lights.diffuse_material);
        model.shader.setUniform("lights.specular_material", lights.specular_material);
        model.shader.setUniform("lights.specular_shinines", lights.specular_shinines);

        model.shader.setUniform("lights.cos_cutoff", lights.cos_cutoff);
        //model.shader.setUniform("lights.spot_direction", lights.spot_direction);
        //model.shader.setUniform("lights.spot_exponent", lights.spot_exponent);

        model.shader.setUniform("lights.constant", lights.constant);
        model.shader.setUniform("lights.linear", lights.linear);
        model.shader.setUniform("lights.quadratic", lights.quadratic);
    }

	return lights;
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
		FPS = frames / (float)elapsed_seconds.count();
        
        frames = 0;
        frame_time = now;
    }
}

GLuint App::textureInit(const std::filesystem::path& filepath, bool& out_transparent)
{
    cv::Mat image = cv::imread(filepath.string(), cv::IMREAD_UNCHANGED);  // Read with (potential) Alpha
    if (image.empty()) {
        throw std::runtime_error("No texture in file: " + filepath.string());
    }

    // or print warning, and generate synthetic image with checkerboard pattern 
    // using OpenCV and use as a texture replacement 

    GLuint texture = gen_tex(image, out_transparent);

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

GLuint App::gen_tex(cv::Mat& image, bool& out_transparent)
{
    GLuint ID = 0;

    if (image.empty()) {
        throw std::runtime_error("Image empty?\n");
    }

    // Generates an OpenGL texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    switch (image.channels()) {
    case 3:
        out_transparent = false;
        // Create and clear space for data - immutable format
        glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
        // Assigns the image to the OpenGL Texture object
        glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case 4:
        out_transparent = true;
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

uchar App::getmap(cv::Mat& map, int x, int y)
{
    x = std::clamp(x, 0, map.cols);
    y = std::clamp(y, 0, map.rows);

    //at(row,col)!!!
    return map.at<uchar>(y, x);
}

// Random  map gen
void App::genLabyrinth(cv::Mat& map) {
    cv::Point2i start_position, end_position;

    // C++ random numbers
    std::random_device r; // Seed with a real random value, if available
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_height(1, map.rows - 2); // uniform distribution between int..int
    std::uniform_int_distribution<int> uniform_width(1, map.cols - 2);
    std::uniform_int_distribution<int> uniform_block(0, 15); // how often are walls generated: 0=wall, anything else=empty

    //inner maze 
    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            switch (uniform_block(e1))
            {
            case 0:
                map.at<uchar>(cv::Point(i, j)) = '#';
                break;
            case 1:
                map.at<uchar>(cv::Point(i, j)) = '#';
                break;
            case 2:
                map.at<uchar>(cv::Point(i, j)) = 'w';
                break;
            case 3:
                map.at<uchar>(cv::Point(i, j)) = 'o';
                break;
            default:
                map.at<uchar>(cv::Point(i, j)) = '.';
                break;
            }
        }
    }

    //walls
    for (int i = 0; i < map.cols; i++) {
        map.at<uchar>(cv::Point(i, 0)) = '@';
        map.at<uchar>(cv::Point(i, map.rows - 1)) = '@';
    }
    for (int j = 0; j < map.rows; j++) {
        map.at<uchar>(cv::Point(0, j)) = '@';
        map.at<uchar>(cv::Point(map.cols - 1, j)) = '@';
    }

    //gen start_position inside maze (excluding walls)
    do {
        start_position.x = uniform_width(e1);
        start_position.y = uniform_height(e1);
    } while (getmap(map, start_position.x, start_position.y) == '#'); //check wall

    //gen end different from start, inside maze (excluding outer walls) 
    do {
        end_position.x = uniform_width(e1);
        end_position.y = uniform_height(e1);
    } while (start_position == end_position); //check overlap
    map.at<uchar>(cv::Point(end_position.x, end_position.y)) = 'e';

    std::cout << "Start: " << start_position << std::endl;
    std::cout << "End: " << end_position << std::endl;

    //print map
    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            if ((i == start_position.x) && (j == start_position.y))
                std::cout << 'X';
            else
                std::cout << getmap(map, i, j);
        }
        std::cout << std::endl;
    }

    //generate boxes
    glm::vec4 green = { 0.0f, 1.0f, 0.0f, 0.5f };
    //ShaderProgram slope_shader = ShaderProgram("resources/basic_core.vert", "resources/basic_uniform.frag");
    //ShaderProgram wall_shader = ShaderProgram("resources/directional.vert", "resources/directional.frag");
    ShaderProgram wall_shader = globalShader;
    bool transparent;
    int box_num = 0;
    GLuint gold = textureInit("resources/gold.png", transparent);
    GLuint grass_tex = textureInit("resources/grass_tall.png", transparent);
    GLuint box_t = textureInit("resources/wall.jpg", transparent);
    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            if (getmap(map, i, j) == 'o') {
                Model treasure = Model("resources/bunny_tri_vnt.obj", wall_shader);
                treasure.meshes[0].texture_id = gold;
                treasure.transparent = false;
                float scale = 0.05;
                treasure.scale *= scale;
                treasure.origin.x = i * (1 / scale);
                treasure.origin.z = j * (1 / scale);
                treasure.origin.y = -0.36 * (1 / scale);
                treasure.meshes[0].AABB_max = glm::vec3(i+0.45, 2, j+0.45);
                treasure.meshes[0].AABB_min = glm::vec3(i-0.45, -0.1, j-0.45);
                scene.emplace("treasure" + std::to_string(box_num), treasure);
            }
            if (getmap(map, i, j) == 'w') {
                Model grass1 = Model("resources/square.obj", wall_shader);
                grass1.meshes[0].texture_id = grass_tex;
                grass1.transparent = true;
                grass1.origin.x = i;
                grass1.origin.z = j;
                grass1.additional_rotation = glm::vec3(0.0, glm::radians(static_cast<float>(45)), 0.0);
                scene.emplace("grass1" + std::to_string(box_num), grass1);
                Model grass2 = Model("resources/square.obj", wall_shader);
                grass2.meshes[0].texture_id = grass_tex;
                grass2.transparent = true;
                grass2.origin.x = i;
                grass2.origin.z = j;
                grass2.additional_rotation = glm::vec3(0.0, glm::radians(static_cast<float>(135)), 0.0);
                scene.emplace("grass2" + std::to_string(box_num), grass2);
            }
            if (getmap(map, i, j) == '#') {
                Model box = Model("resources/cube_triangles_vnt.obj", wall_shader);
                box.meshes[0].texture_id = box_t;
                box.transparent = false;
                box.origin.x = i;
                box.origin.z = j;
                box.scale.y = 1;

                scene.emplace("slope" + std::to_string(box_num), box);
                box_num += 1;
            }
            if (getmap(map, i, j) == '@') {
                Model box = Model("resources/cube_triangles_vnt.obj", wall_shader);
                box.meshes[0].texture_id = box_t;
                box.transparent = false;
                box.origin.x = i;
                box.origin.z = j;
                box.scale.y = 2;
                box.origin.y += 0.25;

                scene.emplace("wall" + std::to_string(box_num), box);
                box_num += 1;
            }
        }
        std::cout << std::endl;
    }


    //set player position in 3D space (transform X-Y in map to XYZ in GL)
    camera.Position.x = (start_position.x); //+ 1.0 / 2.0f;
    camera.Position.z = (start_position.y); //+ 1.0 / 2.0f;
    camera.Position.y = camera.Height;
}

Model App::init_hm(void)
{
    // height map
    {
        std::filesystem::path hm_file("resources/heights.png");
        cv::Mat hmap = cv::imread(hm_file.string(), cv::IMREAD_GRAYSCALE);

        if (hmap.empty())
            throw std::runtime_error("ERR: Height map empty? File: " + hm_file.string());

        Model height_map = GenHeightMap(hmap, 10); //image, step size
        std::cout << "Note: height map vertices: " << height_map.meshes[0].vertices.size() << std::endl;
        return height_map;
    }
}

void App::init_imgui(void) {
    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    std::cout << "ImGUI version: " << ImGui::GetVersion() << "\n";
}

//return bottom left ST coordinate of subtexture
glm::vec2 App::get_subtex_st(const int x, const int y)
{
    return glm::vec2(x * 1.0f / 16, y * 1.0f / 16);
}

// choose subtexture based on height
glm::vec2 App::get_subtex_by_height(float height)
{
    if (height > 0.9)
        return get_subtex_st(2, 11); //snow
    else if (height > 0.8)
        return get_subtex_st(3, 11); //ice
    else if (height > 0.5)
        return get_subtex_st(0, 14); //rock
    else if (height > 0.3)
        return get_subtex_st(2, 15); //soil
    else
        return get_subtex_st(0, 11); //grass
}

Model App::GenHeightMap(const cv::Mat& hmap, const unsigned int mesh_step_size)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    glm::vec3 v;
    glm::vec4 c;

    std::cout << "Note: heightmap size:" << hmap.size << ", channels: " << hmap.channels() << std::endl;

    if (hmap.channels() != 1) {
        std::cerr << "WARN: requested 1 channel, got: " << hmap.channels() << std::endl;
    }

    // Create heightmap mesh from TRIANGLES in XZ plane, Y is UP (right hand rule)
    //
    //   3-----2
    //   |    /|
    //   |  /  |
    //   |/    |
    //   0-----1
    //
    //   012,023
    //

    for (unsigned int x_coord = 0; x_coord < (hmap.cols - mesh_step_size); x_coord += mesh_step_size)
    {
        for (unsigned int z_coord = 0; z_coord < (hmap.rows - mesh_step_size); z_coord += mesh_step_size)
        {
            // Get The (X, Y, Z) Value For The Bottom Left Vertex = 0
            glm::vec3 p0(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Bottom Right Vertex = 1
            glm::vec3 p1(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Top Right Vertex = 2
            glm::vec3 p2(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)), z_coord + mesh_step_size);
            // Get The (X, Y, Z) Value For The Top Left Vertex = 3
            glm::vec3 p3(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)), z_coord + mesh_step_size);

            // Get max normalized height for tile, set texture accordingly
            // Grayscale image returns 0..256, normalize to 0.0f..1.0f by dividing by 256
            float max_h = std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord)) / 256.0f,
                std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)) / 256.0f,
                    std::max(hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)) / 256.0f,
                        hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)) / 256.0f
                    )));

            // Get texture coords in vertices, bottom left of geometry == bottom left of texture
            glm::vec2 tc0 = get_subtex_by_height(max_h);
            glm::vec2 tc1 = tc0 + glm::vec2(1.0f / 16, 0.0f);       //add offset for bottom right corner
            glm::vec2 tc2 = tc0 + glm::vec2(1.0f / 16, 1.0f / 16);  //add offset for top right corner
            glm::vec2 tc3 = tc0 + glm::vec2(0.0f, 1.0f / 16);       //add offset for bottom leftcorner

            // normals for both triangles, CCW
            glm::vec3 n1 = glm::normalize(glm::cross(p1 - p0, p2 - p0)); // for p1
            glm::vec3 n2 = glm::normalize(glm::cross(p2 - p0, p3 - p0)); // for p3
            glm::vec3 navg = glm::normalize(n1 + n2);                 // average for p0, p2 - common

            Vertex v1;
            v1.Position = p0;
            v1.Normal = navg;
            v1.TexCoords = tc0;
            Vertex v2;
            v2.Position = p1;
            v2.Normal = n1;
            v2.TexCoords = tc1;
            Vertex v3;
            v3.Position = p2;
            v3.Normal = navg;
            v3.TexCoords = tc2;
            Vertex v4;
            v4.Position = p3;
            v4.Normal = n2;
            v4.TexCoords = tc3;

            //place vertices and ST to mesh
            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);

            // place indices
            //indices.emplace_back(0, 1, 2, 0, 2, 3);
            indices.emplace_back(0);
            indices.emplace_back(1);
            indices.emplace_back(2);
            indices.emplace_back(0);
            indices.emplace_back(2);
            indices.emplace_back(3);
        }
    }
	bool transparent;
    ShaderProgram tex_shader = ShaderProgram("resources/dir.vert", "resources/tex.frag");
    Model m = Model(GL_TRIANGLES, vertices, indices, tex_shader, textureInit("resources/tex_256.png", transparent));
    m.transparent = false;
    return m;
}

void App::change_screen_mode() {
	App::fullScreen = !App::fullScreen;

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);

    if (App::fullScreen) {
		glfwGetWindowPos(window, &windowPositionX, &windowPositionY);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		/*std::cout << "Windowed mode" << std::endl;
		std::cout << "Window position: " << windowPositionX << ", " << windowPositionY << std::endl;
		std::cout << "Window size: " << windowWidth << ", " << windowHeight << std::endl;*/
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window, nullptr, windowPositionX, windowPositionY, windowWidth, windowHeight, 0);
    }
}

bool App::intersect(glm::vec3 point, Mesh& mesh) {
    //std::cout << point.x << " " << point.y << " " << point.z << " | " << mesh.AABB_max.x << " " << mesh.AABB_max.y << " " << mesh.AABB_max.z << " | " << mesh.AABB_max.x << " " << mesh.AABB_max.y << " " << mesh.AABB_max.z << std::endl;
    return
        point.x >= mesh.AABB_min.x && point.x <= mesh.AABB_max.x &&
        point.y >= mesh.AABB_min.y && point.y <= mesh.AABB_max.y &&
        point.z >= mesh.AABB_min.z && point.z <= mesh.AABB_max.z;
}



void App::init_assets() {
	bool transparent;
    ShaderProgram my_shader_program = ShaderProgram("resources/directional.vert", "resources/directional.frag");

    globalShader = my_shader_program;

    Model my_model = Model("resources/cube_triangles_vnt.obj", my_shader_program);
    GLuint mytex = textureInit("resources/sun2.jpg", transparent);
	my_model.transparent = transparent;
    my_model.meshes[0].texture_id = mytex;
    my_model.scale *= 2.0f;
    //my_model.texture_id = mytex;

    Model floor = Model("resources/square.obj", my_shader_program);
    GLuint floor_tex = textureInit("resources/grass_floor.jpg", transparent);
    floor.transparent = false;
    floor.meshes[0].texture_id = floor_tex;
    floor.orientation.x = - glm::radians(static_cast<float>(90));
    floor.scale.z += 20.0f;
    floor.scale.x += 30.0f;
    floor.origin.z -= 0.5f; //y level

    floor.origin.x += 0.5f;

    scene.emplace("sun", my_model);
    scene.emplace("floor", floor);

    genLabyrinth(mapa);
}