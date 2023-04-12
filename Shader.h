#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>; // Подключаем glew для того, чтобы получить все необходимые заголовочные файлы OpenGL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:

	GLuint Program;
	// The constructor reads and assembles the shader 
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// Program using
	void Use();
	void setVec3(std::string attributeName, float x, float y, float z);
	void setVec3(std::string attributeName, glm::vec3 v);
	void setFloat(std::string attributeName, float num);
	void setInt(std::string attributeName, int num);

};

