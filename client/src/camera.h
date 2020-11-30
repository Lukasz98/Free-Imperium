#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "log.h"

class Camera
{
public:
    glm::vec4 planes[6];
    glm::mat4 projection;
    glm::vec3 eye, center, up;
    glm::vec3 look;
    float width, height;
    float speed;
    float baseSpeed;

    float left, bottom;
    float fov, fovMax, fovMin;
    float near, far;

    glm::vec3 mouseInWorld;
    glm::vec2 windowSize;
    glm::vec3 mouseRay;

    void setPlanes();

public:
    Camera(glm::vec2 winSize);

    void MoveHor(int x, float dt);
    void MoveVer(int y, float dt);

    void Rotate(int ax, float dt);

    glm::mat4 GetMat();

    void Update(double xMouse, double yMouse, const unsigned char * terrain);
    
    void Scroll(int z);

    bool IsPointInFrustum(glm::vec3 p);

    glm::vec3 GetMouseRay() { return mouseRay; }
    glm::vec3 GetMouseInWorld() { return mouseInWorld; }
    float GetFov() { return fov; }
    glm::vec3 GetEye() { return eye; }
    float GetFovDelta() { return fov - fovMin; }
};
