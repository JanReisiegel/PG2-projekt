#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>

#include "assets.hpp"
#include "ShaderProgram.hpp"
#include "assert.h"
#include <iostream>
                         
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
    Mesh(GLenum primitive_type, ShaderProgram shader, std::vector<Vertex> const& vertices, std::vector<GLuint> const& indices, glm::vec3 const& origin, glm::vec3 const& orientation, GLuint const texture_id = 0) :
        primitive_type(primitive_type),
        shader(shader),
        vertices(vertices),
        indices(indices),
        origin(origin),
        orientation(orientation),
        texture_id(texture_id) {


		//Vao, Vbo, Ebo
        glCreateVertexArrays(1, &VAO);

        glCreateBuffers(1, &VBO);
        glNamedBufferStorage(VBO, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &EBO);
        glNamedBufferStorage(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayElementBuffer(VAO, EBO);

        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        glVertexArrayAttribBinding(VAO, 0, 0);

        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
        glVertexArrayAttribBinding(VAO, 1, 0);

        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
        glVertexArrayAttribBinding(VAO, 2, 0);

        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        
    };
        // TODO: create and initialize VAO, VBO, EBO and parameters;

    
    void draw(glm::vec3 const & offset, glm::vec3 const & rotation ) {
 		if (VAO == 0) {
			std::cerr << "VAO not initialized!\n";
			return;
		}
 
        shader.activate();
        
        // for future use: set uniform variables: position, textures, etc...  
        //set texture id etc...
        //if (texture_id > 0) {
        //    ...
        //}
        
        //TODO: draw mesh: bind vertex array object, draw all elements with selected primitive type 
        glBindVertexArray(VAO);
        glDrawElements(primitive_type, indices.size(), GL_UNSIGNED_INT, 0);
    }


	void clear(void) {
        texture_id = 0;
        primitive_type = GL_POINT;
        // TODO: clear rest of the member variables to safe default
        // ----maybe wrong
        VBO = 0;
        VAO = 0;
        EBO = 0;
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &EBO);
        
        // TODO: delete all allocations 
        vertices.clear();
        indices.clear();
    };

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
     unsigned int VAO{0}, VBO{0}, EBO{0};
};
  


