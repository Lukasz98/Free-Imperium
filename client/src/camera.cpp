#include "camera.h"

    Camera::Camera(glm::vec2 winSize)
    {
        width = 1920.0f, height = 1080.0f;
        speed = 1000.0f;
        baseSpeed = 1000.0f;
        left = 0.0f, bottom = 0.0f;
        fov = 45.0f, fovMax = 46.0f, fovMin = 44.5f;

       near = 5.1, far = 1000.1;
       near = 5.1, far = 500.1;
       
        windowSize = winSize;
        projection = glm::perspectiveFovRH(fov, width, height, near, far);
        //eye = glm::vec3(1077, 537, 169.0f);
        eye = glm::vec3{0.0, 537, 169};
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        look = glm::vec3(.0f, .0f, -0.1f);
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
        up.y = look.y;
        setPlanes();
        //look.x += speed *dt*ax;
    }

    void Camera::Update(double xMouse, double yMouse)
    {
        glm::vec3 win = {xMouse, yMouse, 0.0};
        glm::mat4 model(1.0);
        glm::vec4 viewport = { 0, 0, windowSize.x, windowSize.y };


//glm::mat4 view{glm::lookAt(eye + glm::vec3{0.0,-look.y,0.0}, eye + look, up)};
glm::mat4 view{glm::lookAt(eye, eye + look, up)};
//glm::mat4 view{glm::lookAt(eye, eye + glm::vec3{0.0,0.0,-0.1}, up)};
float x = (2.0f * xMouse) / windowSize.x - 1.0f;
//float y = 1.0f - (2.0f * yMouse) / windowSize.y;
float y = (2.0f * yMouse) / windowSize.y - 1.0f;

//float x = (2.0f * xMouse) / width - 1.0f;
//float y = 1.0f - (2.0f * yMouse) / height;
float z = 1.0f;
glm::vec3 ray_nds(x, y, z);
glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.0, 1.0);
//glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.0, 1.0);
glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
glm::vec3 ray_wor = (glm::inverse(view) * ray_eye);//.xyz;
// don't forget to normalise the vector at some point
Log("AAA"<<ray_wor.x << " " << ray_wor.y <<" "<<ray_wor.z);
//ray_wor.w = 1.0f; ray_wor = glm::normalize(ray_wor);
//ray_wor = 1.0 / ray_wor;
//Log("NNN"<<ray_wor.x << " " << ray_wor.y <<" "<<ray_wor.z);


float mapZ = 0.0f;
auto cPos = eye;// + look;
eye.y -= look.y;
float distance = cPos.z - mapZ;
auto scaledRay = distance * ray_wor;// * distance;
auto pos = cPos + scaledRay;
Log("BBB: "<<pos.x << " " << pos.y <<" "<<pos.z);

        //glm::vec3 pos = glm::unProject(win, model, projection * glm::lookAt(eye, eye + look, up), viewport);
        //glm::vec3 pos = glm::unProject(win, model, projection * glm::lookAt(eye, eye + look, up), viewport);
        ///pos.x += (pos.x - eye.x) * 9600.0f;

        //pos.y += -(pos.y - eye.y) * 108000.0f / 11.0f;
        
        //pos.x += (pos.x - eye.x);
        //pos.y += -(pos.y - eye.y);
        //mouseInWorld = pos;
        mouseInWorld = ray_wor;
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

        if (fov < fovMin)
            fov = fovMin;
        else if (fov > fovMax)
            fov = fovMax;
        
        if (ok) {
            speed = baseSpeed * (fov - fovMin + 0.01f);
            projection = glm::perspectiveFovRH(fov, width, height, near, far);
        }
        setPlanes();
    }

void Camera::setPlanes()
{
glm::mat4 m = projection * glm::lookAt(eye, eye + look, up);
/*
 planes[0].x = m[3][0] + m[0][0];
 planes[0].y = m[3][1] + m[0][1];
 planes[0].z = m[3][2] + m[0][2];
 planes[0].w = m[3][3] + m[0][3];
 // Right clipping plane
 planes[1].x = m[3][0] - m[0][0];
 planes[1].y = m[3][1] - m[0][1];
 planes[1].z = m[3][2] - m[0][2];
 planes[1].w = m[3][3] - m[0][3];
 // Top clipping plane
 planes[2].x = m[3][0] - m[1][0];
 planes[2].y = m[3][1] - m[1][1];
 planes[2].z = m[3][2] - m[1][2];
 planes[2].w = m[3][3] - m[1][3];
 // Bottom clipping plane
 planes[3].x = m[3][0] + m[1][0];
 planes[3].y = m[3][1] + m[1][1];
 planes[3].z = m[3][2] + m[1][2];
 planes[3].w = m[3][3] + m[1][3];
 // Near clipping plane
 planes[4].x = m[3][0] + m[2][0];
 planes[4].y = m[3][1] + m[2][1];
 planes[4].z = m[3][2] + m[2][2];
 planes[4].w = m[3][3] + m[2][3];
 // Far clipping plane
 planes[5].x = m[3][0] - m[2][0];
 planes[5].y = m[3][1] - m[2][1];
 planes[5].z = m[3][2] - m[2][2];
 planes[5].w = m[3][3] - m[2][3];
*/

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
 planes[4].x = m[0][2];// + m[2][0];
 planes[4].y = m[1][2];// + m[2][1];
 planes[4].z = m[2][2];// + m[2][2];
 planes[4].w = m[3][2];// + m[2][3];
 // Far clipping plane
 planes[5].x = m[0][3] - m[0][2];
 planes[5].y = m[1][3] - m[1][2];
 planes[5].z = m[2][3] - m[2][2];
 planes[5].w = m[3][3] - m[3][2];


 //normalize
 //for (int i = 0; i < 6; i++)
 //planes[i] = 1.0f / planes[i];
}

bool Camera::IsPointInFrustum(glm::vec3 p)
{
    for(int i = 0; i < 6; i++) {
      float d = planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w;
      if (d < 0)
        return false;
      //if(planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w <= 0)
      //   return false;
    }
    return true;

/*
     d = plane.a*pt.x + plane.b*pt.y + plane.c*pt.z + plane.d;
 if (d < 0) return NEGATIVE;
 if (d > 0) return POSITIVE;
 return ON_PLANE;
*/
}