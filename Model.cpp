#include "Model.hpp"

#define MAX_LINE_SIZE 255

Model::Model(const std::filesystem::path& filename, ShaderProgram shader): shader(shader) {
	std::vector<glm::vec3> temp_postions;
	std::vector<glm::vec2> temp_texcoords;
	std::vector<glm::vec3> temp_normals;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	if (!LoadOBJFile(filename.string().c_str(), temp_postions, temp_texcoords, temp_normals))
		throw std::runtime_error("Failed to load OBJ file!");

	if (temp_postions.size() != temp_texcoords.size() || temp_texcoords.size() != temp_normals.size())
		throw std::runtime_error("OBJ file has inconsistent data!");

	vertices.reserve(temp_postions.size());
	indices.reserve(temp_postions.size());
	for (int i = 0; i < temp_postions.size(); i++) {
		Vertex vertex{};
		vertex.Position = temp_postions[i];
		vertex.TexCoords = temp_texcoords[i];
		vertex.Normal = temp_normals[i];
		vertices.push_back(vertex);
		indices.push_back(i);
	}

	glm::vec3 origin{0,0,0};
	glm::vec3 orientation{0,0,0};

	Mesh mesh = Mesh(GL_TRIANGLES, shader, vertices, indices, origin, orientation);

	meshes.push_back(mesh);

	//draw();
}

bool Model::LoadOBJFile(const char* path,
	std::vector < glm::vec3 >& out_vertices,
	std::vector < glm::vec2 >& out_uvs,
	std::vector < glm::vec3 >& out_normals){

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();

	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[MAX_LINE_SIZE];
		int res = fscanf_s(file, "%s", lineHeader, MAX_LINE_SIZE);
		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.y, &uv.x);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by simple parser :( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	// unroll from indirect to direct vertex specification
	// sometimes not necessary, definitely not optimal

	for (unsigned int u = 0; u < vertexIndices.size(); u++) {
		unsigned int vertexIndex = vertexIndices[u];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}
	for (unsigned int u = 0; u < uvIndices.size(); u++) {
		unsigned int uvIndex = uvIndices[u];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);
	}
	for (unsigned int u = 0; u < normalIndices.size(); u++) {
		unsigned int normalIndex = normalIndices[u];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);
	}

	fclose(file);
	return true;

	
}

void Model::draw(
	glm::vec3 const& offset, 
	glm::vec3 const& rotation,
	glm::vec3 const& scale_change) {

	// compute complete transformation
	glm::mat4 t = glm::translate(glm::mat4(1.0f), origin);
	glm::mat4 rx = glm::rotate(glm::mat4(1.0f), orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ry = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rz = glm::rotate(glm::mat4(1.0f), orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 m_off = glm::translate(glm::mat4(1.0f), offset);
	glm::mat4 m_rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 m_ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 m_rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m_s = glm::scale(glm::mat4(1.0f), scale_change);

	glm::mat4 model_matrix = local_model_matrix * s * rz * ry * rx * t * m_s * m_rz * m_ry * m_rx * m_off;


	// call draw() on mesh (all meshes)
	for (auto& mesh : meshes) {
		mesh.draw(model_matrix);
	}
}
void Model::draw(glm::mat4 const& model_matrix) {
	// draw all meshes
	for (auto & mesh : meshes) {
		mesh.draw(local_model_matrix * model_matrix);
	}
}