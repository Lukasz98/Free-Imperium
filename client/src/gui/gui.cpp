#include "gui.h"
#include "load_from_png.h"
#include "../asset_manager.h"


Gui::TextLabel::~TextLabel()
{
    delete text;
}
Gui::ClickEventType Gui::TextLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return Gui::ClickEventType::NONE;
}
void Gui::TextLabel::setText(const std::string & text)
{
    if (this->text == nullptr)
        this->text = new Text{};
    this->text->text = text;
    int realCharSize = this->text->height * 0.7f;
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
            if (realSize.x + g->bitmap.width + realCharPadding > this->text->maxWidth)
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

    if (this->text->centered) {
        this->text->position.x = this->text->centerTo.x - textSize.x / 2;
        this->text->position.y = this->text->centerTo.y - (textSize.y + realBelowBaseline) / 2;
        this->text->position.z = this->text->centerTo.z;
    }
    
    glm::vec3 textPos = this->text->position;
    textPos.y += (this->text->height - realSize.y - realBelowBaseline) / 2;

    if (text == "-")
        Log(atlas_w << ", " << atlas_h);
    unsigned char * pixels = new unsigned char[atlas_w * atlas_h * 4];


    for (int i = 0; i < atlas_w * atlas_h * 4 - 1; i += 4) {
        if (i + 3 >= atlas_w * atlas_h * 4 - 1)
            break;
        unsigned char xd = 25;
        pixels[i + 0] = this->text->bgC.r;
        pixels[i + 1] = this->text->bgC.g;
        pixels[i + 2] = this->text->bgC.b;
        pixels[i + 3] = this->text->bgC.a;        
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
                    pixels[index + 0] = this->text->textC.r;
                    pixels[index + 1] = this->text->textC.g;
                    pixels[index + 2] = this->text->textC.b;
                    pixels[index + 3] = transparency;
                }
                else { // draw background color 
                    pixels[index + 0] = this->text->bgC.r;
                    pixels[index + 1] = this->text->bgC.g;
                    pixels[index + 2] = this->text->bgC.b;
                    pixels[index + 3] = this->text->bgC.a;
                }
            }
                
        }
        offsetX += g->bitmap.width + charPadding;
        lastColumn = offsetX;
    }

    this->text->texture = std::make_unique<Texture>(pixels, atlas_w, atlas_h);
    this->text->backgr = std::make_unique<Rectangle>(textPos, textSize);
    //this->text->backgr = std::make_unique<Rectangle>(this->text->position, textSize);
    //this->text->content->SetTexture(texture);

    delete [] pixels;

    glm::vec4 bgColVec = { (double)this->text->bgC.r / 255, (double)this->text->bgC.g / 255,
                           (double)this->text->bgC.b / 255, (double)this->text->bgC.a / 255
                         };
    //this->backgr = std::make_unique<Rectangle>(this->text->position, textSize, bgColVec);
    //size = textSize;
Log("TEXTSIZE = " <<textSize.x << " " << textSize.y);// << " " << textSize.z);
//Log("TEXTPOS = " <<position.x << " " << position.y << " " << position.z);
}
void Gui::TextLabel::Draw()
{
Log("drt");
    if (backgr)
        backgr->Draw();
    if (text && text->texture && text->backgr) {
Log("drt2");
        text->texture->Bind();
        //backgr->Draw();
        text->backgr->Draw();
        text->texture->Unbind();
    }
}







Gui::IconLabel::~IconLabel()
{
    delete icon;
}
Gui::ClickEventType Gui::IconLabel::Click(const glm::vec2 & mPos)
{
    if (backgr && backgr->Click(mPos.x, mPos.y))
        return evName;
    return Gui::ClickEventType::NONE;
}
void Gui::IconLabel::setIcon(const std::string & path)
{
    if (icon == nullptr)
        icon = new Icon{};
    icon->iconPath = path;
    icon->texture = std::make_unique<Texture>(path);
    backgr = std::make_unique<Rectangle>(pos, size);
}
void Gui::IconLabel::Draw()
{
    if (icon && icon->texture)
        icon->texture->Bind();
    if (backgr)
        backgr->Draw();
    if (icon && icon->texture)
        icon->texture->Unbind();
}


Gui::Group::~Group()
{
    for (auto g : groups)
        delete g;
    for (auto text : tLabels)
        delete text;
    for (auto icon : iLabels)
        delete icon;
}
bool Gui::Group::Click(Gui::ClickData & clickData, const glm::vec2 & mPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    
    for (auto t : tLabels) {
        auto ev = t->Click(mPos);
        if (ev != Gui::ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }

    for (auto i : iLabels) {
        auto ev = i->Click(mPos);
        if (ev != Gui::ClickEventType::NONE) {
            clickData.group = this;
            clickData.evType = ev;
            return true;
        }
    }
    return false;
}
void Gui::Group::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto text : tLabels) {
        text->Draw();
    }
    for (auto icon : iLabels) {
        icon->Draw();
    }
}



Gui::List::~List()
{
    for (auto g : groups)
        delete g;
}
bool Gui::List::Click(Gui::ClickData & clickData, const glm::vec2 & mPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mPos))
            return true;
    }
    return false;
}
void Gui::List::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto grp : groups)
        grp->Draw();
}



Gui::Window::~Window()
{
    for (auto g : groups)
        delete g;
}
bool Gui::Window::GetClick(Gui::ClickData & clickData, glm::vec2 mousPos)
{
    for (auto g : groups) {
        if (g->Click(clickData, mousPos))
            return true;
    }
    return false;
}
void Gui::Window::Draw()
{
    if (backgr)
        backgr->Draw();
    for (auto grp : groups) {
        grp->Draw();
    }
}


std::vector<Gui::Window*> windows;

void Gui::Draw()
{
    for (auto w : windows) {
        //Log("Draw");
        w->Draw();
    }
}

void Gui::OpenTopBar(const std::vector<std::string> & values)
{
    Window * w = new Window{};
    windows.push_back(w);
    w->type = WindowType::TOP_BAR;
    w->id = 0;
    w->backgr = std::make_unique<Rectangle>(glm::vec3{100.0, 100.0, 0.0}, glm::vec2{400.0, 400.0}, glm::vec4{1.0, 0.0, 0.0, 1.0});

    Group * grp = new Group{};
    grp->id = 0;
    w->groups.push_back(grp);
    grp->backgr = std::make_unique<Rectangle>(glm::vec3{110.0, 110.0, 1.0}, glm::vec2{200.0, 200.0}, glm::vec4{1.0, 1.0, 0.0, 1.0});

    TextLabel * title = new TextLabel{};
    grp->tLabels.push_back(title);
    title->backgr = std::make_unique<Rectangle>(glm::vec3{100.0, 100.0, 1.0}, glm::vec2{200.0, 200.0}, glm::vec4{.0, 1.0, 0.0, .5});
   
    title->id = 0;
    title->text = new TextLabel::Text{};
    title->text->height = 140;
    title->text->textC = Color{0, 0, 255, 255};
    title->text->bgC = Color{0, 255, 255, 255};
    title->text->maxWidth = 550;
    title->text->centered= false;
    title->text->position = glm::vec3{10.0, 10.0, .1};
    //title->text->text = values[0];
    title->setText("huala");//values[0]);
    
}
