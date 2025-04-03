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
	ShaderProgram shader; //{0};
	GLuint texture_id{ 0 }; // texture id=0  means no texture
    bool transparent { false };
    void addTexture(GLuint);
    Model(const std::filesystem::path& filename, ShaderProgram shader); //{
        // load mesh (all meshes) of the model, (in the future: load material of each mesh, load textures...)
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
        //    notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model   
		
    //}

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; // s = s0 + v*dt
    }
    
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
    void clear(void);
};

