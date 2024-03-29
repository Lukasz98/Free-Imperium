#include "model_3d.h"
#include <math.h>
#define PI 3.14159265
#define meshId 13

Model3D::Model3D(const std::string& path, glm::vec3 pos) : position{pos}
{
    bool loadout = Loader.LoadFile(path);

    if (!loadout) {
        Log("Nie moge zaladowac modelu: " << path);
        return;
    }

    Log("Werticow= " << Loader.LoadedVertices.size() << ", " << Loader.LoadedMaterials.size());

    std::vector<glm::vec3> verts;
    std::vector<float> rawVerts;

    int minX = 1111, minY = 1111, maxX = -1111, maxY = -1111;

    for (auto v : Loader.LoadedVertices) {
        if (v.Position.X < minX)
            minX = v.Position.X;
        if (v.Position.X > maxX)
            maxX = v.Position.X;
        if (v.Position.Y < minY)
            minY = v.Position.Y;
        if (v.Position.Y > maxY)
            maxY = v.Position.Y;

        verts.push_back(glm::vec3{v.Position.X, v.Position.Y, v.Position.Z});
        rawVerts.push_back(v.Position.X);
        rawVerts.push_back(v.Position.Y);
        rawVerts.push_back(v.Position.Z);
        rawVerts.push_back(v.Normal.X);
        rawVerts.push_back(v.Normal.Y);
        rawVerts.push_back(v.Normal.Z);
        rawVerts.push_back(v.TextureCoordinate.X);
        rawVerts.push_back(v.TextureCoordinate.Y);
    }

    plane = std::make_unique<Rectangle>(glm::vec3{minX, minY, 0}, glm::vec2{maxX - minX, maxY - minY},
                                        glm::vec4{1.0, 0.0, 0.0, 0.5});

    {
        float h = maxY - minY, w = maxX - minX;
        glm::vec3 rectVerts[] = {
            {minX, minY, 0},
            {minX, minY + h, 0},
            {minX + w, minY + h, 0},
            {minX + w, minY + h, 0},
            {minX + w, minY, 0},
            {minX, minY, 0},
        };
        //GLuint rectVbo;
        glCreateVertexArrays(1, &rectVao);
        glBindVertexArray(rectVao);
        glCreateBuffers(1, &rectVbo);

        glBindBuffer(GL_ARRAY_BUFFER, rectVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 6, rectVerts, GL_STATIC_DRAW);
        glEnableVertexArrayAttrib(rectVao, 0);
        GLuint err = glGetError();
        if (err)
            Log("Opengl error: " << err);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);  //(const GLvoid*)0);
    }

    glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    bind();
    glBufferData(GL_ARRAY_BUFFER, rawVerts.size() * sizeof(float), rawVerts.data(), GL_STATIC_DRAW);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, Loader.LoadedIndices.size() * sizeof(unsigned int),
        Loader.LoadedIndices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));

    iboCount = Loader.LoadedIndices.size();

    unbind();
}

Model3D::~Model3D()
{
}

void Model3D::Draw()
{
    bind();
    glDrawElements(GL_TRIANGLES, Loader.LoadedIndices.size(), GL_UNSIGNED_INT, nullptr);
    unbind();
}

void Model3D::DrawRect()
{
    plane->Draw();
}

void Model3D::bind()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void Model3D::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool Model3D::Click(const glm::mat4& ml, const glm::mat4& mlRot, const glm::vec3& vv, const glm::vec3& eye,
                    float yScale, float rotateAngle)
{
    glm::vec3 normal{0.0, 0.0, -1.0};
    normal = mlRot * glm::vec4{normal, 1.0};

    double dn = glm::dot(vv, normal);
    if (dn == 0.0)
        return false;

    glm::vec3 right{0.0, 0.0, 0.0};
    right = ml * glm::vec4{right, 1.0};

    glm::vec2 realSize = glm::vec4{plane->GetSize(), 1.0, 1.0};
    glm::vec3 left{-realSize.x, 0.0, 0.0};
    left = ml * glm::vec4{left, 1.0};

    double tt = glm::dot(right - eye, normal) / dn;

    glm::vec3 qq{vv.x * tt, vv.y * tt, vv.z * tt};
    glm::vec3 h{eye + qq};

    glm::vec3 bottom{0.0, 0.0, 0.0};
    bottom = ml * glm::vec4{bottom, 1.0};

    glm::vec3 top{0.0, realSize.y, 0.0};

    double model_h = realSize.y * yScale;

    double z = sin(rotateAngle) * model_h;
    double y = cos(rotateAngle) * z;
    top = bottom;
    top.z += z;
    top.y += y;

    if (h.x > left.x && h.x < right.x && h.y > bottom.y && h.y < top.y && h.z >= bottom.z && h.z <= top.z) {
        return true;
    }
    else
        return false;
}

