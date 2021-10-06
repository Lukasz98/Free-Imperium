#pragma once

#include <vector>
#include "gui_batch.h"
#include "log.h"

struct TextLabel10 {
    Vertex verts[4];
    //std::vector<std::string> text;
    //std::string text;
    int id, firstCharId, lastCharId;
};
struct Group {
    Vertex verts[4];
    std::vector<TextLabel10> textLabels;
    int id, firstTextLabel10Id, lastTextLabel10Id;
};

struct Tab {
    Vertex verts[4];
    std::vector<Group> groups;

    int id, firstGrpId, lastGrpId;
};

struct Win {
    Vertex verts[4];
    std::vector<Tab> tabs;
    std::vector<Group> groups;


    std::vector<Vertex> vertexes;
};

namespace GuiPreview {


void Play(Window & window, glm::vec2 resolution, glm::vec2 windowSize);


}
