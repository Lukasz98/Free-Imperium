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
    setPlanes();
}

void Camera::SetPos(float x, float y)
{
    eye.x = x;
    eye.y = y;
    setPlanes();
}

void Camera::MoveHor(int x, float dt)
{
    eye.x += speed * x * dt;
    setPlanes();
}

void Camera::MoveVer(int y, float dt)
{
    eye.y += speed * y * dt;
    setPlanes();
}

void Camera::Rotate(int ax, float dt)
{
    look.y += 0.4f * dt * ax;
    setPlanes();
}

void Camera::Reset()
{
    look = look_def;
    setPlanes();
}

void Camera::Scroll(int z)
{
    eye.z += 40.0f * z;
    if (eye.z < minZ)
        eye.z = minZ;
    else if (eye.z > maxZ)
        eye.z = maxZ;
    // Log(eye.z);
    setPlanes();
    return;

    bool ok = false;
    if (fov >= fovMin + 0.1 && z == -1) {
        fov -= .05f;
        ok = 1;
    }
    if (fov < fovMax && z == 1) {
        fov += .1f;
        ok = 1;
    }

    if (fov < fovMin)
        fov = fovMin;
    else if (fov > fovMax)
        fov = fovMax;

    if (ok) {
        // speed = baseSpeed * (fov - fovMin + 0.01f);
        projection = glm::perspectiveFovRH(fov, width, height, near, far);
    }
    setPlanes();
}

void Camera::setPlanes()
{
    glm::mat4 m = projection * glm::lookAt(eye, eye + look, up);
    planes[0].x = m[0][3] + m[0][0];
    planes[0].y = m[1][3] + m[1][0];
    planes[0].z = m[2][3] + m[2][0];
    planes[0].w = m[3][3] + m[3][0];
    // Right clipping plane
    planes[1].x = m[0][3] - m[0][0];
    planes[1].y = m[1][3] - m[1][0];
    planes[1].z = m[2][3] - m[2][0];
    planes[1].w = m[3][3] - m[3][0];
    // Top clipping plane
    planes[2].x = m[0][3] - m[0][1];
    planes[2].y = m[1][3] - m[1][1];
    planes[2].z = m[2][3] - m[2][1];
    planes[2].w = m[3][3] - m[3][1];
    // Bottom clipping plane
    planes[3].x = m[0][3] + m[0][1];
    planes[3].y = m[1][3] + m[1][1];
    planes[3].z = m[2][3] + m[2][1];
    planes[3].w = m[3][3] + m[3][1];
    // Near clipping plane
    planes[4].x = m[0][2];  // + m[2][0];
    planes[4].y = m[1][2];  // + m[2][1];
    planes[4].z = m[2][2];  // + m[2][2];
    planes[4].w = m[3][2];  // + m[2][3];
    // Far clipping plane
    planes[5].x = m[0][3] - m[0][2];
    planes[5].y = m[1][3] - m[1][2];
    planes[5].z = m[2][3] - m[2][2];
    planes[5].w = m[3][3] - m[3][2];
}

bool Camera::IsPointInFrustum(glm::vec3 p)
{
    for (int i = 0; i < 6; i++) {
        float d = planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w;
        if (d < 0)
            return false;
    }
    return true;
}

glm::mat4 Camera::GetMat()
{
    return projection * glm::lookAt(eye, eye + look, up);
}
