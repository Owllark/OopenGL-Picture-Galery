#define _CRT_SECURE_NO_WARNINGS
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image_resize.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Camera.h"
#include "Button.h"
#include "TextRender.h"
#include "Slider.h"
 
#include <iostream>
#include <fstream>
#include <string> 
#include <vector>

//setup converter
//using convert_type = std::codecvt_utf8<wchar_t>;
//std::wstring_convert<convert_type, wchar_t> converter;

glm::vec3 defaultLightAmbient = glm::vec3(0.8f, 0.8f, 0.8f);
glm::vec3 defaultLightDiffuse = glm::vec3(0.6f, 0.6f, 0.6f);
glm::vec3 defaultLightSpecular = glm::vec3(0.2f,0.2f, 0.2f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(0.0f, 0.9f, -6.0f);

const int WIDTH = 1920;
const int HEIGHT = 1080;
const GLfloat galeryWidth = 5.0;
const GLfloat leftBorder = -(galeryWidth / 2);
const GLfloat rightBorder = +(galeryWidth / 2);
const GLfloat indent = 0.35f;
const GLfloat SECTION_WIDTH = 3.84;
const GLfloat SECTION_HEIGHT = 2.16;
const GLfloat UNDER_SECTION_HEIGHT = 0.6;
const GLfloat suffitHeight = SECTION_HEIGHT + SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT / 2;
const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float buttonIndent = 100;
const float collectionButtonIndent = 300;
const float borderButtonIndent = 50;
const float choiceBtnWidth = 250;
const float choiceBtnHeight = 150;
const float lightHeight = suffitHeight - 0.1;
const float startBtnY = (HEIGHT / 5) / 2 - 30;
const float pictureBtnY = (HEIGHT / 5) + (HEIGHT / 5) - 50;
const float collectionBtnY = ((HEIGHT / 5) * 3) + (HEIGHT / 5) - 50;

const int max_light_level = 8;
const float linear[max_light_level] = {0.8, 0.7, 0.35, 0.22, 0.14, 0.09, 0.07, 0.045};
const float quadratic[max_light_level] = {2, 1.8, 0.44, 0.2, 0.07, 0.032, 0.017, 0.0075};

int lightLevel = 4;
const int START_BTN_NUM = 1;
const int COLLECTION_BTNS_FIRST = 128;
const int PICTURE_BTNS_FIRST = 256;
const int COLLECTIONS_SLIDER = 2;
const int PICTURES_SLIDER = 3;


GLfloat backBorder = SECTION_WIDTH / 2;
GLfloat frontBorder = 0;
glm::vec4 leftPlane = glm::vec4(1.0, 0, 0, -leftBorder);
glm::vec4 rightPlane = glm::vec4(1.0, 0, 0, -rightBorder);

glm::vec3 cameraPos = glm::vec3(0.0f, 1.8f, 0.0f);

bool isKeyPressed[1024];
bool firstMouseMove = true;
bool loading = false;

GLfloat deltaTime, lastFrame = 0;

double lastX = WIDTH / 2, lastY = HEIGHT / 2;

Camera GameCamera(cameraPos, worldUp);



unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

bool inMenu = true;

glm::vec3 intersectionPoint(glm::vec3 vec, glm::vec3 point, glm::vec4 plane) {
	GLfloat xA = vec.x, xB =-point.x;
	GLfloat yA = vec.y, yB = point.y;
	GLfloat zA = vec.z, zB = point.z;
	GLfloat t = -(plane.x * xB + plane.y * yB + plane.z * zB + plane.w) / (plane.x * xA + plane.y * yA + plane.z * zA);
	glm::vec3 result = glm::vec3(0.0);
	result.x = xA * t + xB;
	result.y = yA * t + yB;
	result.z = zA * t + zB;
	return result;
}

bool pointInRect(glm::vec3 point, glm::vec3 rDown, glm::vec3 vec) {

	bool res = true;

	if (point.x < rDown.x - 0.001 || point.x > (rDown.x + vec.x) + 0.001) res = false;
	if (point.y < rDown.y - 0.001 || point.y > (rDown.y + vec.y) + 0.001) res = false;
	if (point.z < rDown.z - 0.001 || point.z > (rDown.z + vec.z) + 0.001) res = false;

	return res;
}

wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	//mbstowcs(wc, c, cSize);
	for (int i = 0; i < cSize; i++) {
		if(c[i] >= 0 && c[i] <= 128) wc[i] = c[i];
		else wc[i] = 1040 + (c[i] + 64);
	}

	return wc;
}
void toWideString(wchar_t* res, wchar_t* str, int len) {
	int i;
	for (i = 0; i < len; i++) {
		res[i] = str[i];
	}
	res[i] = 0;
}
void strToChar(char* c, std::string str) {
	int i;
	for (i = 0; i < str.length(); i++) {
		c[i] = str[i];
	}
	c[i] = 0;
}
void wstrToWchar(wchar_t* c, std::wstring str) {
	int i;
	for (i = 0; i < str.length(); i++) {
		c[i] = str[i];
	}
	c[i] = 0;
}
int wstrToInt(std::wstring str) {
	int res = 0;
	for (int i = 0; i < str.length(); i++) {
		res = res * 10 + str[i] - '0';
	}
	return res;
}
int strToInt(std::string str) {
	int res = 0;
	for (int i = 0; i < str.length(); i++) {
		res = res * 10 + str[i] - '0';
	}
	return res;
}
wchar_t* charToWchar(char* str, int len) {
	wchar_t* res = new wchar_t[len];
	for (int i = 0; i < len; i++) {
		res[i] = str[i];
	}
	return res;
}
std::string intToStr(int n) {
	std::string res = "";
	if (0 == n) res = "0";
	while (n > 0) {
		res += '0' + n % 10;
		n /= 10;
	}
	return res;
}

typedef struct pictureInf {
	wchar_t picName[255];
	char path[255];
	wchar_t description[2048];
	float width;
	float height;
};

typedef struct Picture {
	struct pictureInf* inf;
	GLint texture;
};

void justVoid(int num) {

}


std::vector <Button> pictureBtns;
std::vector <Button> collectionBtns;

struct pictureInf** collections = NULL;
struct pictureInf* allPictures = NULL;
bool* isPictureChosen = NULL;
int* dirLen = NULL;
int dirAmount = 0;
int allPicsAmount = 0;
int chosenPicsAmount = 0;
int menuChoice = 0;
bool isDecriptionVisible = false;

bool checkActive() {
	for (int i = 0; i < allPicsAmount; i++) {
		if (pictureBtns[i].IsPressed()) return true;
	}
	return false;
}

void activatePictureBtns(int num) {
	int idx = 0;
	int i = 0;
	for (i = 0; i < num - COLLECTION_BTNS_FIRST; i++) {
		idx += dirLen[i];
	}
	for (int j = idx; j < idx + dirLen[i]; j++) {
		if (collectionBtns[i].IsPressed()) pictureBtns[j].SetPressed();
		else pictureBtns[j].SetUnpressed();
	}
}

void FileReading() {
	std::ifstream in;
	std::ifstream infile;
	in.open("Collections/directory_list.txt");
	std::string s;
	std::getline(in, s);
	dirAmount = strToInt(s);

	std::string dirName;
	collections = (struct pictureInf**)malloc(sizeof(struct pictureInf*) * dirAmount);
	dirLen = (int*)malloc(sizeof(int) * dirAmount);

	for (int i = 0; i < dirAmount; i++) {
		in >> dirName;
		infile.open("Collections/" + dirName + "/" + "collection.txt");

		std::string collectionName;
		std::getline(infile, collectionName);
		char simpleString[255];
		strToChar(simpleString, collectionName);

		Button* btn = new Button(borderButtonIndent + choiceBtnWidth / 2 + i * (collectionButtonIndent + choiceBtnWidth), collectionBtnY,
									choiceBtnWidth, choiceBtnHeight, glm::vec3(0.2, 0.1, 0.4), glm::vec3(0.4, 0.4, 0.4), GetWC(simpleString), activatePictureBtns, COLLECTION_BTNS_FIRST + i);
		collectionBtns.push_back(*btn);

		std::string;
		std::getline(infile, s);
		int pictureAmount = strToInt(s);
		collections[i] = (struct pictureInf*)malloc(sizeof(struct pictureInf) * pictureAmount);
		dirLen[i] = pictureAmount;
		allPicsAmount += pictureAmount;
		std::string fileName, picName, description, w, h;
		for (int j = 0; j < pictureAmount; j++) {
			description = "";
			std::getline(infile, fileName);
			std::getline(infile, picName);
			std::getline(infile, h);
			std::getline(infile, w);
			description += picName + "\n";
			description += h + "*" + w + "\n";
			std::string line;
			do {
				std::getline(infile, line);
				description += line;
				description += "\n";
			} while (line != "");
			struct pictureInf* p = (struct pictureInf*)malloc(sizeof(struct pictureInf));

			p->width = 0.01 * strToInt(w);
			p->height = 0.01 * strToInt(h);

			char simpleString[2048];
			strToChar(simpleString, picName);
			toWideString(p->picName, GetWC(simpleString), picName.length());

			strToChar(p->path, "Collections/" + dirName + "/" + fileName);

			strToChar(simpleString, description);
			toWideString(p->description, GetWC(simpleString), description.length());

			collections[i][j] = *p;
		}
		infile.close();
	}

	allPictures = (struct pictureInf*)malloc(sizeof(struct pictureInf) * allPicsAmount);
	isPictureChosen = (bool*)malloc(sizeof(bool) * allPicsAmount);
	int idx = 0;
	for (int i = 0; i < dirAmount; i++) {
		for (int j = 0; j < dirLen[i]; j++) {
			allPictures[idx] = collections[i][j];
			isPictureChosen[idx] = false;
			Button* btn = new Button(borderButtonIndent + choiceBtnWidth / 2 + idx * (buttonIndent + choiceBtnWidth), pictureBtnY,
									 choiceBtnWidth, choiceBtnHeight, glm::vec3(0.2, 0.1, 0.4), glm::vec3(0.4, 0.4, 0.4), collections[i][j].picName, justVoid, PICTURE_BTNS_FIRST + idx);
			pictureBtns.push_back(*btn);
			idx++;
		}
	}
	return;
}

struct Picture* chosenPics = NULL;
void makeList() {
	chosenPicsAmount = 0;
	chosenPics = (struct Picture*)malloc(sizeof(struct Picture) * 0);
	for (int i = 0; i < allPicsAmount; i++) {
		if (pictureBtns[i].IsPressed()) {
			chosenPicsAmount++;
			chosenPics = (struct Picture*)realloc(chosenPics, sizeof(struct Picture) * chosenPicsAmount);
			chosenPics[chosenPicsAmount - 1].inf = &allPictures[i];
			chosenPics[chosenPicsAmount - 1].texture = loadTexture(allPictures[i].path);
		}

	}
}

void start(int num) {
	if (checkActive) inMenu = false;
	loading = true;
}

void GaleryInit() {
	makeList();
	frontBorder = -((chosenPicsAmount + chosenPicsAmount % 2) * SECTION_WIDTH - 1.92);
	GameCamera.Init(cameraPos, worldUp);
}

wchar_t startBtnCapture[] = L"Начать";

Button startBtn(WIDTH / 2, startBtnY, 300.0f, 80.0f, glm::vec3(0.1, 0.5, 0.3), glm::vec3(0.9, 0.3, 0.3), startBtnCapture, start, START_BTN_NUM);

Slider sliderPictures(0 + 150, WIDTH - 150, 200.0f, pictureBtnY - 150, true, glm::vec3(0.1, 0.5, 0.3), glm::vec3(0.12, 0.6, 0.35), glm::vec3(0.3, 0.3, 0.3), 10.0f, 40.0f, startBtnCapture, justVoid, PICTURES_SLIDER);

Slider sliderCollections(0 + 150, WIDTH - 150, 200.0f, collectionBtnY - 150, true, glm::vec3(0.1, 0.5, 0.3), glm::vec3(0.12, 0.6, 0.35), glm::vec3(0.3, 0.3, 0.3), 10.0f, 40.0f, startBtnCapture, justVoid, COLLECTIONS_SLIDER);

bool setCursor = true;
int descriptionNumber = -1;
bool descriptionShow = false;
wchar_t* descriptionText;
void RestartMenu() {
	inMenu = true;
	loading = false;
	setCursor = true;
	startBtn.SetUnpressed();
	descriptionShow = false;
	chosenPicsAmount = 0;
}

TextRender Description;

void doMovement() {

	glm::vec3 lastPos = GameCamera.Position;


	if (isKeyPressed[GLFW_KEY_W]) GameCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);

	if (isKeyPressed[GLFW_KEY_S]) GameCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);

	if (isKeyPressed[GLFW_KEY_A]) GameCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);

	if (isKeyPressed[GLFW_KEY_D]) GameCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

	if ((GameCamera.Position.x >= rightBorder - indent) || (GameCamera.Position.x <= leftBorder + indent)) GameCamera.Position.x = lastPos.x;

	if ((GameCamera.Position.z >= backBorder - indent) || (GameCamera.Position.z <= frontBorder + indent)) GameCamera.Position.z = lastPos.z;

	//curs
	//if (isKeyPressed[GLFW_KEY_SPACE]) GameCamera.ProcessKeyboard(Camera_Movement::UP, deltaTime);

}



void ShowDescription() {
	Shader textShader("Shaders/textVertexShader.vs", "Shaders/textFragmentShader.frag");
	int i = 0;
	int wallNum = 1;
	descriptionShow = false;
	while(i < chosenPicsAmount) {
		glm::vec3 point = intersectionPoint(GameCamera.Front, GameCamera.Position, leftPlane);
		if (pointInRect(point, glm::vec3(leftBorder, 0, (SECTION_WIDTH / 2) - SECTION_WIDTH * (2 * wallNum)), glm::vec3(0, SECTION_HEIGHT, SECTION_WIDTH))
			&& GameCamera.Front.x * point.x > 0) {
			if (descriptionNumber != i) {
				descriptionShow = true;
				descriptionNumber = i;
			}
			else {
				descriptionShow = false;
				descriptionNumber = -1;
			}
			descriptionText = chosenPics[i].inf->description;
		}
		i++;
		point = intersectionPoint(GameCamera.Front, GameCamera.Position, rightPlane);
		if (i < chosenPicsAmount)
		if (pointInRect(point, glm::vec3(rightBorder, 0, (SECTION_WIDTH / 2) - SECTION_WIDTH * (2 * wallNum)), glm::vec3(0, SECTION_HEIGHT, SECTION_WIDTH))
			&& GameCamera.Front.x * point.x > 0) {
			if (descriptionNumber != i) {
				descriptionShow = true;
				descriptionNumber = i;
			}
			else {
				descriptionShow = false;
				descriptionNumber = -1;
			}
			descriptionText = chosenPics[i].inf->description;
			
		}
		i++;
		wallNum++;
	}
	
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (action == GLFW_RELEASE)
	{

		isKeyPressed[key] = false;
		switch (key)
		{
		case (GLFW_KEY_ESCAPE): {
			if (inMenu) glfwSetWindowShouldClose(window, GL_TRUE);
			else RestartMenu();
		}
				break;
			case (GLFW_KEY_SPACE): {
				//if (TEXT) TEXT = false;
				ShowDescription(); 
			}
				break;
			case (GLFW_KEY_UP): {
				if (isKeyPressed[GLFW_KEY_R] && lightColor.x < 1) lightColor += glm::vec3(0.1, 0, 0);
				else if (isKeyPressed[GLFW_KEY_G] && lightColor.y < 1) lightColor += glm::vec3(0, 0.1, 0);
				else if (isKeyPressed[GLFW_KEY_B] && lightColor.z < 1) lightColor += glm::vec3(0, 0, 0.1);
				else if ((!isKeyPressed[GLFW_KEY_R] && !isKeyPressed[GLFW_KEY_G] && !isKeyPressed[GLFW_KEY_B]) && (lightLevel < max_light_level - 1)) lightLevel++;
			}
			break;
			case (GLFW_KEY_DOWN): {
				if (isKeyPressed[GLFW_KEY_R] && lightColor.x > 0) lightColor -= glm::vec3(0.1, 0, 0);
				else if (isKeyPressed[GLFW_KEY_G] && lightColor.y > 0) lightColor -= glm::vec3(0, 0.1, 0);
				else if (isKeyPressed[GLFW_KEY_B] && lightColor.z > 0) lightColor -= glm::vec3(0, 0, 0.1);
				else if ((!isKeyPressed[GLFW_KEY_R] && !isKeyPressed[GLFW_KEY_G] && !isKeyPressed[GLFW_KEY_B]) && (lightLevel > 0)) lightLevel--;
			}
			break;
		}
	}
	else if (action == GLFW_PRESS)
	{

		isKeyPressed[key] = true;
		
		switch (key)
		{

		}

	}
}

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods)
{
	if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double posX, posY;
		//getting cursor position
		glfwGetCursorPos(window, &posX, &posY);
		posY += 21;
		startBtn.CheckClick(posX, HEIGHT - posY);
		for (int i = 0; i < pictureBtns.size(); i++) {
			pictureBtns[i].CheckClick(posX, HEIGHT - posY);
		}
		for (int i = 0; i < collectionBtns.size(); i++) {
			collectionBtns[i].CheckClick(posX, HEIGHT - posY);
		}
		sliderPictures.CheckClick(posX, HEIGHT - posY);
		sliderCollections.CheckClick(posX, HEIGHT - posY);
		isKeyPressed[key] = true;
	}
	if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isKeyPressed[key] = false;
		sliderPictures.Deactivate();
		sliderCollections.Deactivate();
	}
}

void mouse_callback(GLFWwindow* window, double posX, double posY)
{
	if (firstMouseMove)
	{
		lastX = posX;
		lastY = posY;
		firstMouseMove = false;
	}

	GLfloat offsetX = posX - lastX;
	GLfloat offsetY = lastY - posY;

	if (inMenu) {
		if (isKeyPressed[GLFW_MOUSE_BUTTON_LEFT] && sliderPictures.IsActive()) {
			float btnBarLen = 2 * borderButtonIndent + allPicsAmount * choiceBtnWidth + (allPicsAmount - 1) * (buttonIndent);
			sliderPictures.Motion(offsetX, -offsetY);
			float percent = sliderPictures.GetContentPercent();
			//float btnBarLen = (allPicsAmount / 2 + (allPicsAmount % 2)) + 2 * 100 + (allPicsAmount / 2 + (allPicsAmount % 2) - 1) * 500;
			for (int i = 0; i < pictureBtns.size(); i++) {
				float btnPosX = borderButtonIndent + (i) * choiceBtnWidth + (i) * (buttonIndent) + choiceBtnWidth / 2;
				pictureBtns[i].SetPosX(btnPosX - percent * btnBarLen);
			}
		}
		if (isKeyPressed[GLFW_MOUSE_BUTTON_LEFT] && sliderCollections.IsActive()) {
			float btnBarLen = 2 * borderButtonIndent + dirAmount * choiceBtnWidth + (dirAmount - 1) * (collectionButtonIndent);
			sliderCollections.Motion(offsetX, -offsetY);
			float percent = sliderCollections.GetContentPercent();
			//float btnBarLen = (allPicsAmount / 2 + (allPicsAmount % 2)) + 2 * 100 + (allPicsAmount / 2 + (allPicsAmount % 2) - 1) * 500;
			for (int i = 0; i < collectionBtns.size(); i++) {
				float btnPosX = borderButtonIndent + (i)*choiceBtnWidth + (i) * (collectionButtonIndent)+choiceBtnWidth / 2;
				collectionBtns[i].SetPosX(btnPosX - percent * btnBarLen);
			}
		}
	}
	else {
		GameCamera.ProcessMouseMovement(offsetX, offsetY);
	}

	lastX = posX;
	lastY = posY;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	GameCamera.ProcessMouseScroll(-yoffset);
}

void FreeMemory() {
	pictureBtns.clear();
	collectionBtns.clear();
	if(collections != NULL) free(collections);
	if(allPictures != NULL) free(allPictures);
	if(isPictureChosen != NULL) free(isPictureChosen);
}

int main() {
	glfwInit();
	//GLFW setup
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Galery", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glViewport(0, 0, WIDTH, HEIGHT);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CROSSHAIR_CURSOR);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
    double lastTime = glfwGetTime();
	const int fps = 60;
	double delta;

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	float cubeVertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f

	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture coordinates attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE);

	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// Coordinates attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader objectShader("Shaders/VertexShader3D.vs", "Shaders/FragmentShader3D.frag");
	Shader lightShader("Shaders/lightVertexShader.vs", "Shaders/lightFragmentShader.frag");
	//GLint texture1 = loadTexture("Allocade.png");
	//GLint texture2 = loadTexture("ParisStreet.png");
	GLint textureFloor = loadTexture("Collections/stone.jpg");
	GLint textureUnder = loadTexture("Collections/blank2.png");
	GLint textureBlank = loadTexture("Collections/blank.png");

	Shader buttonShader("Shaders/VertexShader2D.vs", "Shaders/FragmentShader2D.frag");
	Shader textShader("Shaders/textVertexShader.vs", "Shaders/textFragmentShader.frag");

	
	FileReading();

	glfwSwapInterval(1);


	float btnBarLen = 2 * borderButtonIndent + allPicsAmount * choiceBtnWidth + (allPicsAmount - 1) * (buttonIndent);
	sliderPictures.SetSliderSize(WIDTH / btnBarLen);
	btnBarLen = 2 * borderButtonIndent + dirAmount * choiceBtnWidth + (dirAmount - 1) * (collectionButtonIndent);
	sliderCollections.SetSliderSize(WIDTH / btnBarLen);

	wchar_t t1[] = L"Собрания картин:";
	wchar_t t2[] = L"Все картины:";
	wchar_t loadingText[] = L"Загрузка галереи..";

	TextRender textCollections, textPictures, textLoading;

	textCollections.Init(48, 24);
	textPictures.Init(48, 24);
	textLoading.Init(40, 20);
	Description.Init(20, 12);

	startBtn.Init();
	sliderPictures.Init();
	sliderCollections.Init();
	for (int i = 0; i < pictureBtns.size(); i++) {
		pictureBtns[i].Init();
	}
	for (int i = 0; i < collectionBtns.size(); i++) {
		collectionBtns[i].Init();
	}
	
    while(!glfwWindowShouldClose(window)){
		
		if (inMenu) {
			if (setCursor) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
				setCursor = false;
			}
			glfwPollEvents();
			glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			

			if (!checkActive()) startBtn.Disable();
			else if (startBtn.disabled) startBtn.Able();

			//Btn.Draw(buttonShader, textShader, WIDTH, HEIGHT);
			startBtn.Draw(buttonShader, textShader, WIDTH, HEIGHT);
			sliderPictures.Draw(buttonShader, WIDTH, HEIGHT);
			sliderCollections.Draw(buttonShader, WIDTH, HEIGHT);
			for (int i = 0; i < pictureBtns.size(); i++) {
				pictureBtns[i].Draw(buttonShader, textShader, WIDTH, HEIGHT);
			}
			for (int i = 0; i < collectionBtns.size(); i++) {
				collectionBtns[i].Draw(buttonShader, textShader, WIDTH, HEIGHT);
			}

			textCollections.RenderText(textShader, t1, WIDTH / 2 - 48 * 5, collectionBtnY + 150, glm::vec3(1, 1, 1));
			textPictures.RenderText(textShader, t2, WIDTH / 2 - 48 * 4, pictureBtnY + 150, glm::vec3(1, 1, 1));
			if (loading) textPictures.RenderText(textShader, loadingText, (WIDTH / 4) * 3 - 100, startBtnY - 20, glm::vec3(1, 1, 1));
			if (!inMenu) {
				glfwSwapBuffers(window);
				GaleryInit();
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
		else {
			glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			//glfwGetCursorPos(window, &lastX, &lastY);
			glfwPollEvents();
			doMovement();

			GLint modelLoc;
			GLint viewLoc;
			GLint projLoc;
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			glBindVertexArray(VAO);
			objectShader.Use();

			objectShader.setVec3("viewPos", GameCamera.Position.x, GameCamera.Position.y, GameCamera.Position.z);
			//point light properties
			int pointLightsAmount = (chosenPicsAmount / 2) + (chosenPicsAmount % 2);
			objectShader.setInt("pointLightsAmount", pointLightsAmount);
			for (int i = 0; i < pointLightsAmount; i++) {
				std::string idx = intToStr(i);
				objectShader.setVec3("pointLight[" + idx + "].position", glm::vec3(0.0f, lightHeight, -(i * 2 * SECTION_WIDTH + SECTION_WIDTH)));
				objectShader.setVec3("pointLight[" + idx + "].ambient", defaultLightAmbient);
				objectShader.setVec3("pointLight[" + idx + "].diffuse", defaultLightDiffuse);
				objectShader.setVec3("pointLight[" + idx + "].specular", defaultLightSpecular);
				objectShader.setFloat("pointLight[" + idx + "].constant", 1.0f);
				objectShader.setFloat("pointLight[" + idx + "].lihear", linear[lightLevel]);
				objectShader.setFloat("pointLight[" + idx + "].quadratic", quadratic[lightLevel]);
			}

			//material properties
			objectShader.setFloat("material.shininess", 64.0f);
			objectShader.setVec3("material.ambient", defaultLightAmbient);
			objectShader.setVec3("material.diffuse", defaultLightDiffuse);
			objectShader.setVec3("material.specular", defaultLightSpecular);
			objectShader.setVec3("lightColor", lightColor);

			objectShader.setInt("maintexture", 0);

			modelLoc = glGetUniformLocation(objectShader.Program, "model");
			viewLoc = glGetUniformLocation(objectShader.Program, "view");
			projLoc = glGetUniformLocation(objectShader.Program, "projection");

			int picsCounter = 0;

			int wallAmount = chosenPicsAmount + chosenPicsAmount % 2;

			for (int i = 0; i < wallAmount; i++) {

				objectShader.setVec3("pointLights[" + intToStr(i) + "].position", glm::vec3(0.0f, lightHeight, 2 * SECTION_WIDTH));

				view = glm::mat4(1.0f);
				projection = glm::mat4(1.0f);
				view = GameCamera.GetViewMatrix();
				projection = glm::perspective(glm::radians(GameCamera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

				if (i % 2 == 1) {
					glActiveTexture(GL_TEXTURE1);
					if (picsCounter < chosenPicsAmount) {
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, chosenPics[picsCounter].texture);
						objectShader.setInt("maintexture", 1);
						model = glm::mat4(1.0f);
						model = glm::translate(model, glm::vec3(leftBorder - 0.47, SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(i)*SECTION_WIDTH));
						model = glm::scale(model, glm::vec3(1.0f, chosenPics[picsCounter].inf->height, chosenPics[picsCounter].inf->width));
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
						//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

						glDrawArrays(GL_TRIANGLES, 0, 36);
						picsCounter++;
					}
				}

				if (i % 2 == 1) {
					glActiveTexture(GL_TEXTURE1);
					if (picsCounter < chosenPicsAmount) {
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, chosenPics[picsCounter].texture);
						objectShader.setInt("maintexture", 1);
						model = glm::mat4(1.0f);
						model = glm::translate(model, glm::vec3(rightBorder + 0.47, SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(i)*SECTION_WIDTH));
						model = glm::scale(model, glm::vec3(1.0f, chosenPics[picsCounter].inf->height, chosenPics[picsCounter].inf->width));
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

						glDrawArrays(GL_TRIANGLES, 0, 36);
						picsCounter++;
					}
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureBlank);
				objectShader.setInt("maintexture", 0);

				// LEFT BLANK WALL
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(leftBorder, SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(i)*SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				
				// Pass them to the shaders

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				
				glDrawArrays(GL_TRIANGLES, 0, 36);
				// RIGHT BLANK WALL
				
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(rightBorder, SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(i)*SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);

			}

			if (true) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureUnder);
				objectShader.setInt("maintexture", 0);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(leftBorder, -SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(wallAmount / 2 + wallAmount % 2) * SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH * (wallAmount + 1)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureUnder);
				objectShader.setInt("maintexture", 0);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(leftBorder, SECTION_HEIGHT + SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(wallAmount / 2 + wallAmount % 2) * SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH * (wallAmount + 1)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureUnder);
				objectShader.setInt("maintexture", 0);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(rightBorder, -SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(wallAmount / 2 + wallAmount % 2) * SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH * (wallAmount + 1)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureUnder);
				objectShader.setInt("maintexture", 0);
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(rightBorder, SECTION_HEIGHT + SECTION_HEIGHT / 2 + UNDER_SECTION_HEIGHT, -(wallAmount / 2 + wallAmount % 2) * SECTION_WIDTH));
				model = glm::scale(model, glm::vec3(0.01f, SECTION_HEIGHT, SECTION_WIDTH * (wallAmount + 1)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			
			/// FLOOR AND SOFFIT

			objectShader.setInt("maintexture", 1);

			int w = galeryWidth + 2, l = (chosenPicsAmount * SECTION_WIDTH);
			for (int i = -w / 2; i < w / 2; i++) {
				for (int j = -2; j < (chosenPicsAmount * SECTION_WIDTH) + 4; j++) {

					model = glm::mat4(1.0f);
					view = glm::mat4(1.0f);
					projection = glm::mat4(1.0f);
					view = GameCamera.GetViewMatrix();
					projection = glm::perspective(glm::radians(GameCamera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
					glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
					glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


					model = glm::translate(model, glm::vec3((i) * 1.0f, 0, -(j) * 1.0f));
					model = glm::scale(model, glm::vec3(1.0f, 0.01f, 1.0f));

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, textureFloor);
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

					glDrawArrays(GL_TRIANGLES, 0, 36);

					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3((i) * 1.0f, suffitHeight, -(j) * 1.0f));
					model = glm::scale(model, glm::vec3(1.0f, 0.01f, 1.0f));

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, textureBlank);
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			// FRONT WALL
			model = glm::mat4(1.0f);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


			model = glm::translate(model, glm::vec3(0.0f, (suffitHeight) / 2, frontBorder));
			model = glm::scale(model, glm::vec3(galeryWidth, suffitHeight, 0.1f));

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureBlank);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);

			// BACK WALL
			model = glm::mat4(1.0f);

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


			model = glm::translate(model, glm::vec3(0.0f, (suffitHeight) / 2, backBorder));
			model = glm::scale(model, glm::vec3(galeryWidth, suffitHeight, 0.1f));

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureBlank);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);

			for (int i = 0; i < pointLightsAmount; i++) {
				glBindVertexArray(lightVAO);
				lightShader.Use();
				lightShader.setVec3("lightColor", lightColor);
				// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
				modelLoc = glGetUniformLocation(lightShader.Program, "model");
				viewLoc = glGetUniformLocation(lightShader.Program, "view");
				projLoc = glGetUniformLocation(lightShader.Program, "projection");
				// Set matrices
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0, lightHeight, -(i * 2 * SECTION_WIDTH + SECTION_WIDTH)));
				model = glm::scale(model, glm::vec3(0.2f, 0.02f, 0.8f)); // Make it a smaller cube
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				// Draw the light object
				glBindVertexArray(lightVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}

			if (descriptionShow) Description.RenderText(textShader, descriptionText, 20, 1000, glm::vec3(0.9, 0.9, 0.9));
		}
		
		glfwSwapBuffers(window);
    }

	FreeMemory();
	return 0;
    
}