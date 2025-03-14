#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>

#include "assets.hpp"
#include "ShaderProgram.hpp"
#include "assert.h"
#include <iostream>
#include <GLFW/glfw3.h>
                         
class Mesh {
public:
    // mesh data
    glm::vec3 origin{};
    glm::vec3 orientation{};
    
    GLuint texture_id{0}; // texture id=0  means no texture
    GLenum primitive_type = GL_POINT;
    ShaderProgram shader;
    
    // mesh material
    glm::vec4 ambient_material{1.0f}; //white, non-transparent 
    glm::vec4 diffuse_material{1.0f}; //white, non-transparent 
    glm::vec4 specular_material{1.0f}; //white, non-transparent
    float reflectivity{1.0f}; 
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
    
    // indirect (indexed) draw 
    Mesh(
        GLenum primitive_type, 
        ShaderProgram shader, 
        std::vector<Vertex> const& vertices, 
        std::vector<GLuint> const& indices, 
        glm::vec3 const& origin, 
        glm::vec3 const& orientation, 
        GLuint const texture_id = 0);
        // TODO: create and initialize VAO, VBO, EBO and parameters;

    
    void draw(glm::vec3 const& offset, glm::vec3 const& rotation);

	void draw(glm::mat4 const& model_matrix);

    void clear(void);

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
     unsigned int VAO{0}, VBO{0}, EBO{0};
};
  


