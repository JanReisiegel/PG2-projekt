#include <iostream>

#include "app.hpp"
#include "gl_err_callback.hpp"

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "Unknown";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "Unknown";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "Unknown";
		}
		}();

	std::cout << "[GL CALLBACK]: " <<
		"source = " << src_str <<
		", type = " << type_str <<
		", severity = " << severity_str <<
		", ID = '" << id << '\'' <<
		", message = '" << message << '\'' << std::endl;
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto app_instance = static_cast<App*>(glfwGetWindowUserPointer(window));
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_V: //GLFW_KEY_F12 - GLFW_KEY_V
			
			if (action == GLFW_PRESS) {
				if (app_instance->vsync) {
					glfwSwapInterval(0);
					app_instance->vsync = false;
				}
				else {
					glfwSwapInterval(1);
					app_instance->vsync = true;
				}
			}
			break;
		default:
			break;
	}
	
}

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void App::fbsize_callback(GLFWwindow* window, int width, int height)
{
	auto app_instance = static_cast<App*>(glfwGetWindowUserPointer(window));
	glfwGetFramebufferSize(window, &width, &height);    // Get GL framebuffer size	

	if (height <= 0) // avoid division by 0
		height = 1;

	float ratio = static_cast<float>(width) / height;

	glm::mat4 projectionMatrix_tmp = glm::perspective(
		glm::radians(120.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
		ratio,			     // Aspect Ratio. Depends on the size of your window.
		0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		20000.0f              // Far clipping plane. Keep as little as possible.
	);
	app_instance->projectionMatrix = projectionMatrix_tmp;

	std::cout << "Projection Matrix in callback:" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "| ";
		for (int j = 0; j < 4; j++) {
			std::cout << projectionMatrix_tmp[i][j] << " ";
		}
		std::cout << "|" << std::endl;
	}

	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//std::cout << "Mouse clicked at: " << xpos << ", " << ypos << std::endl;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "Mouse moved to: " << xpos << ", " << ypos << std::endl;
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto app_instance = static_cast<App*>(glfwGetWindowUserPointer(window));
	//zmìnit GLfloat r na jinou hodnotu podle scrollu
	//GLfloat tem_g = app_instance->g;
	//app_instance->g= std::max(0.0f, std::min(1.0f, tem_g + (float)yoffset / 10.0f));
}