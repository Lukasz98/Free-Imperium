#include "camera.h"

Camera::Camera(glm::vec2 winSize) : look_def(glm::vec3(.0f, .0f, -0.1f))
{
    width = 1920, height = 1080.0f;
    speed = 250.0f;
    speed = 850.0f;
    baseSpeed = 1000.0f;
    left = 0.0f, bottom = 0.0f;
    fov = 45.0f, fovMax = 46.0f, fovMin = 44.5f;

    near = 5.1f, far = 1000.1f;
    far = 1800.0f;
    near = 65.1f;
    far = 4800.0f;
    windowSize = winSize;
    projection = glm::perspectiveFovRH(fov, width, height, near, far);
    int mapWidth = 5632, mapHeight = 2048;

    eye = glm::vec3(mapWidth * 1.0 * 2.0, mapHeight * 1.4f * 2.0f, 169.0f + 1550.0f);
    maxZ = 1800.0f;
    maxZ = 2800.0f;
    maxZ = far;
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    look = look_def;
}

void Camera::SetPos(float x, float y)
{
    eye.x = x;
    eye.y = y;
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
}

void Camera::Reset()
{
    look = look_def;
}

void Camera::Scroll(int z)
{
    eye.z += 40.0f * z;
    if (eye.z < minZ)
        eye.z = minZ;
    else if (eye.z > maxZ)
        eye.z = maxZ;
    return;
}

glm::mat4 Camera::GetMat()
{
    return projection * glm::lookAt(eye, eye + look, up);
}
