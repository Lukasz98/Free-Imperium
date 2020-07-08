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

    void setPlanes();

    glm::vec3 rrrr;

public:
    Camera(glm::vec2 winSize);

   void MoveHor(int x, float dt);

    void MoveVer(int y, float dt);

    void Rotate(int ax, float dt);

    bool IsPointInFrustum(glm::vec3 p);
    float GetFovDelta() { return fov - fovMin; }
    glm::mat4 GetMat();

    void Update(double xMouse, double yMouse);
    
    void Scroll(int z);

    glm::vec3 GetMouseInWorld() { return mouseInWorld; }
    float GetFov() { return fov; }
    glm::vec3 GetEye() { return eye; }
};
