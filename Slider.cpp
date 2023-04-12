#include "Slider.h"

/*Slider::Slider(float x, float y, float width, float height, glm::vec3 color, wchar_t* text, void(*f)(void)) :Button(x, y, width, height, color, text, *f) {};
{
	return;
}*/

void Slider::Motion(float deltaX, float deltaY)
{
    float len = abs(highBorder - lowBorder);
    if (this->orient == horizontal) {
        if(posX + deltaX - width/2 >= lowBorder && posX + deltaX + width / 2 <= highBorder) posX += deltaX;
    }
    else {
        int deltaPos = (deltaY / height) * len;
        if (posY + deltaY - height / 2 >= lowBorder && posY + deltaY + height / 2 <= highBorder) posY += deltaY;
    }
}

void Slider::CheckClick(float x, float y)
{
    if (x >= this->posX - this->width / 2 && x <= this->posX + this->width / 2 
        &&
        y >= this->posY - this->height / 2 && y <= this->posY + this->height / 2) {

        Activate();
        
        //((void(*)())onClick)();
    }
}

void Slider::Draw(Shader& btnShader, float screenWidth, float screenHeight)
{
    //capture.RenderText(textShader, this->text, 6, left, top - height/2, 1, glm::vec3(255, 255, 255));
    float x = this->posX;
    float y = this->posY;
    btnShader.Use();
    projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
    GLint projLoc = glGetUniformLocation(btnShader.Program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    int w = this->width / 2;
    int h = this->height / 2;
    GLfloat vertices[] = {
         x - w,  y - h,    -0.01f,  // Bottom Left
         x + w,  y - h,    -0.01f,  // Bottom Right
         x + w,  y + h,    -0.01f,  // Top Right
         x + w,  y + h,    -0.01f,  // Top Right
         x - w,  y + h,    -0.01f,  // Top Left 
         x - w,  y - h,    -0.01f,  // Bottom Left



       /*-1.0f, -1.0f, 0.0f,  // Bottom Lefft
         1.0f, -1.0f, 0.0f,  // Bottom Right
         1.0f,  1.0f, 0.0f,  // Top Right
         1.0f,  1.0f, 0.0f,  // Top Right
        -1.0f,  1.0f, 0.0f,   // Top Left
        -1.0f, -1.0f, 0.0f,  // Bottom Left*/
    };
    btnShader.Use();
    btnShader.setVec3("buttonColor", this->sliderColor);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    btnShader.setVec3("buttonColor", this->lineColor);
    if (orient == horizontal) {
        float w = (highBorder - lowBorder);
        GLfloat vertices[] = {
            lowBorder,      y - h,    -0.01f,  // Bottom Left
            lowBorder + w,  y - h,    -0.01f,  // Bottom Right
            lowBorder + w,  y + h,    -0.01f,  // Top Right
            lowBorder + w,  y + h,    -0.01f,  // Top Right
            lowBorder,      y + h,    -0.01f,  // Top Left 
            lowBorder,      y - h,    -0.01f,  // Bottom Left
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    else {
        float h = abs(highBorder - lowBorder);
        GLfloat vertices[] = {
            x - w,  lowBorder,        -0.01f,  // Bottom Left
            x + w,  lowBorder,        -0.01f,  // Bottom Right
            x + w,  lowBorder + h,    -0.01f,  // Top Right
            x + w,  lowBorder + h,    -0.01f,  // Top Right
            x - w,  lowBorder + h,    -0.01f,  // Top Left 
            x - w,  lowBorder,        -0.01f,  // Bottom Left
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
   

    

    glBindVertexArray(0);
}

void Slider::Activate()
{
    this->sliderColor = this->activeColor;
    isActive = true;
}

void Slider::Deactivate()
{
    this->sliderColor = this->unactiveColor;
    isActive = false;
}

bool Slider::IsActive()
{
    return isActive;
}

float Slider::GetContentPercent()
{
    float res;
    if (orient == horizontal) {
        res = (posX - width / 2 - lowBorder) / (highBorder - lowBorder);
    }
    else {
        res = (posY - height / 2 - lowBorder) / (lowBorder - highBorder);
    }
    return res;
}

float Slider::GetValuePercent()
{
    float res;
    if (orient == horizontal) {
        res = (posX - width / 2) / (highBorder - lowBorder - width / 2);
    }
    else {
        res = (posY - height / 2) / abs(lowBorder - highBorder - height / 2);
    }
    return res;
}

void Slider::SetSliderSize(float percent)
{
    if (orient == horizontal) {
        if (percent >= 1) this->width = abs(highBorder - lowBorder);
        else this->width = percent * abs(highBorder - lowBorder);
        posX = lowBorder + width / 2;
    }
    else {
        if (percent >= 1) this->height = abs(highBorder - lowBorder);
        else this->height = percent * abs(highBorder - lowBorder);
        posY = lowBorder - height / 2;
    }
}
