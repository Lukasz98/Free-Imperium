#pragma once
#include <math.h>

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
    void add(Vec3 v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    bool operator==(const Vec3& v)
    {
        return x == v.x && y == v.y && z == v.z;
    }
};

struct Vec4 {
    float x, y, z, w;
    bool operator==(const Vec4 b)
    {
        return x == b.x && y == b.y && z == b.z && w == b.w;
    }
};

struct Mat4 {
    float v[16];
    Mat4()
    {
        for (int i = 0; i < 16; ++i) v[i] = 0.0f;
        v[0] = 1.0f;
        v[5] = 1.0f;
        v[10] = 1.0f;
        v[15] = 1.0f;
    }

    void translate(const Vec3& vec)
    {
        v[3] = vec.x;
        v[7] = vec.y;
        v[11] = vec.z;
    }

    void rotate(const Vec3& vec, float a)
    {
        Mat4 m;
        m.v[5] = m.v[5] + vec.x * cos(a);
        m.v[6] = m.v[6] + vec.x * sin(a);
        m.v[9] = m.v[9] + vec.x * -sin(a);
        m.v[10] = m.v[10] + vec.x * cos(a);

        m.v[0] = m.v[0] + vec.y * cos(a);
        m.v[2] = m.v[2] + vec.y * -sin(a);
        m.v[8] = m.v[8] + vec.y * sin(a);
        m.v[10] = m.v[10] + vec.y * cos(a);

        m.v[0] = m.v[0] + vec.z * cos(a);
        m.v[1] = m.v[1] + vec.z * -sin(a);
        m.v[4] = m.v[4] + vec.z * sin(a);
        m.v[5] = m.v[5] + vec.z * cos(a);

        mul(m);
    }

    void mul(const Mat4& m)
    {
        Mat4 r;
        r.v[0] = v[0] * m.v[0] + v[1] * m.v[4] + v[2] * m.v[8] + v[3] * m.v[12];
        r.v[1] = v[0] * m.v[1] + v[1] * m.v[5] + v[2] * m.v[9] + v[3] * m.v[13];
        r.v[2] = v[0] * m.v[2] + v[1] * m.v[6] + v[2] * m.v[10] + v[3] * m.v[14];
        r.v[3] = v[0] * m.v[3] + v[1] * m.v[7] + v[2] * m.v[11] + v[3] * m.v[15];

        r.v[4] = v[4] * m.v[0] + v[5] * m.v[4] + v[6] * m.v[8] + v[7] * m.v[12];
        r.v[5] = v[4] * m.v[1] + v[5] * m.v[5] + v[6] * m.v[9] + v[7] * m.v[13];
        r.v[6] = v[4] * m.v[2] + v[5] * m.v[6] + v[6] * m.v[10] + v[7] * m.v[14];
        r.v[7] = v[4] * m.v[3] + v[5] * m.v[7] + v[6] * m.v[11] + v[7] * m.v[15];

        r.v[8] = v[8] * m.v[0] + v[9] * m.v[4] + v[10] * m.v[8] + v[11] * m.v[12];
        r.v[9] = v[8] * m.v[1] + v[9] * m.v[5] + v[10] * m.v[9] + v[11] * m.v[13];
        r.v[10] = v[8] * m.v[2] + v[9] * m.v[6] + v[10] * m.v[10] + v[11] * m.v[14];
        r.v[11] = v[8] * m.v[3] + v[9] * m.v[7] + v[10] * m.v[11] + v[11] * m.v[15];

        r.v[12] = v[12] * m.v[0] + v[13] * m.v[4] + v[14] * m.v[8] + v[15] * m.v[12];
        r.v[13] = v[12] * m.v[1] + v[13] * m.v[5] + v[14] * m.v[9] + v[15] * m.v[13];
        r.v[14] = v[12] * m.v[2] + v[13] * m.v[6] + v[14] * m.v[10] + v[15] * m.v[14];
        r.v[15] = v[12] * m.v[3] + v[13] * m.v[7] + v[14] * m.v[11] + v[15] * m.v[15];
    
        for (int i = 0; i < 16; ++i) 
            v[i] = r.v[i];

    }

    Vec4 mul(const Vec4& vec)
    {
        Vec4 r;
        r.x = v[0] * vec.x + v[1] * vec.y + v[2] * vec.z + v[3];
        r.y = v[4] * vec.x + v[5] * vec.y + v[6] * vec.z + v[7];
        r.z = v[8] * vec.x + v[9] * vec.y + v[10] * vec.z + v[11];
        r.w = v[12] * vec.x + v[13] * vec.y + v[14] * vec.z + v[15];
        return r;
    }
};
