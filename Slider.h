#pragma once
#include "Button.h"
enum orientation {
	horizontal,
	vertical
};
class Slider: public Button{
public:
	Slider(float lowBorder, float highBorder, float startPosX, float startPosY, bool isHorizontal, glm::vec3 unactiveColor, glm::vec3 activeColor, glm::vec3 lineColor,
		   float width, float height, wchar_t* text, void(*f)(int), int num) :Button(startPosX, startPosY, width, height, unactiveColor, activeColor, text, *f, num) {
		this->lineColor = lineColor;
		Deactivate();
		if (isHorizontal) {
			orient = horizontal;
			this->lowBorder = lowBorder;
			this->highBorder = highBorder;
			this->posX = lowBorder + width / 2;
			this->posY = startPosY;
		}
		else {
			orient = vertical;
			this->lowBorder = lowBorder;
			this->highBorder = highBorder;
			this->posX = startPosX;
			this->posY = lowBorder - height / 2;
		}
	};
	void Motion(float deltaX, float deltaY);
	void CheckClick(float x, float y);
	void Draw(Shader& btnShader, float screenWidth, float screenHeight);
	void Activate();
	void Deactivate();
	bool IsActive();
	float GetContentPercent();
	float GetValuePercent();
	void SetSliderSize(float percent);

private:
	bool isActive;
	float lowBorder, highBorder;
	orientation orient;
	glm::vec3 sliderColor;
	glm::vec3 lineColor;
};

