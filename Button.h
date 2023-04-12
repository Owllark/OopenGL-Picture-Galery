#pragma once
#ifndef BUTTON_H
#define BUTTON_H

//  GL Includes
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#include "Shader.h"
#include "TextRender.h"

//#include "TextRendering.h"

class Button {
public:
    Button(float x, float y, float width, float height, glm::vec3 unactiveColor, glm::vec3 activeColor, wchar_t* text, void(*f)(int), int num);
    ~Button();
    void Init();
    void CheckClick(double x, double y);
    void SetText(wchar_t* text);
    void SetPos(float x, float y);
    void SetPosX(float x);
    void SetPosY(float y);
    void Draw(Shader& btnShader, Shader& textShader, float width, float height);
    void SetPressed();
    void SetUnpressed();
    bool IsPressed();
    void Disable();
    void Able();
    bool disabled = false;
    
protected:
    void SeparateLines(wchar_t* text);
    wchar_t* GetSubstring(wchar_t* s, int start, int n); //returns n symbols from s string starting with symbol with 'start' inde
    int num;
    float posY;
    float posX;
    bool isPressed;
    wchar_t* text;
    float width;
    float height;
    float b;
    int fontSize;
    int lineSpacing;
    int captureLen;
    float captureX, captureY;
    glm::mat4 projection = glm::mat4(1.0f);
    glm::vec3 color;
    glm::vec3 unactiveColor;
    glm::vec3 activeColor;
    glm::vec3 disabledColor = glm::vec3(0.35, 0.35, 0.35);
    
    void* onClick;
    TextRender capture;
    GLuint VAO, VBO;
};

#endif
