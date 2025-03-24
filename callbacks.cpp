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
	auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	this_inst->width = width;
	this_inst->height = height;

	// set viewport
	glViewport(0, 0, width, height);
	//now your canvas has [0,0] in bottom left corner, and its size is [width x height] 

	this_inst->update_projection_matrix();
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

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto app = static_cast<App*>(glfwGetWindowUserPointer(window));

	app->camera.ProcessMouseMovement(xpos - app->cursorLastX, (ypos - app->cursorLastY) * -1.0);
	app->cursorLastX = xpos;
	app->cursorLastY = ypos;
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// get App instance
	auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	this_inst->fov += 10 * yoffset; // yoffset is mostly +1 or -1; one degree difference in fov is not visible
	this_inst->fov = std::clamp(this_inst->fov, 20.0f, 170.0f); // limit FOV to reasonable values...

	this_inst->update_projection_matrix();
}