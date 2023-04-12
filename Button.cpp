#include "Button.h"

Button::Button(float x, float y, float width, float height, glm::vec3 unactiveColor, glm::vec3 activeColor, wchar_t* text, void(*f)(int), int num){
    this->num = num;
    this->posX = x;
    this->posY = y;
    this->width = width;
    this->height = height;
    this->unactiveColor = unactiveColor;
    this->activeColor = activeColor;
    this->text = text;
    this->captureLen = wcslen(text);
    this->fontSize = width / (captureLen + 2);
    this->lineSpacing = fontSize / 2;
    onClick = f;
    SetUnpressed();
}

Button::~Button() {

}

void Button::Init() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    SeparateLines(text);
    
    bool twoLines = false;
    int idx;
    wchar_t c;
    for (int i = 0; i < captureLen && !twoLines; i++)
    {
        c = *(text + i * sizeof(wchar_t) / 2);
        if (L'\n' == c) {
            twoLines = true;
            idx = i;
        }
    }
    int bigPart;
    if (twoLines) {
        if (idx > captureLen / 2) bigPart = idx;
        else bigPart = captureLen - idx - 1;
        fontSize = width / (bigPart) + bigPart / 6;
        captureX = posX - width / 2 + 2*fontSize;
        captureY = captureY = posY + fontSize / 2;
    }
    else{
        fontSize = width / (captureLen + 2);
        captureX = posX  - width / 2 + 2*fontSize;
        captureY = posY - fontSize / 2;
    }
    
    
    capture.Init(fontSize, lineSpacing);
}

void Button::CheckClick(double x, double y) {
    if(!disabled)
    if (x >= posX - width / 2 && x <= posX + width / 2 && y >= posY - height / 2 && y <= posY + height / 2) {
        if (!isPressed) SetPressed();
        else SetUnpressed();
        ((void(*)(int))onClick)(num);
    }
    
}

void Button::SetText(wchar_t* text)
{
    this->text = text;
}

void Button::SetPos(float x, float y)
{
    captureX += (x - posX);
    captureY += (y - posY);
    posX = x;
    posY = y;

}

void Button::SetPosX(float x)
{
    captureX += (x - posX);
    posX = x;
}

void Button::SetPosY(float y)
{
    captureY += (y - posY);
    posY = y;
}

void Button::Draw(Shader& btnShader, Shader& textShader, float screenWidth, float screenHeight)
{
    //capture.RenderText(textShader, this->text, 6, left, top - height/2, 1, glm::vec3(255, 255, 255));
    float x = this->posX;
    float y = this->posY;
    btnShader.Use();
    projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
    GLint projLoc = glGetUniformLocation(btnShader.Program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    btnShader.setVec3("buttonColor", this->color);
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
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glEnableVertexAttribArray(0);
    btnShader.Use();
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    capture.RenderText(textShader, this->text, captureX, captureY, glm::vec3(255, 255, 255));
    
}

void Button::SetPressed()
{
    this->color = this->activeColor;
    isPressed = true;
}

void Button::SetUnpressed()
{
    this->color = this->unactiveColor;
    isPressed = false;
}

bool Button::IsPressed()
{
    return isPressed;
}

void Button::Disable()
{
    disabled = true;
    this->color = disabledColor;
    isPressed = false;
}

void Button::Able()
{
    disabled = false;
    if (isPressed) this->color = activeColor;
    this->color = unactiveColor;
}

void Button::SeparateLines(wchar_t* text)
{
    int len = wcslen(text);
    int separateIdx = -1;
    for (int i = 0; i < len; i++) {
        if (text[i] == L' ') {
            if (abs(len / 2 - i) < abs(len / 2 - separateIdx)) {
                separateIdx = i;
            }
        }
    }
    if (len > 10) text[separateIdx] = L'\n';
}

wchar_t* Button::GetSubstring(wchar_t* s, int start, int n)
{
    if (n < 1) return NULL;
    wchar_t* res = (wchar_t*)malloc(sizeof(wchar_t) * (n + 1));
    int i;
    for (i = 0; i < n; i++) {
        res[i] = s[start + i];
    }
    res[i] = '\0';
    return res;
}
