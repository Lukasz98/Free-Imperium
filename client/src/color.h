#pragma once
#include <functional>

struct Color3 {
    unsigned char r, g, b;

    bool operator==(const Color3& p) const
    {
        if (r == p.r && g == p.g && b == p.b)
            return true;
        return false;
    }

    bool operator!=(const Color3& p) const
    {
        if (r == p.r && g == p.g && b == p.b)
            return false;
        return true;
    }

    void operator=(const Color3& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
    }
};

struct Color {
    unsigned char r, g, b, a = 255;

    int pushed = 0;
    void Push(unsigned char x)
    {
        switch (pushed) {
            case 0:
                r = x;
                break;
            case 1:
                g = x;
                break;
            case 2:
                b = x;
                break;
        }
        pushed++;
    }

    bool operator==(const Color& p) const
    {
        if (r == p.r && g == p.g && b == p.b && a == p.a)
            return true;
        return false;
    }

    bool operator!=(const Color& p) const
    {
        if (r == p.r && g == p.g && b == p.b && a == p.a)
            return false;
        return true;
    }

    void operator=(const Color& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }
};

namespace CCC {
struct Hash {
    std::size_t operator()(const Color& c) const
    {
        union U {
            std::size_t hash;
            unsigned char t[4];
        } u;
        u.hash = 0;
        u.t[0] = c.g;
        u.t[1] = c.r;
        u.t[2] = c.b;
        u.t[3] = c.a;
        return std::hash<std::size_t>{}(u.hash);
    }
};
}  // namespace CCC

