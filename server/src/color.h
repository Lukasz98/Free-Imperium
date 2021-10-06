#pragma once

struct Color
{
    unsigned char r, g, b, a = 255;

    int pushed = 0;
    void Push(unsigned char x);
    
    bool operator== (const Color & p);
    bool operator!= (const Color & p);
    
};
