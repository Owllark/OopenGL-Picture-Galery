#include "TextRender.h"

TextRender::TextRender()
{
    
}

void TextRender::Init(int fontSize, int lineSpacing)
{
    this->fontSize = fontSize;
    this->lineSpacing = lineSpacing;
    setlocale(LC_ALL, "Russian"); // Russian localization

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arnamu.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (wchar_t c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
        // Characters[c] = character;
    }
    wchar_t c;
    for (int i = 0; i < 66; i++)
    {
        c = cyrillic[i];
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<wchar_t, Character>(c, character));
        // Characters[c] = character;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return;
}

void TextRender::RenderText(Shader& shader, wchar_t* text, GLfloat x, GLfloat y, glm::vec3 color)
{
    int len = wcslen(text);
    projection = glm::mat4(1.0f);
    projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
    // Activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    GLint projLoc = glGetUniformLocation(shader.Program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    GLfloat startX = x;
    // Iterate through all characters
    wchar_t c;

    for (int i = 0; i < len; i++)
    {
        c = *(text + i * sizeof(wchar_t) / 2);
        Character ch = Characters[c];

        GLfloat xpos = x + ch.bearing.x;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y);

        GLfloat w = ch.size.x;
        GLfloat h = ch.size.y;
        if (L'\n' == c) {
            x = startX;
            y -= fontSize + lineSpacing;
        }
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos, ypos, 0.0, 1.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos + w, ypos + h, 1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) ; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

