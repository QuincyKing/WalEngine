#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "../render/Shader.h"
#include "../core/Entity.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Model : public Entity
{
public:
    vector<Tex> textures_loaded;	
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string name, 
		std::string const &path, 
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0),
		bool gamma = false) :
		Entity(name, pos, rot, scale),
		gammaCorrection(gamma)
    {
        load_model(path);
    }

	inline void render()
	{
		glm::mat4 model = mTransform->get_model();
		get_component<Material>(ComType::Mat)->mShader->use();
		get_component<Material>(ComType::Mat)->mShader->set_mat4("T_model", model);
		draw();
	}

    void draw()
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw();
    }
    
private:
	void load_model(string const &path);
	void process_node(aiNode *node, const aiScene *scene);
	Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
	vector<Tex> load_material_textures(aiMaterial *mat, aiTextureType type, string typeName);
};
