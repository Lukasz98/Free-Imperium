#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
 public:
    glm::mat4 projection;
    glm::vec3 eye, center, up;
    glm::vec3 look;
    float width = 1920.0f, height = 1080.0f;
    float speed = 1000.0f;
    float baseSpeed = 1000.0f;

    float left = 0.0f, bottom = 0.0f;
    float fov = 45.0f, fovMax = 46.0f, fovMin = 44.0f;
    float near = 0.1f, far = 1001.0f;

    glm::vec3 mouseInWorld;
    glm::vec2 windowSize;
    
public:
    Camera(glm::vec2 winSize)
    {
        windowSize = winSize;
        projection = glm::perspectiveFovRH(fov, width, height, near, far);
        eye = glm::vec3(1077, 537, 969.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        look = glm::vec3(.0f, .0f, -0.1f);
    }
    ~Camera()
    {
    }

    glm::mat4 GetMat()
    {
        return projection * glm::lookAt(eye, eye + look, up);
    }

    void MoveHor(int x, float dt)
    {
        eye.x += speed * x * dt;
    }

    void MoveVer(int y, float dt)
    {
        eye.y += speed * y * dt;
    }

    void Update(double xMouse, double yMouse)
    {
        glm::vec3 win = {xMouse, yMouse, 0.0};
        glm::mat4 model(1.0);
        glm::vec4 viewport = { 0, 0, windowSize.x, windowSize.y };

        glm::vec3 pos =
            glm::unProject(win, model, projection * glm::lookAt(eye, eye + look, up), viewport);
            
        pos.x += (pos.x - eye.x) * 9600.0f;

        pos.y += -(pos.y - eye.y) * 108000.0f / 11.0f;
        mouseInWorld = pos;
    }
    
    void Scroll(int z)
    {
        bool ok = false;
        if (fov >= fovMin + 0.1 && z == -1) {
            fov -= .05f;
            ok = 1;
        }
        if (fov < fovMax && z == 1) {
            fov += .1f;
            ok = 1;
        }
        
        if (ok) {
            speed = baseSpeed * (fov - fovMin + 0.01f);
            projection = glm::perspectiveFovRH(fov, width, height, near, far);
        }
    }

    glm::vec3 GetMouseInWorld() { return mouseInWorld; }
    float GetFov() { return fov; }
};
