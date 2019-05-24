#pragma once

struct Color
{
    unsigned char r, g, b, a = 255;

    int pushed = 0;
    void Push(unsigned char x)
    {
        switch (pushed)
        {
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
    
    bool operator== (const Color & p)
    {
        if (r == p.r && g == p.g && b == p.b && a == p.a)
            return true;
        return false;
    }

    bool operator!= (const Color & p)
    {
        if (r == p.r && g == p.g && b == p.b && a == p.a)
            return false;
        return true;
    }
};
