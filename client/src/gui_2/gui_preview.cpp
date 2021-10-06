#include "gui_preview.h"
#include "camera.h"
#include "asset_manager.h"

           #include <iomanip> 
#include <fstream>
#include <string>
#include <cstring>
#include "load_values_from_txt.h"

using namespace GuiPreview;
int lineCount = 0;
/*
Vec3 loadVec4(char * ptr) // assumes pointer is continuation of strtok
{
    Vec3 v; 
    ptr = strtok(NULL, " ");
    v.x = std::stof(ptr);
    ptr = strtok(NULL, " ");
    v.y = std::stof(ptr);
    ptr = strtok(NULL, " ");
    v.z = std::stof(ptr);
    return v;
}
Vec3 loadVec4(char * ptr) // assumes pointer is continuation of strtok
{
    Vec3 v; 
    ptr = strtok(NULL, " ");
    v.x = std::stof(ptr);
    ptr = strtok(NULL, " ");
    v.y = std::stof(ptr);
    ptr = strtok(NULL, " ");
    v.z = std::stof(ptr);
    return v;
}
*/

void loadChars(const std::string & s, int max, Win & win, Vec3 parentPos, Vec2 parentSize, int textAlign, int padding, AM::FontSize fontSize, Vec4 color)
{
    Vec3 textPos{0.0f, 0.0f, 0.1f};
    textPos.add(parentPos);
    //GLuint tid = AM::atlasTexture[AM::FontSize::PX32]->GetId();
    GLuint tid = (int)fontSize;
    float spaceWidth = 10.0f;
    float off = 0.0f, betweenWidth = 1.0f;

    //if (textAlign == 1 || textAlign == 2) { // if center or right
    float widthSum = 0.0f, maxY = 0.0f;
    for (int i = 0; i < s.size() && i < max; ++i) {
        if (AM::atlasInfo[fontSize][s[i]].set) {
            widthSum += AM::atlasInfo[fontSize][s[i]].width + betweenWidth;
            if (maxY < AM::atlasInfo[fontSize][s[i]].height)
                maxY = AM::atlasInfo[fontSize][s[i]].height;
        }
        else
            widthSum += spaceWidth;
    }
    if (textAlign == 1) // centered
        textPos.x = (parentPos.x + parentSize.x * 0.5f) - widthSum * 0.5f;
    else if (textAlign == 2) // aligned to right
        textPos.x = (parentPos.x + parentSize.x) - widthSum - padding;
    //}
    else { // aligned left
        textPos.x += padding;
    }
    textPos.y = (parentPos.y + parentSize.y * 0.5f) - maxY * 0.5f;
    
    for (int i = 0; i < s.size() && i < max; ++i) {
        Vec3 rPos;
        Vec2 rSize, rLBTC, tcLen;
        if (AM::atlasInfo[fontSize][s[i]].set) {
            rPos = {textPos.x + off, textPos.y + AM::atlasInfo[fontSize][s[i]].yPos, textPos.z};
            // this 2 if's are walkaround around bug. If we draw char at x or y with floating point
            // it gets more blury the closer it gets to .5f, at .0f it looks fine
            if (rPos.x > (((float)((int)rPos.x))))
                rPos.x = ((int)(rPos.x + 0.5f));
            if (rPos.y > (((float)((int)rPos.y))))
                rPos.y = ((int)(rPos.y + 0.5f));
            rSize = {(float)AM::atlasInfo[fontSize][s[i]].width, (float)AM::atlasInfo[fontSize][s[i]].height};
            rLBTC = {AM::atlasInfo[fontSize][s[i]].tcX, AM::atlasInfo[fontSize][s[i]].tcY};
            tcLen = {AM::atlasInfo[fontSize][s[i]].tcWidth, AM::atlasInfo[fontSize][s[i]].tcHeight};
            off += rSize.x + betweenWidth;

            int vindex = win.vertexes.size();
            win.vertexes.push_back({rPos, color, rLBTC, (float)tid});
            win.vertexes.push_back({rPos, color, rLBTC, (float)tid});
            win.vertexes.push_back({rPos, color, rLBTC, (float)tid});
            win.vertexes.push_back({rPos, color, rLBTC, (float)tid});
            win.vertexes[vindex + 1].pos.y += rSize.y;
            win.vertexes[vindex + 2].pos.y += rSize.y;
            win.vertexes[vindex + 2].pos.x += rSize.x;
            win.vertexes[vindex + 3].pos.x += rSize.x;
            win.vertexes[vindex + 1].tc.y += tcLen.y;
            win.vertexes[vindex + 2].tc.y += tcLen.y;
            win.vertexes[vindex + 2].tc.x += tcLen.x;
            win.vertexes[vindex + 3].tc.x += tcLen.x;
            //text.rects.push_back(std::make_unique<Rectangle>(rPos, rSize, rLBTC, tcLen)); 
            if (s[i] == '6' || s[i] == '5') {
                //Log(rPos.x << " " << rPos.y);
                //Log(rSize.x << " " << rSize.y);
                //Log(rLBTC.x << " " << rLBTC.y);
                //Log(tcLen.x << " " << tcLen.y);
                Log(std::fixed << std::setprecision(10) << win.vertexes[vindex + 0].pos.x);
                Log(std::fixed << std::setprecision(10) << win.vertexes[vindex + 1].pos.x);
                Log(std::fixed << std::setprecision(10) << win.vertexes[vindex + 2].pos.x);
                Log(std::fixed << std::setprecision(10) << win.vertexes[vindex + 3].pos.x);
                //Log(win.vertexes[vindex + 3].pos.x);
            }
        }
        else {
            rPos = {textPos.x + off, textPos.y, textPos.z};
            rSize = {spaceWidth, 10.0};
            rLBTC = {0, 0};
            tcLen = {0, 0};
            off += rSize.x;
        }
      //  Log(rPos.x << " " << rPos.y << " "<<rPos.z);
    }
    for (int i = max - s.size(); i >= 0; --i) { // trzeba chyba jeszcze dopisac za spacje
       win.vertexes.push_back({Vec3{}, Vec4{}, Vec2{}, -1});
       win.vertexes.push_back({Vec3{}, Vec4{}, Vec2{}, -1});
       win.vertexes.push_back({Vec3{}, Vec4{}, Vec2{}, -1});
       win.vertexes.push_back({Vec3{}, Vec4{}, Vec2{}, -1});
    }

}

TextLabel10 loadTextLabel(std::fstream & file, Win & win, Vec3 parentPos, int max)
{
    bool pos = false, sizeb = false, color = false, ended = false;
    Vec2 size;
    Vec4 fontColor{1.0f, 1.0f, 1.0f, 1.0f};
    int textAlign = 1, padding = 3;
    AM::FontSize fontSize = AM::FontSize::PX32;
    TextLabel10 textLabel;
    textLabel.id = win.vertexes.size();
    textLabel.firstCharId = textLabel.id + 4;
    textLabel.lastCharId = textLabel.id + 4 + 10;
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});

    char * textContent = NULL;
    std::string line;
    while (getline(file, line)) {
        ++lineCount;
        char * ptr = line.data();
        ptr = strtok(line.data(), " ");
        while (ptr != NULL) {
            Log(ptr); 
            if (strcmp(ptr, "pos:") == 0) {
                win.vertexes[textLabel.id].pos = loadVec3(ptr);
                win.vertexes[textLabel.id + 1].pos = win.vertexes[textLabel.id].pos;
                win.vertexes[textLabel.id + 2].pos = win.vertexes[textLabel.id].pos;
                win.vertexes[textLabel.id + 3].pos = win.vertexes[textLabel.id].pos;
                win.vertexes[textLabel.id + 1].pos.y += size.y;
                win.vertexes[textLabel.id + 2].pos.y += size.y;
                win.vertexes[textLabel.id + 2].pos.x += size.x;
                win.vertexes[textLabel.id + 3].pos.x += size.x;
                win.vertexes[textLabel.id + 0].pos.add(parentPos);
                win.vertexes[textLabel.id + 1].pos.add(parentPos);
                win.vertexes[textLabel.id + 2].pos.add(parentPos);
                win.vertexes[textLabel.id + 3].pos.add(parentPos);

                //textLabel.verts[0].pos = loadVec3(ptr);
                //textLabel.verts[1].pos = textLabel.verts[0].pos;
                //textLabel.verts[2].pos = textLabel.verts[0].pos;
                //textLabel.verts[3].pos = textLabel.verts[0].pos;
                pos = true;
                break;
            }
            else if (strcmp(ptr, "size:") == 0) {
                size = loadVec2(ptr);
                sizeb = true;
                break;
            }
            else if (strcmp(ptr, "color:") == 0) {
                win.vertexes[textLabel.id].color = loadVec4(ptr);
                win.vertexes[textLabel.id + 1].color = win.vertexes[textLabel.id].color;
                win.vertexes[textLabel.id + 2].color = win.vertexes[textLabel.id].color;
                win.vertexes[textLabel.id + 3].color = win.vertexes[textLabel.id].color;
                color = true;
                break;
            }
            else if (strcmp(ptr, "fontColor:") == 0) {
                fontColor = loadVec4(ptr);
            }
            else if (strcmp(ptr, "textureId:") == 0) {
                win.vertexes[textLabel.id].textureId = loadInt(ptr);
                win.vertexes[textLabel.id + 1].textureId = win.vertexes[textLabel.id].textureId;
                win.vertexes[textLabel.id + 2].textureId = win.vertexes[textLabel.id].textureId;
                win.vertexes[textLabel.id + 3].textureId = win.vertexes[textLabel.id].textureId;
            }
            else if (strcmp(ptr, "padding:") == 0) {
                padding = loadInt(ptr);
            }
            else if (strcmp(ptr, "textAlign:") == 0) {
                ptr = strtok(NULL, " ");
                if (strcmp(ptr, "left") == 0)
                    textAlign = 0;
                else if (strcmp(ptr, "center") == 0)
                    textAlign = 1;
                else if (strcmp(ptr, "right") == 0)
                    textAlign = 2;
            }
            else if (strcmp(ptr, "text:") == 0) {
                ptr = strtok(NULL, " ");
                textContent = ptr;
                ptr = NULL;
            }
            else if (strcmp(ptr, "fontSize:") == 0) {
                int fs = loadInt(ptr);
                if (fs == 16)
                    fontSize = AM::FontSize::PX16;
                else if (fs == 32)
                    fontSize = AM::FontSize::PX32;
                else if (fs == 64)
                    fontSize = AM::FontSize::PX64;
            }
            else if (strcmp(ptr, "}") == 0) {
                ended = true;
                break;
            }
            else {
                ended = true;
                Log("error -> line:" << lineCount);
                break;
                //ptr = strtok(NULL, " ");
            }
            ptr = strtok(NULL, " ");
        }
        if (ended)
            break;
    }

    if (!(pos && sizeb && color)) {
        Log("Too little parameters: pos, size, color");
    }

///void loadChars(const std::string & s, int max, Win & win, Vec3 parentPos)
    if (textContent != NULL) {
        loadChars(textContent, max, win, win.vertexes[textLabel.id].pos, size, textAlign, padding, fontSize, fontColor);
    
    }
    //textLabel.verts[1].pos.y += size.y;
    //textLabel.verts[2].pos.y += size.y;
    //textLabel.verts[2].pos.x += size.x;
    //textLabel.verts[3].pos.x += size.x;

    return textLabel;
}


Group loadGroup(std::fstream & file, Win & win, Vec3 parentPos)
{
    bool pos = false, sizeb = false, color = false, ended = false;
    Vec2 size;
    Group group;
    group.id = win.vertexes.size();
    group.firstTextLabel10Id = group.id + 4;
    group.lastTextLabel10Id = group.firstTextLabel10Id;
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    std::string line;
    while (getline(file, line)) {
        ++lineCount;
        char * ptr = line.data();
        ptr = strtok(line.data(), " ");
        while (ptr != NULL) {
            Log(ptr); 
            if (strcmp(ptr, "textLabel10{") == 0) {
                group.textLabels.push_back(loadTextLabel(file, win, win.vertexes[group.id].pos, 10));
                //TextLabel10 tl = loadTextLabel10(file, win);
                group.lastTextLabel10Id += 10;
                break;
            }
            else if (strcmp(ptr, "pos:") == 0) {
                win.vertexes[group.id].pos = loadVec3(ptr);
                win.vertexes[group.id + 1].pos = win.vertexes[group.id].pos;
                win.vertexes[group.id + 2].pos = win.vertexes[group.id].pos;
                win.vertexes[group.id + 3].pos = win.vertexes[group.id].pos;
                win.vertexes[group.id + 1].pos.y += size.y;
                win.vertexes[group.id + 2].pos.y += size.y;
                win.vertexes[group.id + 2].pos.x += size.x;
                win.vertexes[group.id + 3].pos.x += size.x;
                win.vertexes[group.id + 0].pos.add(parentPos);
                win.vertexes[group.id + 1].pos.add(parentPos);
                win.vertexes[group.id + 2].pos.add(parentPos);
                win.vertexes[group.id + 3].pos.add(parentPos);
                //group.verts[0].pos = loadVec3(ptr);
                //group.verts[1].pos = group.verts[0].pos;
                //group.verts[2].pos = group.verts[0].pos;
                //group.verts[3].pos = group.verts[0].pos;
                pos = true;
                break;
            }
            else if (strcmp(ptr, "size:") == 0) {
                size = loadVec2(ptr);
                sizeb = true;
                break;
            }
            else if (strcmp(ptr, "color:") == 0) {
                win.vertexes[group.id].color = loadVec4(ptr);
                win.vertexes[group.id + 1].color = win.vertexes[group.id].color;
                win.vertexes[group.id + 2].color = win.vertexes[group.id].color;
                win.vertexes[group.id + 3].color = win.vertexes[group.id].color;
                //group.verts[0].color = loadVec4(ptr);
                //group.verts[1].color = group.verts[0].color;
                //group.verts[2].color = group.verts[0].color;
                //group.verts[3].color = group.verts[0].color;
                color = true;
                break;
            }
            else if (strcmp(ptr, "textureId:") == 0) {
                win.vertexes[group.id].textureId = loadInt(ptr);
                win.vertexes[group.id + 1].textureId = win.vertexes[group.id].textureId;
                win.vertexes[group.id + 2].textureId = win.vertexes[group.id].textureId;
                win.vertexes[group.id + 3].textureId = win.vertexes[group.id].textureId;
                //group.verts[0].textureId = loadInt(ptr);
                //group.verts[1].textureId = group.verts[0].textureId;
                //group.verts[2].textureId = group.verts[0].textureId;
                //group.verts[3].textureId = group.verts[0].textureId;
                break;
            }
            else if (strcmp(ptr, "}") == 0) {
                ended = true;
                break;
            }
            else {
                Log("error -> line: "<<lineCount);
                ended = true;
                break;
                //ptr = strtok(NULL, " ");
            }
            
        }
        if (ended)
            break;
    }

    if (!(pos && sizeb && color)) {
        Log("Too little parameters: pos, size, color");
    }

    //group.verts[1].pos.y += size.y;
    //group.verts[2].pos.y += size.y;
    //group.verts[2].pos.x += size.x;
    //group.verts[3].pos.x += size.x;

    return group;
}

Tab loadTab(std::fstream & file, Win & win, Vec3 parentPos)
{
    bool pos = false, sizeb = false, color = false, ended = false;
    Vec2 size;
    Tab tab;
    tab.id = win.vertexes.size();
    tab.firstGrpId = tab.id + 4;
    tab.lastGrpId = tab.firstGrpId;
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});
    win.vertexes.push_back({});
 
    std::string line;
    while (getline(file, line)) {
        ++lineCount;
        char * ptr = line.data();
        ptr = strtok(line.data(), " ");
        while (ptr != NULL) {
            Log(ptr); 
            if (strcmp(ptr, "group{") == 0) {
                tab.groups.push_back(loadGroup(file, win, win.vertexes[tab.id].pos));
                tab.lastGrpId = tab.groups.back().lastTextLabel10Id;
                break;
            }
            else if (strcmp(ptr, "pos:") == 0) {
                win.vertexes[tab.id].pos = loadVec3(ptr);
                win.vertexes[tab.id + 1].pos = win.vertexes[tab.id].pos;
                win.vertexes[tab.id + 2].pos = win.vertexes[tab.id].pos;
                win.vertexes[tab.id + 3].pos = win.vertexes[tab.id].pos;
                
                win.vertexes[tab.id + 1].pos.y += size.y;
                win.vertexes[tab.id + 2].pos.y += size.y;
                win.vertexes[tab.id + 2].pos.x += size.x;
                win.vertexes[tab.id + 3].pos.x += size.x;
                win.vertexes[tab.id + 0].pos.add(parentPos);
                win.vertexes[tab.id + 1].pos.add(parentPos);
                win.vertexes[tab.id + 2].pos.add(parentPos);
                win.vertexes[tab.id + 3].pos.add(parentPos);
                //tab.verts[0].pos = loadVec3(ptr);
                //tab.verts[1].pos = tab.verts[0].pos;
                //tab.verts[2].pos = tab.verts[0].pos;
                //tab.verts[3].pos = tab.verts[0].pos;
                pos = true;
                break;
            }
            else if (strcmp(ptr, "size:") == 0) {
                size = loadVec2(ptr);
                sizeb = true;
                break;
            }
            else if (strcmp(ptr, "color:") == 0) {
                win.vertexes[tab.id].color = loadVec4(ptr);
                win.vertexes[tab.id + 1].color = win.vertexes[tab.id].color;
                win.vertexes[tab.id + 2].color = win.vertexes[tab.id].color;
                win.vertexes[tab.id + 3].color = win.vertexes[tab.id].color;
                //tab.verts[0].color = loadVec4(ptr);
                //tab.verts[1].color = tab.verts[0].color;
                //tab.verts[2].color = tab.verts[0].color;
                //tab.verts[3].color = tab.verts[0].color;
                color = true;
                break;
            }
            else if (strcmp(ptr, "textureId:") == 0) {
                win.vertexes[tab.id].textureId= loadInt(ptr);
                win.vertexes[tab.id + 1].textureId = win.vertexes[tab.id].textureId;
                win.vertexes[tab.id + 2].textureId = win.vertexes[tab.id].textureId;
                win.vertexes[tab.id + 3].textureId = win.vertexes[tab.id].textureId;
                //tab.verts[0].textureId = loadInt(ptr);
                //tab.verts[1].textureId = tab.verts[0].textureId;
                //tab.verts[2].textureId = tab.verts[0].textureId;
                //tab.verts[3].textureId = tab.verts[0].textureId;
                break;
            }
            else if (strcmp(ptr, "}") == 0) {
                ended = true;
                break;
            }
            else {
                Log("error -> line: " << lineCount);
                ended = true;
                break;
                //ptr = strtok(NULL, " ");
            }
        }
        if (ended)
            break;
    }

    if (!(pos && sizeb && color)) {
        Log("Too little parameters: pos, size, color");
    }


    //tab.verts[1].pos.y += size.y;
    //tab.verts[2].pos.y += size.y;
    //tab.verts[2].pos.x += size.x;
    //tab.verts[3].pos.x += size.x;

    return tab;
}

Win load()
{
    std::string path{"src/gui_2/test.txt"};
    std::fstream file;
    file.open(path, std::ios::in);
    if (!file.is_open()) {
        Log("Failed reading: " << path);
        return Win{};
    }


    bool pos = false, sizeb = false, color = false, ended = false;
    Vec2 size;
    Win window;
    window.vertexes.push_back({});
    window.vertexes.push_back({});
    window.vertexes.push_back({});
    window.vertexes.push_back({});
    std::string line;
    while (getline(file, line)) {
        ++lineCount;
        char * ptr = line.data();
        ptr = strtok(line.data(), " ");
        while (ptr != NULL) {
            Log(ptr); 
            if (strcmp(ptr, "group{") == 0) {
                window.groups.push_back(loadGroup(file, window, window.vertexes[0].pos));
                //Group grp = loadGroup(file, win);
                break;
            }
            else if (strcmp(ptr, "tab{") == 0) {
                window.tabs.push_back(loadTab(file, window, window.vertexes[0].pos));
                break;
            }
            else if (strcmp(ptr, "pos:") == 0) {
                window.vertexes[0].pos = loadVec3(ptr);
                window.vertexes[0 + 1].pos = window.vertexes[0].pos;
                window.vertexes[0 + 2].pos = window.vertexes[0].pos;
                window.vertexes[0 + 3].pos = window.vertexes[0].pos;
                window.vertexes[0 + 1].pos.y += size.y;
                window.vertexes[0 + 2].pos.y += size.y;
                window.vertexes[0 + 2].pos.x += size.x;
                window.vertexes[0 + 3].pos.x += size.x;
                //window.verts[0].pos = loadVec3(ptr);
                //window.verts[1].pos = window.verts[0].pos;
                //window.verts[2].pos = window.verts[0].pos;
                //window.verts[3].pos = window.verts[0].pos;
                pos = true;
                break;
            }
            else if (strcmp(ptr, "size:") == 0) {
                size = loadVec2(ptr);
                sizeb = true;
                break;
            }
            else if (strcmp(ptr, "color:") == 0) {
                window.vertexes[0].color = loadVec4(ptr);
                window.vertexes[0 + 1].color = window.vertexes[0].color;
                window.vertexes[0 + 2].color = window.vertexes[0].color;
                window.vertexes[0 + 3].color = window.vertexes[0].color;
                //window.verts[0].color = loadVec4(ptr);
                //window.verts[1].color = window.verts[0].color;
                //window.verts[2].color = window.verts[0].color;
                //window.verts[3].color = window.verts[0].color;
                color = true;
                break;
            }
            else if (strcmp(ptr, "textureId:") == 0) {
                window.vertexes[0].textureId= loadInt(ptr);
                window.vertexes[0 + 1].textureId = window.vertexes[0].textureId;
                window.vertexes[0 + 2].textureId = window.vertexes[0].textureId;
                window.vertexes[0 + 3].textureId = window.vertexes[0].textureId;
                //window.verts[0].textureId = loadInt(ptr);
                //window.verts[1].textureId = window.verts[0].textureId;
                //window.verts[2].textureId = window.verts[0].textureId;
                //window.verts[3].textureId = window.verts[0].textureId;
                break;
            }
            else if (strcmp(ptr, "}") == 0) {
                ended = true;
                break;
            }
            else {
                Log("error -> line: " << lineCount);
                ended = true;
                break;
                //ptr = strtok(NULL, " ");
            }
        }
        if (ended)
            break;
    }

    if (!(pos && sizeb && color)) {
        Log("Too little parameters: pos, size, color");
    }

    Log("vertexes: " << window.vertexes.size());

    //window.verts[1].pos.y += size.y;
    //window.verts[2].pos.y += size.y;
    //window.verts[2].pos.x += size.x;
    //window.verts[3].pos.x += size.x;

    file.close();

    return window;
}

//void Play(Window & window, glm::Vec2 windowSize)
//void GuiPreview::Play(Window & window, glm::vec2 windowSize)
void GuiPreview::Play(Window & window, glm::vec2 resolution, glm::vec2 windowSize)
{
    Win guiWin = load();
    Shader shader("src/graphics/shaders/gui_batch/a.vert", 
            "src/graphics/shaders/gui_batch/a.frag", "", ""); 
    Camera camera{window.GetSize()};
    glUseProgram(shader.GetProgram());
    glm::mat4 matrix = glm::ortho(0.0f, (float)windowSize.x, 0.0f, (float)windowSize.y);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "pr_matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
    int texID[32];
    for (int i = 0; i < 32; i++)
        texID[i] = i;
    texID[0] = AM::atlasTexture[AM::FontSize::PX16]->GetId();
    texID[1] = AM::atlasTexture[AM::FontSize::PX32]->GetId();
    texID[2] = AM::atlasTexture[AM::FontSize::PX64]->GetId();
    glUniform1iv(glGetUniformLocation(shader.GetProgram(), "tex"), 32, texID);

    GLuint textures[] = { (GLuint)texID[0], (GLuint)texID[1], (GLuint)texID[2]   };
    glBindTextures(textures[0], 3, textures);
    
    Batch batch;
    batch.Init();

    bool draging = false;
    float dt = 0.0f;
    float dragingSpeed = 50.0f;
    float time = 0.0f;
    Vec2 dragLen;

    while (!window.ShouldClose()) {
        window.Refresh();
    
        if (window.keys['R']) {
            guiWin = load();
        }
        Vec2 mousePos{(float)window.xMouse * resolution.x / windowSize.x, (float)(windowSize.y - window.yMouse) * (resolution.y / windowSize.y)};
        
        if (window.mouseL) {
            if (mousePos.x >= guiWin.vertexes[0].pos.x &&
                mousePos.x <= guiWin.vertexes[2].pos.x &&
                mousePos.y >= guiWin.vertexes[0].pos.y &&
                mousePos.y <= guiWin.vertexes[1].pos.y) {
                if (draging == false)
                    dragLen = Vec2{mousePos.x - guiWin.vertexes[0].pos.x, mousePos.y - guiWin.vertexes[0].pos.y};
                Vec2 delta = Vec2{
                    mousePos.x - dragLen.x - guiWin.vertexes[0].pos.x,
                    mousePos.y - dragLen.y - guiWin.vertexes[0].pos.y
                };
                delta.x *= (dragingSpeed * dt);
                delta.y *= (dragingSpeed * dt);
                
                for (int i = 0; i < guiWin.vertexes.size(); ++i) {
                    guiWin.vertexes[i].pos.x += delta.x;
                    guiWin.vertexes[i].pos.y += delta.y;
                }
                draging = true;
            }
            else
                draging = false;
        }
        else    
            draging = false;


        batch.Begin();
        clock_t t1 = clock();
        
/*
        batch.Push(guiWin.verts);
        for (int i = 0; i < guiWin.groups.size(); ++i) {
            batch.Push(guiWin.groups[i].verts);
            for (int j = 0; j < guiWin.groups[i].textLabels.size(); ++j) {
                batch.Push(guiWin.groups[i].textLabels[j].verts);
            }
        }
        for (int i = 0; i < guiWin.tabs.size(); ++i) {
            batch.Push(guiWin.tabs[i].verts);
            for (int j = 0; j < guiWin.tabs[i].groups.size(); ++j) {
                batch.Push(guiWin.tabs[i].groups[j].verts);
                for (int k = 0; k < guiWin.tabs[i].groups[j].textLabels.size(); ++k) {
                    batch.Push(guiWin.tabs[i].groups[j].textLabels[k].verts);
                }
            }
        }
*/
        for (int i = 0; i < guiWin.vertexes.size(); i += 4) {
            batch.Push(&guiWin.vertexes[i]);
        }

        ////////////////////////
        clock_t t2 = clock();
        //Log("Fill buffer: " << t2 - t1);
        //Log("Fill ms: " << (float)(t2-t1) / CLOCKS_PER_SEC);
        batch.Flush();
   
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Log("Draw call: " << clock() - t2);
        window.Update();
        
        dt = glfwGetTime() - time;
        time = glfwGetTime();
    }
}



