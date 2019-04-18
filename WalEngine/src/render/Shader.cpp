#include "Shader.h"
#include "../core/Util.h"
#include <cassert>

int ShaderData::SupportedOpenGLLevel = 0;
std::string ShaderData::GlslVersion = "";

ShaderData::ShaderData(const std::string& fileName)
{
	std::string actualFileName = fileName;

	mProgram = glCreateProgram();

	if (mProgram == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	if (SupportedOpenGLLevel == 0)
	{
		int majorVersion;
		int minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		SupportedOpenGLLevel = majorVersion * 100 + minorVersion * 10;

		if (SupportedOpenGLLevel >= 330)
		{
			std::ostringstream os;
			os << SupportedOpenGLLevel;

			GlslVersion = os.str();
		}
		else if (SupportedOpenGLLevel >= 320)
		{
			GlslVersion = "150";
		}
		else if (SupportedOpenGLLevel >= 310)
		{
			GlslVersion = "140";
		}
		else if (SupportedOpenGLLevel >= 300)
		{
			GlslVersion = "130";
		}
		else if (SupportedOpenGLLevel >= 210)
		{
			GlslVersion = "120";
		}
		else if (SupportedOpenGLLevel >= 200)
		{
			GlslVersion = "110";
		}
		else
		{
			fprintf(stderr, "Error: OpenGL Version %d.%d does not support shaders.\n", majorVersion, minorVersion);
			exit(1);
		}
	}

	std::string shaderText = LoadShader(actualFileName);
	std

	std::string vertexShaderText = "#version " + GlslVersion + "\n#define VS_BUILD\n#define GLSL_VERSION " + GlslVersion + "\n" + shaderText;
	std::string fragmentShaderText = "#version " + GlslVersion + "\n#define FS_BUILD\n#define GLSL_VERSION " + GlslVersion + "\n" + shaderText;

	add_vertex_shader(vertexShaderText);
	add_fragment_shader(fragmentShaderText);

	std::string attributeKeyword = "attribute";
	add_all_attributes(vertexShaderText, attributeKeyword);

	compile_shader();

	add_shader_uniforms(shaderText);
}

ShaderData::~ShaderData()
{
	for (std::vector<int>::iterator it = mShaders.begin(); it != mShaders.end(); ++it)
	{
		glDetachShader(mProgram, *it);
		glDeleteShader(*it);
	}
	glDeleteProgram(mProgram);
}


void ShaderData::add_vertex_shader(const std::string& text)
{
	add_program(text, GL_VERTEX_SHADER);
}

void ShaderData::add_geometry_shader(const std::string& text)
{
	add_program(text, GL_GEOMETRY_SHADER);
}

void ShaderData::add_fragment_shader(const std::string& text)
{
	add_program(text, GL_FRAGMENT_SHADER);
}

void ShaderData::add_program(const std::string& text, int type)
{
	int shader = glCreateShader(type);

	if (shader == 0)
	{
		fprintf(stderr, "Error creating shader type %d\n", type);
		exit(1);
	}

	const GLchar* p[1];
	p[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];

		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader, InfoLog);

		exit(1);
	}

	glAttachShader(mProgram, shader);
	mShaders.push_back(shader);
}

void ShaderData::add_all_attributes(const std::string& vertexShaderText, const std::string& attributeKeyword)
{
	int currentAttribLocation = 0;
	size_t attributeLocation = vertexShaderText.find(attributeKeyword);
	while (attributeLocation != std::string::npos)
	{
		bool isCommented = false;
		size_t lastLineEnd = vertexShaderText.rfind("\n", attributeLocation);

		if (lastLineEnd != std::string::npos)
		{
			std::string potentialCommentSection = vertexShaderText.substr(lastLineEnd, attributeLocation - lastLineEnd);

			//Potential false positives are both in comments, and in macros.
			isCommented = potentialCommentSection.find("//") != std::string::npos || potentialCommentSection.find("#") != std::string::npos;
		}

		if (!isCommented)
		{
			size_t begin = attributeLocation + attributeKeyword.length();
			size_t end = vertexShaderText.find(";", begin);

			std::string attributeLine = vertexShaderText.substr(begin + 1, end - begin - 1);

			begin = attributeLine.find(" ");
			std::string attributeName = attributeLine.substr(begin + 1);

			glBindAttribLocation(mProgram, currentAttribLocation, attributeName.c_str());
			currentAttribLocation++;
		}
		attributeLocation = vertexShaderText.find(attributeKeyword, attributeLocation + attributeKeyword.length());
	}
}

void ShaderData::add_shader_uniforms(const std::string& shaderText)
{
	static const std::string UNIFORM_KEY = "uniform";

	std::vector<UniformStruct> structs = FindUniformStructs(shaderText);

	size_t uniformLocation = shaderText.find(UNIFORM_KEY);
	while (uniformLocation != std::string::npos)
	{
		bool isCommented = false;
		size_t lastLineEnd = shaderText.rfind("\n", uniformLocation);

		if (lastLineEnd != std::string::npos)
		{
			std::string potentialCommentSection = shaderText.substr(lastLineEnd, uniformLocation - lastLineEnd);
			isCommented = potentialCommentSection.find("//") != std::string::npos;
		}

		if (!isCommented)
		{
			size_t begin = uniformLocation + UNIFORM_KEY.length();
			size_t end = shaderText.find(";", begin);

			std::string uniformLine = shaderText.substr(begin + 1, end - begin - 1);

			begin = uniformLine.find(" ");
			std::string uniformName = uniformLine.substr(begin + 1);
			std::string uniformType = uniformLine.substr(0, begin);

			mUniformNames.push_back(uniformName);
			mUniformTypes.push_back(uniformType);
			add_uniform(uniformName, uniformType, structs);
		}
		uniformLocation = shaderText.find(UNIFORM_KEY, uniformLocation + UNIFORM_KEY.length());
	}
}

void ShaderData::add_uniform(const std::string& uniformName, const std::string& uniformType, const std::vector<UniformStruct>& structs)
{
	bool addThis = true;

	for (unsigned int i = 0; i < structs.size(); i++)
	{
		if (structs[i].get_name().compare(uniformType) == 0)
		{
			addThis = false;
			for (unsigned int j = 0; j < structs[i].get_membernames().size(); j++)
			{
				add_uniform(uniformName + "." + structs[i].get_membernames()[j].get_name(), structs[i].get_membernames()[j].get_type(), structs);
			}
		}
	}

	if (!addThis)
		return;

	unsigned int location = glGetUniformLocation(mProgram, uniformName.c_str());

	assert(location != INVALID_VALUE);

	mUniformMap.insert(std::pair<std::string, unsigned int>(uniformName, location));
}

void ShaderData::compile_shader() const
{
	glLinkProgram(mProgram);
	CheckShaderError(mProgram, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(mProgram);
	CheckShaderError(mProgram, GL_VALIDATE_STATUS, true, "Invalid shader program");
}

//--------------------------------------------------------------------------------
// Static Function Implementations
//--------------------------------------------------------------------------------
static void CheckShaderError(int shader, int flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (!success)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		fprintf(stderr, "%s: '%s'\n", errorMessage.c_str(), error);
	}
}

static std::string LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open(("./shader/" + fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);

			if (line.find("#include") == std::string::npos)
				output.append(line + "\n");
			else
			{
				std::string includeFileName = Util::split(line, ' ')[1];
				includeFileName = includeFileName.substr(1, includeFileName.length() - 2);

				std::string toAppend = LoadShader(includeFileName);
				output.append(toAppend + "\n");
			}
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
};


static std::vector<TypedData> FindUniformStructComponents(const std::string& openingBraceToClosingBrace)
{
	static const char charsToIgnore[] = { ' ', '\n', '\t', '{' };
	static const size_t UNSIGNED_NEG_ONE = (size_t)-1;

	std::vector<TypedData> result;
	std::vector<std::string> structLines = Util::split(openingBraceToClosingBrace, ';');

	for (unsigned int i = 0; i < structLines.size(); i++)
	{
		size_t nameBegin = UNSIGNED_NEG_ONE;
		size_t nameEnd = UNSIGNED_NEG_ONE;

		for (unsigned int j = 0; j < structLines[i].length(); j++)
		{
			bool isIgnoreableCharacter = false;

			for (unsigned int k = 0; k < sizeof(charsToIgnore) / sizeof(char); k++)
			{
				if (structLines[i][j] == charsToIgnore[k])
				{
					isIgnoreableCharacter = true;
					break;
				}
			}

			if (nameBegin == UNSIGNED_NEG_ONE && isIgnoreableCharacter == false)
			{
				nameBegin = j;
			}
			else if (nameBegin != UNSIGNED_NEG_ONE && isIgnoreableCharacter)
			{
				nameEnd = j;
				break;
			}
		}

		if (nameBegin == UNSIGNED_NEG_ONE || nameEnd == UNSIGNED_NEG_ONE)
			continue;

		TypedData newData(
			structLines[i].substr(nameEnd + 1),
			structLines[i].substr(nameBegin, nameEnd - nameBegin));

		result.push_back(newData);
	}

	return result;
}

static std::string FindUniformStructName(const std::string& structStartToOpeningBrace)
{
	return Util::split(Util::split(structStartToOpeningBrace, ' ')[0], '\n')[0];
}

static std::vector<UniformStruct> FindUniformStructs(const std::string& shaderText)
{
	static const std::string STRUCT_KEY = "struct";
	std::vector<UniformStruct> result;

	size_t structLocation = shaderText.find(STRUCT_KEY);
	while (structLocation != std::string::npos)
	{
		structLocation += STRUCT_KEY.length() + 1; //Ignore the struct keyword and space

		size_t braceOpening = shaderText.find("{", structLocation);
		size_t braceClosing = shaderText.find("}", braceOpening);

		UniformStruct newStruct(
			FindUniformStructName(shaderText.substr(structLocation, braceOpening - structLocation)),
			FindUniformStructComponents(shaderText.substr(braceOpening, braceClosing - braceOpening)));

		result.push_back(newStruct);
		structLocation = shaderText.find(STRUCT_KEY, structLocation);
	}

	return result;
}
