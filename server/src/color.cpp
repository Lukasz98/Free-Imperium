#include "color.h"

void Color::Push(unsigned char x)
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

bool Color::operator==(const Color &p)
{
    if (r == p.r && g == p.g && b == p.b && a == p.a)
        return true;
    return false;
}

bool Color::operator!=(const Color &p)
{
    if (r == p.r && g == p.g && b == p.b && a == p.a)
        return false;
    return true;
}
