#include "text.h"


Text::Text(glm::vec3 parentPos, std::unordered_map<std::string, std::string> & values, float maxWidth, int id)
    : Object{id}, maxWidth(maxWidth), centered(false), centerTo(0.0, 0.0, 0.0),
      bgColor{50, 50, 50, 0},
      color{100, 0, 0, 255}, height{30.0f}
{
    type = "text";
    for (auto & value : values) {
        std::stringstream stream;
        stream << value.second;
        std::string val;
        stream >> val;
        if (value.first == "position:") {
            relativePos.x = std::stof(val);
            stream >> val;
            relativePos.y = std::stof(val);
            stream >> val;
            relativePos.z = std::stof(val);
        }
        else if (value.first == "height:") {
            height = std::stof(val);
        }
        else if (value.first == "color:") {
            color.r = (unsigned int)std::stoi(val);
            stream >> val;
            color.g = (unsigned int)std::stoi(val);
            stream >> val;
            color.b = (unsigned int)std::stoi(val);
            stream >> val;
            color.a = (unsigned int)std::stoi(val);
        }
        else if (value.first == "bgColor:") {
            bgColor.r = std::stoul(val);
            stream >> val;
            bgColor.g = std::stoul(val);
            stream >> val;
            bgColor.b = std::stoul(val);
            stream >> val;
            bgColor.a = std::stoul(val);
        }
        else if (value.first == "content:") {
            text = value.second;
        }
        else if (value.first == "valueName:") {
            valueName = value.second;
        }
        else if (value.first == "clickEvent:") {
            clickEvent = value.second;
        }
        else {
            Log(value.first << " ;;;; " << value.second);
            this->values[value.first] = value.second;
        }
    }
    
    position = relativePos + parentPos;
    
    if (text.size())
        set();
    else {
        text = "--";
        set();
    }
}


Text::~Text()
{
    //Log("~Text()");
}

void Text::set()
{
    int realCharSize = height * 0.7f;
    int charSize = realCharSize * 7;
    int dpi = 300;
       
    FT_Set_Pixel_Sizes(AM::face, 0, charSize);

    glm::vec2 realSize = {(unsigned int)0, 0};
    unsigned int realBelowBaseline = 0;
    
    int charPadding = (int)(100.0f * (1.0f / (float)charSize) + 0.5f);
    int realCharPadding = (int)(100.0f * (1.0f / (float)realCharSize) + 0.5f);
    char whiteSpace = '.';
    unsigned int atlas_w = 0, atlas_h = 0, belowBaseline = 0;
    int charCount = 0;

    for (auto c : text) {
        if (c == 32)
            c = whiteSpace;
        
        {
            // real Size
            FT_Set_Pixel_Sizes(AM::face, 0, realCharSize);
            if (FT_Load_Char(AM::face, c, FT_LOAD_RENDER))
                continue;
            FT_GlyphSlot g = AM::face->glyph;
            if (realSize.x + g->bitmap.width + realCharPadding > maxWidth)
                break;
    
            realSize.x += g->bitmap.width + realCharPadding;
            realSize.y = std::max((unsigned int)realSize.y, g->bitmap.rows);
            if (g->bitmap.rows - g->bitmap_top < 10000)
                realBelowBaseline = std::max(realBelowBaseline, g->bitmap.rows - g->bitmap_top);

        }

        {
            FT_Set_Pixel_Sizes(AM::face, 0, charSize);
            if (FT_Load_Char(AM::face, c, FT_LOAD_RENDER))
                continue;
            FT_GlyphSlot g = AM::face->glyph;
            atlas_w += g->bitmap.width + charPadding;
            atlas_h = std::max(atlas_h, g->bitmap.rows);
            if (g->bitmap.rows - g->bitmap_top < 10000)
                belowBaseline = std::max(belowBaseline, g->bitmap.rows - g->bitmap_top);
        }
        charCount++;
    }
    
    if (belowBaseline < 10000) {
        atlas_h += belowBaseline;
        realSize.y += realBelowBaseline;
    }
    glm::vec2 textSize = realSize;

    if (centered) {
        position.x = centerTo.x - textSize.x / 2;
        position.y = centerTo.y - (textSize.y + realBelowBaseline) / 2;
        position.z = relativePos.z + centerTo.z;
    }
    
    glm::vec3 textPos = position;
    textPos.y += (height - realSize.y - realBelowBaseline) / 2;

    if (text == "-")
        Log(atlas_w << ", " << atlas_h);
    unsigned char * pixels = new unsigned char[atlas_w * atlas_h * 4];


    for (int i = 0; i < atlas_w * atlas_h * 4 - 1; i += 4) {
        if (i + 3 >= atlas_w * atlas_h * 4 - 1)
            break;
        unsigned char xd = 25;
        pixels[i + 0] = bgColor.r;
        pixels[i + 1] = bgColor.g;
        pixels[i + 2] = bgColor.b;
        pixels[i + 3] = bgColor.a;        
    }
    int offsetX = 0;

    FT_Set_Pixel_Sizes(AM::face, 0, charSize);

    int lastColumn = 0;
    for (auto c : text) {
        charCount--;
        if (charCount < 0)
            break;
        if (c == ' ') {
            FT_Load_Char(AM::face, whiteSpace, FT_LOAD_RENDER);
            FT_GlyphSlot g = AM::face->glyph;
            offsetX += g->bitmap.width + charPadding;
            continue;
        }
        if(FT_Load_Char(AM::face, c, FT_LOAD_RENDER))
            continue;
        FT_GlyphSlot g = AM::face->glyph;
        for (int x = 0; x < g->bitmap.width; x++) {
            for (int y = g->bitmap.rows - 1; y >= 1; y--) {
                int index = 0;
                if (belowBaseline > 0)
                    index = (x + offsetX + (y - (g->bitmap.rows - g->bitmap_top) + belowBaseline) * atlas_w)*4;
                //else
                //Log(g->bitmap.rows);

                if (offsetX + x >= atlas_w)
                    break;
                if (index >= atlas_w * atlas_h * 4)
                    break;

                unsigned char transparency = g->bitmap.buffer[x + (g->bitmap.rows - y) * g->bitmap.width];

                if (transparency == 255) {  // Draw char 
                    pixels[index + 0] = color.r;
                    pixels[index + 1] = color.g;
                    pixels[index + 2] = color.b;
                    pixels[index + 3] = transparency;
                }
                else { // draw background color 
                    pixels[index + 0] = bgColor.r;
                    pixels[index + 1] = bgColor.g;
                    pixels[index + 2] = bgColor.b;
                    pixels[index + 3] = bgColor.a;
                }
            }
                
        }
        offsetX += g->bitmap.width + charPadding;
        lastColumn = offsetX;
    }

    auto texture = std::make_shared<Texture>(pixels, atlas_w, atlas_h);
    content = std::make_unique<Rectangle>(textPos, textSize);
    content->SetTexture(texture);

    delete [] pixels;

    glm::vec4 bgColVec = { (double)bgColor.r / 255, (double)bgColor.g / 255,
                           (double)bgColor.b / 255, (double)bgColor.a / 255
                         };
    background = std::make_unique<Rectangle>(position, textSize, bgColVec);
    size = textSize;
//Log("TEXTPOS = " <<position.x << " " << position.y << " " << position.z);
}

void Text::SetPos(glm::vec3 p)
{
    position = relativePos + p;
    //position = p;
    if (background)
        background->SetPosition(position);
    if (content)
        content->SetPosition(position);
}

void Text::Draw()
{
    if (!visible)
        return;

    if (background)
        background->Draw();
    if (content)
        content->Draw();
}

void Text::SetClick(GuiClick c)
{
    Object::SetClick(c);
    guiClick.values["content:"] = text;
}

void Text::SetContent(const std::string & ct)
{
    if (ct == text)
        return;
    text = ct;
    guiClick.values["content:"] = text;
    set();
}

void Text::Move(glm::vec3 p)
{
    position += p;
    if (background)
        background->SetPosition(position);
    if (content)
        content->SetPosition(position);
}

void Text::MakeCentered(glm::vec3 newPos)
{
    centered = true;
    centerTo = newPos;
    if (text.size())
        set();
}
