#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

struct Character {
    GLuint     textureID; // ID текстуры глифа
    glm::ivec2 size;      // Размеры глифа
    glm::ivec2 bearing;   // Смещение верхней левой точки глифа
    GLuint     advance;   // Горизонтальное смещение до начала следующего глифа
};

const wchar_t cyrillic[] = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";

class TextRender
{
    public:
        
    
    TextRender();

    void Init(int fontSize, int lineSpacing);

    void RenderText(Shader& shader, wchar_t* text, GLfloat x, GLfloat y, glm::vec3 color);
    

    private:
        std::map<wchar_t, Character> Characters;
        GLuint VAO, VBO;
        glm::mat4 projection;
        int fontSize;
        int lineSpacing;
};

