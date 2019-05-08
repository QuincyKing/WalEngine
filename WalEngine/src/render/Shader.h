#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "../core/ReferenceCounter.h"

class RenderEngine;
class Material;

class TypedData
{
public:
	TypedData(const std::string& name, const std::string& type) :
		mName(name),
		mType(type) {}

	inline const std::string& get_name() const { return mName; }
	inline const std::string& get_type() const { return mType; }

private:
	std::string mName;
	std::string mType;
};

class UniformStruct
{
public:
	UniformStruct(const std::string& name, const std::vector<TypedData>& memberNames) :
		mName(name),
		mMemberNames(memberNames) {}

	inline const std::string& get_name()                   const { return mName; }
	inline const std::vector<TypedData>& get_membernames() const { return mMemberNames; }

private:
	std::string            mName;		 //struct name
	std::vector<TypedData> mMemberNames; //all member name of struct
};

class ShaderData : public ReferenceCounter
{
public:
	ShaderData();
	virtual ~ShaderData();

	inline int get_program()                                            const { return mProgram; }
	inline const std::vector<int>& get_shaders()                        const { return mShaders; }
	inline const std::vector<std::string>& get_uniform_names()          const { return mUniformNames; }
	inline const std::vector<std::string>& get_uniform_types()          const { return mUniformTypes; }
	inline const std::map<std::string, unsigned int>& get_uniform_map() const { return mUniformMap; }
	void init(const std::string& file, int type);
	void compile_shader() const;
	void add_shader_uniforms(const std::string& shaderText);

private:
	inline void add_vertex_shader(const std::string& text) { add_program(text, GL_VERTEX_SHADER); }
	inline void add_geometry_shader(const std::string& text) { add_program(text, GL_GEOMETRY_SHADER); }
	inline void add_fragment_shader(const std::string& text) { add_program(text, GL_FRAGMENT_SHADER); }
	void add_program(const std::string& text, int type);

	//void add_all_attributes(const std::string& vertexShaderText, const std::string& attributeKeyword);
	void add_uniform(const std::string& uniformName, const std::string& uniformType, const std::vector<UniformStruct>& structs);


private:
	static int SupportedOpenGLLevel;
	static std::string GlslVersion;
	
	std::vector<int>                    mShaders;
	std::vector<std::string>            mUniformNames;
	std::vector<std::string>            mUniformTypes;
	std::map<std::string, unsigned int> mUniformMap;
	unsigned int mProgram;
};

class Shader
{
public:
	Shader(const std::string& vsFile = "default.vert", const std::string& fsFile = "default.frag");
	Shader(const Shader& other);
	virtual ~Shader();

	void use() const;
	void init();
	//virtual void update_uniforms(const Transform& transform, const Material& material, 
		//const RenderEngine& renderEngine, const Camera& camera) const;

	void set_int(const std::string& uniformName, int value) const;
	void set_float(const std::string& uniformName, float value) const;
	void set_mat4(const std::string& uniformName, const glm::mat4& value) const;
	void set_vec3(const std::string& uniformName, const glm::vec3& value) const;
	Shader& operator=(const Shader &other);
	bool is_default();
	
private:
	static std::map<std::string, ShaderData*> ResourceMap;

	ShaderData* mShaderData;
	std::string mVsName;
	std::string mFsName;
	std::string mGeName;

	/*void set_uniform_dirlight(const std::string& uniformName, const DirectionalLight& value) const;
	void set_uniform_pointlight(const std::string& uniformName, const PointLight& value) const;
	void set_uniform_spotlight(const std::string& uniformName, const SpotLight& value) const;*/
};

//class Shader
//{
//public:
//    unsigned int ID;
//    // constructor generates the shader on the fly
//    // ------------------------------------------------------------------------
//	Shader()
//	{
//
//	}
//
//    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
//    {
//        // 1. retrieve the vertex/fragment source code from filePath
//        std::string vertexCode;
//        std::string fragmentCode;
//        std::string geometryCode;
//        std::ifstream vShaderFile;
//        std::ifstream fShaderFile;
//        std::ifstream gShaderFile;
//        // ensure ifstream objects can throw exceptions:
//        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        try 
//        {
//            // open files
//            vShaderFile.open(vertexPath);
//            fShaderFile.open(fragmentPath);
//            std::stringstream vShaderStream, fShaderStream;
//            // read file's buffer contents into streams
//            vShaderStream << vShaderFile.rdbuf();
//            fShaderStream << fShaderFile.rdbuf();		
//            // close file handlers
//            vShaderFile.close();
//            fShaderFile.close();
//            // convert stream into string
//            vertexCode = vShaderStream.str();
//            fragmentCode = fShaderStream.str();			
//            // if geometry shader path is present, also load a geometry shader
//            if(geometryPath != nullptr)
//            {
//                gShaderFile.open(geometryPath);
//                std::stringstream gShaderStream;
//                gShaderStream << gShaderFile.rdbuf();
//                gShaderFile.close();
//                geometryCode = gShaderStream.str();
//            }
//        }
//        catch (std::ifstream::failure e)
//        {
//            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//        }
//        const char* vShaderCode = vertexCode.c_str();
//        const char * fShaderCode = fragmentCode.c_str();
//        // 2. compile shaders
//        unsigned int vertex, fragment;
//        int success;
//        char infoLog[512];
//        // vertex shader
//        vertex = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vertex, 1, &vShaderCode, NULL);
//        glCompileShader(vertex);
//        checkCompileErrors(vertex, "VERTEX");
//        // fragment Shader
//        fragment = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragment, 1, &fShaderCode, NULL);
//        glCompileShader(fragment);
//        checkCompileErrors(fragment, "FRAGMENT");
//        // if geometry shader is given, compile geometry shader
//        unsigned int geometry;
//        if(geometryPath != nullptr)
//        {
//            const char * gShaderCode = geometryCode.c_str();
//            geometry = glCreateShader(GL_GEOMETRY_SHADER);
//            glShaderSource(geometry, 1, &gShaderCode, NULL);
//            glCompileShader(geometry);
//            checkCompileErrors(geometry, "GEOMETRY");
//        }
//        // shader Window
//        ID = glCreateProgram();
//        glAttachShader(ID, vertex);
//        glAttachShader(ID, fragment);
//        if(geometryPath != nullptr)
//            glAttachShader(ID, geometry);
//        glLinkProgram(ID);
//        checkCompileErrors(ID, "PROGRAM");
//        // delete the shaders as they're linked into our program now and no longer necessery
//        glDeleteShader(vertex);
//        glDeleteShader(fragment);
//        if(geometryPath != nullptr)
//            glDeleteShader(geometry);
//
//    }
//    // activate the shader
//    // ------------------------------------------------------------------------
//    void use() 
//    { 
//        glUseProgram(ID); 
//    }
//    // utility uniform functions
//    // ------------------------------------------------------------------------
//    void setBool(const std::string &name, bool value) const
//    {         
//        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
//    }
//    // ------------------------------------------------------------------------
//    void setInt(const std::string &name, int value) const
//    { 
//        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
//    }
//    // ------------------------------------------------------------------------
//    void setFloat(const std::string &name, float value) const
//    { 
//        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
//    }
//    // ------------------------------------------------------------------------
//    void setVec2(const std::string &name, const glm::vec2 &value) const
//    { 
//        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//    }
//    void setVec2(const std::string &name, float x, float y) const
//    { 
//        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
//    }
//    // ------------------------------------------------------------------------
//    void setVec3(const std::string &name, const glm::vec3 &value) const
//    { 
//        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//    }
//    void setVec3(const std::string &name, float x, float y, float z) const
//    { 
//        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
//    }
//    // ------------------------------------------------------------------------
//    void setVec4(const std::string &name, const glm::vec4 &value) const
//    { 
//        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//    }
//    void setVec4(const std::string &name, float x, float y, float z, float w) 
//    { 
//        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
//    }
//    // ------------------------------------------------------------------------
//    void setMat2(const std::string &name, const glm::mat2 &mat) const
//    {
//        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//    }
//    // ------------------------------------------------------------------------
//    void setMat3(const std::string &name, const glm::mat3 &mat) const
//    {
//        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//    }
//    // ------------------------------------------------------------------------
//    void setMat4(const std::string &name, const glm::mat4 &mat) const
//    {
//        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//    }
//
//private:
//    // utility function for checking shader compilation/linking errors.
//    // ------------------------------------------------------------------------
//    void checkCompileErrors(GLuint shader, std::string type)
//    {
//        GLint success;
//        GLchar infoLog[1024];
//        if(type != "PROGRAM")
//        {
//            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//            if(!success)
//            {
//                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//            }
//        }
//        else
//        {
//            glGetProgramiv(shader, GL_LINK_STATUS, &success);
//            if(!success)
//            {
//                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
//                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//            }
//        }
//    }
//};