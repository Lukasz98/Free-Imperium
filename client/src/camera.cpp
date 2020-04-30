#include "camera.h"

    Camera::Camera(glm::vec2 winSize)
    {
        width = 1920.0f, height = 1080.0f;
        speed = 1000.0f;
        baseSpeed = 1000.0f;
        left = 0.0f, bottom = 0.0f;
        fov = 45.0f, fovMax = 46.0f, fovMin = 44.0f;
        near = 5.1f, far = 1001.0f;

        windowSize = winSize;
        projection = glm::perspectiveFovRH(fov, width, height, near, far);
        eye = glm::vec3(1077, 537, 169.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        look = glm::vec3(.0f, .0f, -0.1f);
    }
    
    void Camera::MoveHor(int x, float dt)
    {
        eye.x += speed * x * dt;
    }

    void Camera::MoveVer(int y, float dt)
    {
        eye.y += speed * y * dt;
    }

    void Camera::Rotate(int ax, float dt)
    {
        look.y += 0.4f * dt * ax;
        //look.x += speed *dt*ax;
    }

    void Camera::Update(double xMouse, double yMouse)
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
    
    void Camera::Scroll(int z)
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

