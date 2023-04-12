#include "Shader.h"



Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// �������� ��� �������
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		if (!vShaderFile)
			std::cout << "ERROR::FILE_NOT_FOUND::" << vertexPath << std::endl;
		fShaderFile.open(fragmentPath);
		if (!fShaderFile)
			std::cout << "ERROR::FILE_NOT_FOUND::" << fragmentPath << std::endl;
		std::stringstream vShaderStream, fShaderStream;
		//���������� ������ � ������
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	// ������ �������� 
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// ��������� ������
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// ����� ������
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ����������� ������
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// ����� ������
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	// ����� ������
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// �������� ��������
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

void Shader::Use()
{
	glUseProgram(this->Program);
}

void Shader::setVec3(std::string attributeName, float x, float y, float z)
{
	const GLchar* attribute = attributeName.c_str();
	GLint Loc = glGetUniformLocation(this->Program, attribute);
	glUniform3f(Loc, x, y, z);
}

void Shader::setVec3(std::string attributeName, glm::vec3 v)
{
	const GLchar* attribute = attributeName.c_str();
	GLint Loc = glGetUniformLocation(this->Program, attribute);
	glUniform3f(Loc, v.x, v.y, v.z);
}

void Shader::setFloat(std::string attributeName, float num)
{
	const GLchar* attribute = attributeName.c_str();
	GLint Loc = glGetUniformLocation(this->Program, attribute);
	glUniform1f(Loc, num);
}

void Shader::setInt(std::string attributeName, int num)
{
	const GLchar* attribute = attributeName.c_str();
	GLint Loc = glGetUniformLocation(this->Program, attribute);
	glUniform1i(Loc, num);
}

