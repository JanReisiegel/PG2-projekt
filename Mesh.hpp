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

		GLuint prog_h = shader.getProgramID();
		//Vao, Vbo, Ebo
        // Generate the VAO
        glCreateVertexArrays(1, &VAO);
        // Set Vertex Attribute to explain OpenGL how to interpret the data
        GLint position_attrib_location = glGetAttribLocation(prog_h, "aPos");
        glVertexArrayAttribFormat(VAO, position_attrib_location, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(VAO, position_attrib_location, 0);
        glEnableVertexArrayAttrib(VAO, position_attrib_location);
        // Create and fill data
        glCreateBuffers(1, &VBO); // Vertex Buffer Object
        glNamedBufferData(VBO, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glCreateBuffers(1, &EBO); // Element Buffer Object
        glNamedBufferData(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        // Connect together
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(VAO, EBO);
        // USE
        //glUseProgram(shaderProgram);
		shader.activate();
        glBindVertexArray(VAO);
        
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
        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //TODO: draw mesh: bind vertex array object, draw all elements with selected primitive type 
    }


	void clear(void) {
        texture_id = 0;
        primitive_type = GL_POINT;
        // TODO: clear rest of the member variables to safe default
        
        // TODO: delete all allocations 
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &EBO);
        
    };

private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
     unsigned int VAO{0}, VBO{0}, EBO{0};
};
  


