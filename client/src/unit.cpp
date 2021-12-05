#include "unit.h"
#include "gui/num_to_string.h"
Unit::Unit()
    :rotateX(60.0f*3.1459265f/180.0f), yScale(10.0f)
{

}

/*
Unit(int id, std::string name, glm::vec3 pos, int soldiers, int ctrId, const std::vector<glm::vec3>& moves)
    : Rectangle(pos, glm::vec2(10*4, 15*4)), countryId(ctrId), provId(provId),
      rotateX(60.0f*3.1459265f/180.0f), yScale(10.0f)
{
    rotate = glm::mat4{1.0f};
    //model;
    this->name = name;
    this->soldiers = soldiers;
    this->id = id;
    this->country = country;
    dt = 0.0f;
    activeFrame = 0;
    fakePos = pos;
    visible = false;

}

Unit::Unit(int id, std::string name, glm::vec3 pos, int soldiers, std::string country, Color color, int ctrId, int provId)
    : Rectangle(pos, glm::vec2(10*4, 15*4)), countryId(ctrId), provId(provId),
      rotateX(60.0f*3.1459265f/180.0f), yScale(10.0f)
{
    rotate = glm::mat4{1.0f};
    //model;
    this->name = name;
    this->soldiers = soldiers;
    this->id = id;
    this->country = country;
    dt = 0.0f;
    activeFrame = 0;
    fakePos = pos;
    visible = false;
    //texture[0] = std::make_shared<Texture>(std::string("src/img/unit_1.png"), 40, 40);
    //texture[1] = std::make_shared<Texture>(std::string("src/img/unit_2.png"), 40, 40);
    //move_texture[0] = std::make_shared<Texture>(std::string("src/img/unit_move_1.png"), 40, 40);
    //move_texture[1] = std::make_shared<Texture>(std::string("src/img/unit_move_2.png"), 40, 40);
    //battle_texture[0] = std::make_shared<Texture>(std::string("src/img/unit_battle_1.png"), 40, 40);
    //battle_texture[1] = std::make_shared<Texture>(std::string("src/img/unit_battle_2.png"), 40, 40);
    //SetTexture(texture[0]);
}
*/    
Unit::~Unit()
{
}

void Unit::Update(glm::vec3 p, std::vector<glm::vec3> mvs, int soldiersCount)
{
    visible = true;
    if (soldiers != soldiersCount) {
        soldiers = soldiersCount;
        
        std::vector<GuiStruct> vals;
        vals.push_back({1, 5, itos(soldiers)});

        subject.Notify(vals);


    // std::unordered_map<std::string,std::string> barValues;
     //   barValues["unitSize"] = itos(soldiers);
      //  bar->Update(barValues);
    }
    if (p != position) {
        position = p;
        fakePos = p;
        //Init();
    //    bar->SetPos(glm::vec3{position.x - labelXOffset, position.y - labelYOffset, position.z});
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
//        if (moves[0].destiny.x < position.x)
 //           Left();
   //     else
     //       Right();
       // Init();
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

        glm::vec3 ap1 = { unitX, unitY, 10.0 };
        glm::vec3 ap2 = { ms[i].destiny.x, ms[i].destiny.y, 10.0 };
        
        double rectWidth = 10.0;
        glm::vec3 ap3 = { ap1.x + rectWidth, ap1.y + rectWidth, 10.0 };
        glm::vec3 ap4 = { ap2.x + rectWidth, ap2.y + rectWidth, 10.0 };

        ms[i].arrow = std::make_shared<Arrow>(ap1, ap2, ap3, ap4);
    }
    moves = ms;
}

void Unit::Draw(glm::mat4 matrix, bool isSelected)
{
    if (!visible)
        return;
    
//glm::mat4 model = glm::mat4(1.0);
model = glm::mat4(1.0);
model = glm::translate(model, position);
//glm::mat4 rotat = glm::rotate(glm::mat4{1.0}, 20.0f, glm::vec3{1.0, 0.0, 0.0}); 

this->model = model;
rotate = glm::rotate(glm::mat4{1.0}, rotateX, glm::vec3{1.0, 0.0, 0.0}); 
//rotate = glm::rotate(glm::mat4{1.0}, 20.0f, glm::vec3{1.0, 0.0, 0.0}); 
model = model * rotate;
model = glm::scale(model, glm::vec3{20.0, yScale, 10.0});
//model = glm::rotate(model, 0.0f, glm::vec3{1.0, 0.0, 0.0});
//model = glm::rotate(model, 20.0f, glm::vec3{1.0, 0.0, 0.0});


//glUseProgram(AM::am.shader->GetProgram());
//glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
//glUniformMatrix4fv(glGetUniformLocation(AM::am.shader->GetProgram(), "ml"), 1, GL_FALSE, glm::value_ptr(model));
//
//            glUniform1i(glGetUniformLocation(AM::am.shader->GetProgram(), "tex"),
//                          AM::am.modelTexture->GetId());
//
//            glActiveTexture(GL_TEXTURE0);
//AM::am.modelTexture->Bind();
//
////AM::am.model->DrawRect(model);
//AM::am.model->Draw();
//    //Rectangle::Draw();
//    if (isSelected)
//        for (auto & m : moves) {
//            //m.arrow->Draw();
//        }
//
//    //bar->Draw();
}

void Unit::DrawGuiElements(bool isSelected)
{

    if (isSelected) {
        for (auto & m : moves) {
            //Log(m.arrow->GetSize().x);
            m.arrow->Draw(false);
        }
    }
    //bar->Draw();
}

bool Unit::Click(glm::vec3 vv, glm::vec3 eye) {
//bool Click(int x, int y) { 
    if (!visible) return false; 
    else {
        //return AM::am.model->Click(model, rotate, vv, eye, yScale, rotateX);
        return false;
    //return false;//Rectangle::Click(x, y); 
    }
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

    //notify(GetValues());
}

void Unit::Battle(bool is, bool attacker)
{
    isInBattle = is;
    if (is) {
       // if (!attacker)
       //     Left();
       // else
       //     Right();
       // Init();
    }
    //Init();
}

void Unit::UpdateDt(float d)
{
    dt += d;

    if (dt > 0.5f) {
        dt = 0.0f;
        activeFrame++;
        if (activeFrame > 1)
            activeFrame = 0;

     //   if (isInBattle)
     //       SetTexture(battle_texture[activeFrame]);
     //   else if (moves.size())
     //       SetTexture(move_texture[activeFrame]);
     //   else
     //       SetTexture(texture[activeFrame]);
    }
}

std::unordered_map<std::string,std::string> Unit::GetValues() const
{
    std::unordered_map<std::string,std::string> values;
    values["id"] = std::to_string(id);
    values["country"] = country;
    values["name"] = name;
    //values["size"] = itos(soldiers);
    values["siegePower"] = std::to_string(siegePower);
    values["attack"] = std::to_string(attack);
    values["defense"] = std::to_string(defense);
    return values;
}

void Unit::UpdateGuiWin()
{
    std::vector<GuiStruct> vals;
    vals.push_back({1, 1, name});
    vals.push_back({1, 2, country, countryId});
    vals.push_back({1, 5, itos(soldiers)});
    vals.push_back({2, 5, itos(morale)});
    vals.push_back({3, 5, itos(discipline)});

    subject.Notify(vals);
}
