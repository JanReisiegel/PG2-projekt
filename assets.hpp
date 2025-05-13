#pragma once

#include <glm/glm.hpp> 
#include <array>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

constexpr int MAX_LIGHTS = 16;

struct s_lights {
	std::array<glm::vec4, MAX_LIGHTS> position;
	std::array<glm::vec3, MAX_LIGHTS> color;
	// Material properties
	std::array<glm::vec3, MAX_LIGHTS> ambient_intensity;
	std::array<glm::vec3, MAX_LIGHTS> diffuse_intensity;
	std::array<glm::vec3, MAX_LIGHTS> specular_intensity;
	std::array<glm::vec3, MAX_LIGHTS> ambient_material;
	std::array<glm::vec3, MAX_LIGHTS> diffuse_material;
	std::array<glm::vec3, MAX_LIGHTS> specular_material;
	std::array<float, MAX_LIGHTS> specular_shinines;
};