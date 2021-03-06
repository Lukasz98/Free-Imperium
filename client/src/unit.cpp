#include "unit.h"

Unit::Unit(int id, std::string name, glm::vec3 pos, int soldiers, std::string country)
    : Rectangle(pos, glm::vec2(10*4, 15*4))
{
    this->name = name;
    this->soldiers = soldiers;
    this->id = id;
    this->country = country;
    dt = 0.0f;
    activeFrame = 0;
    fakePos = pos;
    visible = false;

    DataObj * obj = new DataObj{"label"};
    obj->values["position:"] = "0.0 0.0 0.1";
    obj->values["size:"] = "90 30";
    //obj->values["valueName:"] = "unitSize";
    obj->values["bgColor:"] = "255 255 255 255";
    obj->values["contentAlign:"] = "center";

    obj->objects.push_back(new DataObj{"text"});
    obj->objects.back()->values["color"] = "255 20 20 255";
    obj->objects.back()->values["valueName:"] = "unitSize";
    obj->objects.back()->values["content:"] = itos(soldiers);
    //obj->objects.back()->values["height:"] = "10";
    obj->objects.back()->values["height:"] = "20";

    std::vector<GuiClick> clickPatterns;
    bar = std::make_unique<Label>(glm::vec3{pos.x - 25, pos.y - 25, pos.z}, obj->values, obj->objects, clickPatterns);

    delete obj;    
    
    texture[0] = std::make_shared<Texture>(std::string("src/img/unit_1.png"), 40, 40);
    texture[1] = std::make_shared<Texture>(std::string("src/img/unit_2.png"), 40, 40);
    move_texture[0] = std::make_shared<Texture>(std::string("src/img/unit_move_1.png"), 40, 40);
    move_texture[1] = std::make_shared<Texture>(std::string("src/img/unit_move_2.png"), 40, 40);
    battle_texture[0] = std::make_shared<Texture>(std::string("src/img/unit_battle_1.png"), 40, 40);
    battle_texture[1] = std::make_shared<Texture>(std::string("src/img/unit_battle_2.png"), 40, 40);
    SetTexture(texture[0]);
}
    
Unit::~Unit()
{
}

void Unit::Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount)
{
    visible = true;
    if (soldiers != soldiersCount) {
        soldiers = soldiersCount;
        std::unordered_map<std::string,std::string> barValues;
        barValues["unitSize"] = itos(soldiers);
        bar->Update(barValues);
    }
    if (p != position) {
        position = p;
        fakePos = p;
        Init();
        bar->SetPos(glm::vec3{position.x - 25, position.y - 25, position.z});
    }

    bool newMoves = false;
    for (auto & move : moves) {
        for (auto & nmv : mvs) {
            if (move.destiny != nmv) {
                newMoves = true;
                break;
            }
        }
    }

    if (moves.size() != mvs.size())
        newMoves = true;

    if (newMoves) {
        std::vector<Move> nMoves;
        for (auto & m : mvs) {
            Move move;
            move.destiny = m;
            nMoves.emplace_back(move);
        }
        AddMove(nMoves);
    }

    if (moves.size()) {
        if (moves[0].destiny.x < position.x)
            Left();
        else
            Right();
        Init();
    }
}

void Unit::AddMove(std::vector<Move> ms) // chyba moze byc prywatne
{
    for (std::size_t i = 0; i < ms.size(); i++) {
        float unitX = position.x;
        float unitY = position.y;
        if (i > 0) {
            unitX = ms[i - 1].destiny.x;
            unitY = ms[i - 1].destiny.y;
        }

        glm::vec3 ap1 = { unitX, unitY, 0.1 };
        glm::vec3 ap2 = { ms[i].destiny.x, ms[i].destiny.y, 0.1 };
        
        double rectWidth = 10.0;
        glm::vec3 ap3 = { ap1.x + rectWidth, ap1.y + rectWidth, 0.1 };
        glm::vec3 ap4 = { ap2.x + rectWidth, ap2.y + rectWidth, 0.1 };

        ms[i].arrow = std::make_shared<Arrow>(ap1, ap2, ap3, ap4);
    }
    moves = ms;
}

void Unit::Draw(bool isSelected)
{
    if (!visible)
        return;
    
    Rectangle::Draw();
    if (isSelected)
        for (auto & m : moves) {
            m.arrow->Draw();
        }

    bar->Draw();
}

void Unit::Kill(int amount)
{
    if (soldiers == 0)
        return;
    int m = amount / soldiers * 1000;
    if (m < 1 && amount > 0)
        m = 1;
        
    morale -= m;
    soldiers -= amount;
        
    if (soldiers < 0)
        soldiers = 0;

    if (morale < 0)
        morale = 0;

    notify(GetValues());
}

void Unit::Battle(bool is, bool attacker)
{
    isInBattle = is;
    if (is) {
        if (!attacker)
            Left();
        else
            Right();
        Init();
    }
    Init();
}

void Unit::UpdateDt(float d)
{
    dt += d;

    if (dt > 0.5f) {
        dt = 0.0f;
        activeFrame++;
        if (activeFrame > 1)
            activeFrame = 0;

        if (isInBattle)
            SetTexture(battle_texture[activeFrame]);
        else if (moves.size())
            SetTexture(move_texture[activeFrame]);
        else
            SetTexture(texture[activeFrame]);
    }
}

std::unordered_map<std::string,std::string> Unit::GetValues() const
{
    std::unordered_map<std::string,std::string> values;
    values["id"] = std::to_string(id);
    values["country"] = country;
    values["name"] = name;
    values["size"] = itos(soldiers);
    values["siegePower"] = std::to_string(siegePower);
    values["attack"] = std::to_string(attack);
    values["defense"] = std::to_string(defense);
    return values;
}
