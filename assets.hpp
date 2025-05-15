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
	// Material
	std::array<glm::vec3, MAX_LIGHTS> ambient_intensity;
	std::array<glm::vec3, MAX_LIGHTS> diffuse_intensity;
	std::array<glm::vec3, MAX_LIGHTS> specular_intensity;
	std::array<glm::vec3, MAX_LIGHTS> ambient_material;
	std::array<glm::vec3, MAX_LIGHTS> diffuse_material;
	std::array<glm::vec3, MAX_LIGHTS> specular_material;
	std::array<float, MAX_LIGHTS> specular_shinines;

	// Spotlight specific
	std::array<glm::vec3, MAX_LIGHTS> spot_direction;
	std::array<float, MAX_LIGHTS> cos_cutoff;
	std::array<float, MAX_LIGHTS> spot_exponent;

	//attenuation
	std::array<float, MAX_LIGHTS> constant;
	std::array<float, MAX_LIGHTS> linear;
	std::array<float, MAX_LIGHTS> quadratic;
};