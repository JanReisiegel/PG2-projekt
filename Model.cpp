#include "Model.hpp"

Model::Model(const std::filesystem::path& filename, ShaderProgram shader) {
	std::vector<glm::vec3> temp_postions;
	std::vector<glm::vec2> temp_texcoords;
	std::vector<glm::vec3> temp_normals;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	if (!LoadOBJFile(filename.c_str(), temp_postions, temp_texcoords, temp_normals))
		throw std::runtime_error("Failed to load OBJ file!");
}

void Model::LoadOBJFile{

}