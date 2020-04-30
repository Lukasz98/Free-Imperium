#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

public:
    Camera(glm::vec2 winSize);

   void MoveHor(int x, float dt);

    void MoveVer(int y, float dt);

    void Rotate(int ax, float dt);

    bool IsPointInFrustum(glm::vec3 p);

    glm::mat4 GetMat()
    {
        return projection * glm::lookAt(eye, eye + look, up);
    }

    void Update(double xMouse, double yMouse);
    
    void Scroll(int z);

    glm::vec3 GetMouseInWorld() { return mouseInWorld; }
    float GetFov() { return fov; }
};
