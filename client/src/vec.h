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

