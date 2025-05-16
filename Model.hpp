#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 

#include "assets.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
	glm::vec3 scale{1.0f};
	glm::mat4 local_model_matrix{1.0f};
    glm::vec3 orientation{};

    //jump
    GLfloat gravity = 0.03f;
    GLfloat jump_velocity;
    GLboolean can_jump = true;

	ShaderProgram shader; //{0};
	GLuint texture_id{ 0 }; // texture id=0  means no texture
    bool transparent { false };
    Model(const std::filesystem::path& filename, ShaderProgram shader); //{
    Model(GLenum type, std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices, ShaderProgram shader, GLuint const texture_id);
    
    void draw(glm::vec3 const& offset = glm::vec3(0.0),
        glm::vec3 const& rotation = glm::vec3(0.0f),
        glm::vec3 const& scale_change = glm::vec3(1.0f));

    void draw(glm::mat4 const& model_matrix);
    bool LoadOBJFile(
        const char* path,
        std::vector < glm::vec3 >& out_vertices,
        std::vector < glm::vec2 >& out_uvs,
        std::vector < glm::vec3 >& out_normals
    );

    void jump(float deltaTime);
    void clear(void);
};

