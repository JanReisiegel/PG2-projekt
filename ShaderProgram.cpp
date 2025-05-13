#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderProgram.hpp"

// set uniform according to name 
// https://docs.gl/gl4/glUniform

ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file) {
	std::vector<GLuint> shader_ids;

    // compile shaders and store IDs for linker
	shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
	shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));

	// link all compiled shaders into shader_program 
    ID = link_shader(shader_ids);
}

void ShaderProgram::setUniform(const std::string& name, const float val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1f(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const int val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1i(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val) {
	//statefull
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform3fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4 in_vec4) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform4fv(loc, 1, glm::value_ptr(in_vec4));
}

void ShaderProgram::setUniform(const std::string& name, const std::array<glm::vec4, MAX_LIGHTS> & val) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform4fv(loc, MAX_LIGHTS, glm::value_ptr(val[0]));

}

void ShaderProgram::setUniform(const std::string& name, const std::array<glm::vec3, MAX_LIGHTS>& val) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform3fv(loc, MAX_LIGHTS, glm::value_ptr(val[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::array<float, MAX_LIGHTS>& val) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1fv(loc, MAX_LIGHTS, val.data());
}


void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj) {
	int infologLength = 0;
	std::string infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> infoVector(infologLength);
		glGetShaderInfoLog(obj, infologLength, NULL, infoVector.data());
		infoLog.assign(infoVector.begin(), infoVector.end());
	}
	return infoLog;
}

std::string ShaderProgram::getProgramInfoLog(const GLuint obj) {
    // implement, print info
	int infologLength = 0;
	std::string infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> infoVector(infologLength);
		glGetProgramInfoLog(obj, infologLength, NULL, infoVector.data());
		infoLog.assign(infoVector.begin(), infoVector.end());
	}
	return infoLog;
}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type) {
	GLuint shader_h;

    // implement, try to compile, check for error; if any, print compiler result (or print allways, if you want to see warnings as well)
    // if err, throw error
	shader_h = glCreateShader(type);
	std::string shader_source = textFileRead(source_file);
	const char* shader_source_cstr = shader_source.c_str();
	glShaderSource(shader_h, 1, &shader_source_cstr, NULL);
	glCompileShader(shader_h);
	{
		GLint status;
		glGetShaderiv(shader_h, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			std::cerr << "Compiler error: " << getShaderInfoLog(shader_h) << '\n';
			throw std::runtime_error("Compiler error");
		}
	}
	return shader_h;
}

GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids) {
	GLuint prog_h = glCreateProgram();

	for (const GLuint id : shader_ids)
	{
		glAttachShader(prog_h, id);
	}

	glLinkProgram(prog_h);
	{ // TODO: implement: check link result, print info & throw error (if any)
        
		GLint status;
		glGetProgramiv(prog_h, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			std::cerr << "Linker error: " << getProgramInfoLog(prog_h) << '\n';
			throw std::runtime_error("Linker error");
		}

	}
	return prog_h;
}

std::string ShaderProgram::textFileRead(const std::filesystem::path& filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error(std::string("Error opening file: ") + filename.string());
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}
