#include "model_3d.h"
#include <math.h>

Model3D::Model3D(const std::string & path, glm::vec3 pos)
: position{pos}
{

	// Load .obj File
	bool loadout = Loader.LoadFile(path);

	// Check to see if it loaded

	// If so continue
	if (!loadout) {
        Log("Nie moge zaladowac modelu: " << path);
        return;
    }

        Log("Werticow= " << Loader.LoadedVertices.size() << ", " << Loader.LoadedMaterials.size());

#define meshId 13
    std::vector<glm::vec3> verts;
    std::vector<float> rawVerts;
    //std::vector<float> normals;

int minX = 1111, minY = 1111, maxX = -1111, maxY = -1111;

    for (auto v : Loader.LoadedVertices) {
    //for (auto v : Loader.LoadedMeshes[meshId].Vertices) {
        if (v.Position.X < minX)
            minX = v.Position.X;
        if (v.Position.X > maxX)
            maxX = v.Position.X;
        if (v.Position.Y < minY)
            minY = v.Position.Y;
        if (v.Position.Y > maxY)
            maxY = v.Position.Y;
        

        verts.push_back(glm::vec3{
            v.Position.X, v.Position.Y, v.Position.Z
        });
        rawVerts.push_back(v.Position.X);
        rawVerts.push_back(v.Position.Y);
        rawVerts.push_back(v.Position.Z);
        rawVerts.push_back(v.Normal.X);
        rawVerts.push_back(v.Normal.Y);
        rawVerts.push_back(v.Normal.Z);
        rawVerts.push_back(v.TextureCoordinate.X);
        rawVerts.push_back(v.TextureCoordinate.Y);
    }

plane = std::make_unique<Rectangle>(glm::vec3{minX, minY, 0}, glm::vec2{maxX - minX, maxY - minY}, glm::vec4{1.0, 0.0,0.0,0.5});

//    for (auto v : verts) {
 //       rawVerts.push_back(v.x);
//        rawVerts.push_back(v.y);
   //     rawVerts.push_back(v.z);
    //}


    glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
 bind();
    glBufferData(GL_ARRAY_BUFFER, rawVerts.size() * sizeof(float),
        rawVerts.data(), GL_STATIC_DRAW
    );

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Loader.LoadedIndices.size() * sizeof(unsigned int),
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, Loader.LoadedMeshes[meshId].Indices.size() * sizeof(unsigned int),
        Loader.LoadedIndices.data(), GL_STATIC_DRAW
    );


    //Log("Mesh count=" << Loader.LoadedVertices.size());
//    bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(6 * sizeof(float)));


    iboCount = Loader.LoadedIndices.size();

    

    unbind();
}

Model3D::~Model3D()
{
    //glDeleteVertexArrays(1, &vao);
    //glDeleteBuffers(1, &vbo);
}

void Model3D::Draw()
{
    bind();
    //glDrawArrays(GL_TRIANGLES, 0, Loader.LoadedVertices.size());
    glDrawElements(GL_TRIANGLES, Loader.LoadedIndices.size(), GL_UNSIGNED_INT, nullptr);
    //glDrawElements(GL_TRIANGLES, Loader.LoadedMeshes[meshId].Indices.size(), GL_UNSIGNED_INT, nullptr);
    unbind();
}

void Model3D::DrawRect(glm::mat4 ml)
{
  //  auto pos = plane->GetPosition();
//glm::vec3 pos2 = ml * glm::vec4{pos,1.0};
//plane->SetPosition(pos);
    plane->Draw();
    //plane->SetPosition(pos);
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


bool Model3D::Click(const glm::mat4 & ml, const glm::mat4 & mlRot, const glm::vec3 & vv, const glm::vec3 & eye, float yScale, float rotateAngle)
{
    glm::vec3 normal{0.0, 0.0, -1.0};
    normal = mlRot * glm::vec4{normal, 1.0};

    double dn = glm::dot(vv, normal);
    if (dn == 0.0) return false;


    glm::vec3 right{0.0, 0.0, 0.0};
    right = ml * glm::vec4{right, 1.0};

    glm::vec2 realSize =  glm::vec4{plane->GetSize(), 1.0, 1.0};
    //glm::vec2 realSize =  glm::vec4{plane->GetSize().x, plane->GetSize().y, 1.0, 1.0};
    glm::vec3 left{-realSize.x, 0.0, 0.0};
    left = ml * glm::vec4{left, 1.0};

    double tt = glm::dot(right - eye, normal) / dn;
    
    glm::vec3 qq{vv.x * tt, vv.y * tt, vv.z * tt};
    glm::vec3 h{eye + qq};

    glm::vec3 bottom{0.0, 0.0, 0.0};
    bottom = ml * glm::vec4{bottom, 1.0};

    glm::vec3 top{0.0, realSize.y, 0.0};
    //top = ml * glm::vec4{top, 1.0};

    //Log("\nH= " <<h.x<<", "<<h.y << ", "<<h.z);
    //Log("bot= " <<bottom.x<<", "<<bottom.y << ", "<<bottom.z);
    //Log("top= " <<top.x<<", "<<top.y << ", "<<top.z);

    double model_h = realSize.y * yScale;

#define PI 3.14159265
    double z = sin(rotateAngle) * model_h;
    double y = cos(rotateAngle) * z;
    top = bottom;
    top.z += z;
    top.y += y;
    
    //Log("top2= " <<top.x<<", "<<top.y << ", "<<top.z);
    //Log("left= " <<left.x<<", "<<left.y << ", "<<left.z);
    //Log("right= " <<right.x<<", "<<right.y << ", "<<right.z);

    if (h.x > left.x && h.x < right.x && 
        h.y > bottom.y && h.y < top.y &&
        h.z >= bottom.z && h.z <= top.z) {
        return true;
        //Log("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBB");
    }
    else 
        return false;
    //Log("nie");
} 



