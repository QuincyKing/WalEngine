#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "../core/Ref.h"

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

class ShaderData : public Ref
{
public:
	ShaderData() {};
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
	void add_program(const std::string& text, int type);
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

class Shader : public Ref
{
public:
	friend class Material;

public:
	Shader(const std::string& vsFile = "default.vert", const std::string& fsFile = "default.frag");
	Shader(const Shader& other);
	virtual ~Shader();

	void use() const;
	void set_int(const std::string& uniformName, int value) const;
	void set_float(const std::string& uniformName, float value) const;
	void set_mat4(const std::string& uniformName, const glm::mat4& value) const;
	void set_vec3(const std::string& uniformName, const glm::vec3& value) const;
	Shader& operator=(const Shader &other);
	bool is_default();
	void set_shader(const std::string& vsFile, const std::string& fsFile);
	
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