#pragma once
#include <GL/glew.h>;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP
};

//default settings 
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat MOUSE_SENSITIVITY = 0.03f;
const GLfloat SCROLL_SENSITIVITY = 1.20f;
const GLfloat ZOOM = 45.0f;

const GLfloat MIN_ZOOM = 5.0f;
const GLfloat MAX_ZOOM = 55.0f;
const GLfloat MIN_PITCH = -90.0f;
const GLfloat MAX_PITCH = 90.0f;


class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 WorldUp;
    glm::vec3 Right;
    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat MouseSensitivity;
    GLfloat ScrollSensitivity;
    GLfloat Zoom;
    GLfloat Speed;
    Camera(glm::vec3 position, glm::vec3 worldUp) {
        this->Position = position;
        this->WorldUp = worldUp;
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Yaw = YAW;
        Pitch = PITCH;
        Speed = SPEED;
        Zoom = ZOOM;
        Up = worldUp;
        MouseSensitivity = MOUSE_SENSITIVITY;
        ScrollSensitivity = SCROLL_SENSITIVITY;
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = front;
    }

    void Init(glm::vec3 position, glm::vec3 worldUp) {
        this->Position = position;
        this->WorldUp = worldUp;
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Yaw = YAW;
        Pitch = PITCH;
        Speed = SPEED;
        Zoom = ZOOM;
        Up = worldUp;
        MouseSensitivity = MOUSE_SENSITIVITY;
        ScrollSensitivity = SCROLL_SENSITIVITY;
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = front;
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, WorldUp);
    }

    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
        switch (direction){
        case FORWARD:
            Position += glm::vec3(Front.x, 0.0f, Front.z) * Speed * deltaTime;
            break;
        case BACKWARD:
            Position -= glm::vec3(Front.x, 0.0f, Front.z) * Speed * deltaTime;
            break;
        case LEFT:
            Position -= Right * Speed * deltaTime;
            break;
        case RIGHT:
            Position += Right * Speed * deltaTime;
            break;
        case UP:
            Position += WorldUp * Speed * deltaTime;
            break;
        }
        this->UpdateCameraVectors();
    }

    void ProcessMouseMovement(GLfloat offsetX, GLfloat offsetY) {
        this->Yaw += offsetX * MouseSensitivity;
        this->Pitch += offsetY * MouseSensitivity;
        if (this->Pitch > MAX_PITCH)
            this->Pitch = MAX_PITCH;
        if (this->Pitch < MIN_PITCH)
            this->Pitch = MIN_PITCH;
        this->UpdateCameraVectors();

    }

    void ProcessMouseScroll(GLfloat offsetY) {
        this->Zoom += offsetY * ScrollSensitivity;
        if (this->Zoom < MIN_ZOOM)
            this->Zoom = MIN_ZOOM;
        if (this->Zoom > MAX_ZOOM)
            this->Zoom = MAX_ZOOM;
    }
private:
    void UpdateCameraVectors(){
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }


};

