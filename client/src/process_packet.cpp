#include "process_packet.h"

void ProcessPacket::DailyUpdate(sf::Packet & packet, Gui & gui, std::vector<War> & wars, std::vector<std::unique_ptr<Province>> & provinces, 
                                std::vector<std::shared_ptr<Country>> & countries, Map & map)
{
    std::unordered_map<std::string, std::string> values;
    std::string strData;
    float floatData = 0;
    int intData = 0;
    int countryCounter = 0;
    packet >> countryCounter;

    for (int i = 0; i < countryCounter; i++) {
        packet >> strData;
        values["countryName"] = strData;

        packet >> floatData;
        values["countryGold"] = ftos(floatData);

        packet >> floatData;
        values["countryIncome"] = ftos(floatData);
    
        packet >> floatData;
        values["armyMaintenance"] = ftos(floatData);

        packet >> intData;
        values["countryManpower"] = itos(intData);

        packet >> intData;
        values["countryManpowerRecovery"] = itos(intData);

        packet >> strData;
        values["date"] = strData;
    
        packet >> intData;
        values["dateSpeed"] = std::to_string(intData);
    
        gui.Update(values, "topBar");

        int provCount;
        packet >> provCount;
        for (int i = 0; i < provCount; i++) {
            int id, manpower, development;
            int population;
            float treasury;
            float monthIncome, totalMonthIncome;
        
            packet >> id;
            packet >> manpower;
            packet >> development;
            packet >> treasury;
            packet >> monthIncome;
            packet >> totalMonthIncome;
            
            assert(id >= 0 && id < provinces.size());
            auto & prov = provinces.at(id);
            //auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const std::unique_ptr<Province> & prov){
            //                                                                   return prov->GetId() == id;
            //                                                               });
            //if (provIt != provinces.end()) {
                prov->SetManpower(manpower);
                prov->SetDevelopment(development);
                prov->SetTreasury(treasury);
                prov->SetMonthIncome(monthIncome);
                prov->SetTotalMonthIncome(totalMonthIncome);

            //}

        
        }
    
        int warsCount = 0;
        packet >> warsCount;
        for (int i = 0; i < warsCount; i++) {
            int id;
            packet >> id;
            int score;
            packet >> score;

            int attackersCount, defendersCount;
            std::vector<int> attackers, defenders;
            packet >> attackersCount;
            for (int att = 0; att < attackersCount; att++) {
                int aId;
                packet >> aId;
                attackers.push_back(aId);//countries.at(aId)->GetName(), aId);
            }
            packet >> defendersCount;
            for (int def = 0; def < defendersCount; def++) {
                int dId;
                packet >> dId;
                defenders.push_back(dId);//countries.at(dId)->GetName(), dId);
            }
            
            for (auto & war : wars) {
                if (war.GetId() == id) {
                    war.SetWarScore(score);
                    break;
                }
            }
        }

        int siegedProvincesCount;
        packet >> siegedProvincesCount;

        for (int i = 0; i < siegedProvincesCount; i++) {
            int provId, siegeCountryId;
            int sieged, siegeSoldiers;
            packet >> provId;
            packet >> sieged;
            packet >> siegeCountryId;
            packet >> siegeSoldiers;
            assert(provId >= 0 && provId < provinces.size());
            auto & prov = provinces.at(provId);
            //auto provIt = std::find_if(provinces.begin(), provinces.end(), [provName](const std::unique_ptr<Province> & prov){
            //               return prov->GetName() == provName;
            //              });
            //if (provIt != provinces.end()) {
                    assert(siegeCountryId >= 0 && siegeCountryId < countries.size());
                    auto sctr = countries.at(siegeCountryId);
                if (sieged == 100 && prov->GetSieged() != 100) {
                    //auto scIt = std::find_if(countries.begin(), countries.end(), [siegeCountry](std::shared_ptr<Country> & ccc) {
                    //             return ccc->GetName() == siegeCountry;
                    //            });
                    //if (scIt != countries.end())
                        map.DrawSieged(prov->GetColor(), sctr->GetColor());
                }
                prov->Sieging(sctr->GetName(), siegeCountryId, sieged, siegeSoldiers);
            //}/
        }

        for (auto & prov : provinces) {
            if (!prov->WasSiegeUpdated()) {
                if (prov->GetSieged() != 0) {
                    assert(prov->GetCountryId() >= 0 && prov->GetCountryId() < countries.size());
                    auto sctr = countries.at(prov->GetCountryId());
                    //auto scIt = std::find_if(countries.begin(), countries.end(), [cccc = prov->GetCountry()](std::shared_ptr<Country> & ccc) {
                    //             return ccc->GetName() == cccc;
                    //            });
                    //if (scIt != countries.end())
                    map.ScrapSiegeColor(prov->GetColor());
                    //map.DrawSieged(prov->GetColor(), sctr->GetColor());
                }
                prov->ResetSieging();
            }
        }
    }
    
}

void ProcessPacket::HourlyUpdate(sf::Packet & packet, Gui & gui, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles, float mapChunkScale)
{
    int unitCount;
    packet >> unitCount;
    for (int i = 0; i < unitCount; i++) {
        int id, soldiersCount;
        glm::vec3 uPos;
        packet >> id;
        packet >> soldiersCount;
        packet >> uPos.x;
        packet >> uPos.y;
        packet >> uPos.z;
       	uPos.x *= mapChunkScale;
    	uPos.y *= mapChunkScale;
        uPos.z = 10.0f;

        int movesCount = 0;
        packet >> movesCount;

        std::vector<glm::vec3> moves;
        for (int i = 0; i < movesCount; i++) {
            glm::vec3 v;
            packet >> v.x;
            packet >> v.y;
            packet >> v.z;
    	    v.x*=2;
    	    v.y*=2;
            moves.push_back(v);
        }

        auto uIt = std::find_if(units.begin(), units.end(), [id](const std::shared_ptr<Unit> u) {
                return u->GetId() == id;
            });
        if (uIt != units.end()) {            
            (*uIt)->Update(uPos, moves, soldiersCount);
        }
    }

    int battlesCount;
    packet >> battlesCount;
    for (int i = 0; i < battlesCount; i++) {
        int id, attSize, defSize;
        packet >> id;
        packet >> attSize;
        packet >> defSize;
        for (auto & battle : battles) {
            if (battle->GetId() == id) {
                battle->SetAttackersSize(attSize);
                battle->SetDefendersSize(defSize);
                break;
            }
        }
    }
}

void ProcessPacket::PeaceAccepted(sf::Packet & packet, Gui & gui, std::vector<std::unique_ptr<Province>> & provinces, 
                        std::vector<std::shared_ptr<Country>> & countries, std::vector<War> & wars, Map & map)
{
    int provinceCount;
    packet >> provinceCount;
        
    for (int i = 0; i < provinceCount; i++) {
        int provId, ctrId;
        packet >> provId;
        packet >> ctrId;
        assert(ctrId >= 0 && ctrId < countries.size());
        auto ctr = countries.at(ctrId);
        assert(provId >= 0 && provId < provinces.size());
        auto & prov = provinces.at(provId);
        prov->SetOwner(ctr->GetName(), ctrId);
        map.ScrapSiegeColor(prov->GetColor());
        //auto provIt = std::find_if(provinces.begin(), provinces.end(), [name](const std::unique_ptr<Province> & prov){
        //        return prov->GetName() == name;
        //    });
        //if (provIt != provinces.end()) {
          //  (*provIt)->SetOwner(ctr);
            //setCountryMap();
        //}
    }

    int warId;
    packet >> warId;

    auto warIt = std::find_if(wars.begin(), wars.end(), [warId](const War & war) {
            return war.GetId() == warId;
        });
    if (warIt == wars.end())
        return;

    std::string peaceType;
    packet >> peaceType;
    if (peaceType == "endWar") {
        gui.EraseObj("notifications", warIt->GetIdInGui());
        wars.erase(warIt);
    }
    else {            
        int ctr;
        packet >> ctr;
        (*warIt).Erase(ctr);
    }
}

void ProcessPacket::NewWar(sf::Packet & packet, Gui & gui, std::vector<War> & wars, int myCountryId, std::vector<std::shared_ptr<Country>> & countries)
{
    int id;
    int attackerId, defenderId;

    packet >> id;
    packet >> attackerId;
    packet >> defenderId;

    if (myCountryId == attackerId || myCountryId == defenderId) {
        assert(attackerId >= 0 && attackerId < countries.size());
        auto attacker = countries.at(attackerId);
        assert(defenderId >= 0 && defenderId < countries.size());
        auto defender = countries.at(defenderId);
        DataObj * obj = new DataObj{"label"};
        obj->values["position:"] = "0.0 0.0 0.1";
        obj->values["bgColor:"] = "0 0 0 0";
        obj->values["size:"] = "50 50";
        
        DataObj * hoverLabel = new DataObj{"label"};
        hoverLabel->values["position:"] = "0.0 0.0 -0.1";
        hoverLabel->values["bgColor:"] = "0 0 0 0";
        hoverLabel->values["size:"] = "50 50";
        hoverLabel->values["hover:"] = "true";
        
        DataObj * popUpLabel = new DataObj{"label"};
        popUpLabel->values["position:"] = "-200.0 3.0 0.1";
        popUpLabel->values["bgColor:"] = "50 15 15 200";
        popUpLabel->values["size:"] = "205 44";
        popUpLabel->values["contentAlign:"] = "center";

        DataObj * icon = new DataObj{"icon"};
        icon->values["position:"] = "0.0 0.0 0.1";
        icon->values["clickEvent:"] = "openWar";
        icon->values["size:"] = "50 50";
        icon->values["texturePath:"] = "src/img/war.png";
        icon->values["impact:"] = std::to_string(id);
        
        DataObj * hoverText = new DataObj{"text"};
        hoverText->values["position:"] = "0.0 0.0 0.1";
        hoverText->values["bgColor:"] = "0 0 0 0";
        hoverText->values["height:"] = "25";
        if (attackerId != myCountryId) {
            hoverText->values["content:"] = attacker->GetName();
            icon->values["content:"] = attacker->GetName();
        }
        else {
            hoverText->values["content:"] = defender->GetName();
            icon->values["content:"] = defender->GetName();
        }
        
        popUpLabel->objects.push_back(hoverText);
        hoverLabel->objects.push_back(popUpLabel);
        obj->objects.push_back(hoverLabel);
        obj->objects.push_back(icon);
        
        int idInGui = gui.AddToList(obj, "notifications", "notificationsList");
        delete obj;

        War war{id, idInGui};
        war.AddAttacker(attacker->GetName(), attackerId);
        war.AddDefender(defender->GetName(), defenderId);
        wars.emplace_back(war);

    }
}

void ProcessPacket::NewUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::shared_ptr<Country>> & countries, 
                            const std::string & countryName)
{
    int id, unitSize, morale, provId;
    std::string name = "cyce";
    int countryId;
    glm::vec3 pos;
    packet >> id;
    packet >> name;
    packet >> countryId;
    packet >> morale;
    packet >> unitSize;
    packet >> pos.x;
    packet >> pos.y;
    packet >> pos.z;
    packet >> provId;
    pos.x*=4;
    pos.y*=4;

    assert(countryId >= 0 && countryId < countries.size());
    auto country = countries.at(countryId);
    Color color = country->GetColor();
    //auto it = std::find_if(countries.begin(), countries.end(), [cn = country](std::shared_ptr<Country> & c){
    //    return c->GetName() == cn;
    //});
    //if (it != countries.end())
      //  color = (*it)->GetColor();

    units.push_back(std::make_shared<Unit>(id, name, pos, unitSize, country->GetName(), color, countryId, provId));

    //if (country == countryName)
    //    units.back()->Visible(true);
}

void ProcessPacket::NewBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles,
                             std::vector<std::unique_ptr<Province>> & provinces)
{
    int id, warId, attackerUnitId, defenderUnitId;
    glm::vec3 pos;
    int provId;
          
    packet >> id;
    packet >> warId;
    packet >> pos.x;
    packet >> pos.y;
    packet >> pos.z;
    packet >> provId;
    packet >> attackerUnitId;
    packet >> defenderUnitId;

    assert(provId >= 0 && provId < provinces.size());
    auto & prov = provinces.at(provId);    
    Battle battle{id, warId, pos, prov->GetName(), provId};
    for (auto & u : units) {
        if (u->GetId() == attackerUnitId) {
            battle.AddAttacker(u);
            break;
        }
    }
    for (auto & u : units) {
        if (u->GetId() == defenderUnitId) {
            battle.AddDefender(u);
            break;
        }
    }
    battles.emplace_back(std::make_unique<Battle>(battle));    
}

void ProcessPacket::EndBattle(sf::Packet & packet, std::vector<std::unique_ptr<Battle>> & battles)
{
    int id;
    packet >> id;
    for (int i = 0; i < battles.size(); i++) {
        if (battles[i]->GetId() == id) {
            battles[i]->EndBattle();
            battles.erase(battles.begin() + i);
            break;
        }
    }    
}

void ProcessPacket::DeleteUnit(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units)
{
    int id;
    packet >> id;
    for (int i = 0; i < units.size(); i++) {
        if (units[i]->GetId() == id) {
            units.erase(units.begin() + i);
            break;
        }
    }
}

void ProcessPacket::NewUnitInBattle(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units, std::vector<std::unique_ptr<Battle>> & battles)
{
    int battleId, unitId;
    bool attacker;
    packet >> battleId;
    packet >> unitId;
    packet >> attacker;
    for (auto & battle : battles) {
        if (battle->GetId() == battleId) {
            for (auto & unit : units) {
                if (unit->GetId() == unitId) {
                    if (attacker)
                        battle->AddAttacker(unit);
                    else
                        battle->AddDefender(unit);
                    break;
                }
            }
            break;
        }
    }
}

void ProcessPacket::MergeUnits(sf::Packet & packet, std::vector<std::shared_ptr<Unit>> & units)
{
    int baseUnitId, unitsCount;
    packet >> baseUnitId;
    packet >> unitsCount;

    auto unitBaseIt = std::find_if(units.begin(), units.end(), [baseUnitId](const std::shared_ptr<Unit> & unit) {
            return unit->GetId() == baseUnitId;
        });
    if (unitBaseIt != units.end()) {
        for (int i = 0; i < unitsCount; i++) {
            int unitId;
            packet >> unitId;
            auto unitIt = std::find_if(units.begin(), units.end(), [unitId](const std::shared_ptr<Unit> & unit) {
                    return unit->GetId() == unitId;
                });
            if (unitIt != units.end()) {
                units.erase(unitIt);
            }
        }
    }
}

void ProcessPacket::ProvincePopulation(sf::Packet & packet, std::vector<std::unique_ptr<Province>> & provinces)
{
    int id, population;
    packet >> id;
    packet >> population;
    auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const std::unique_ptr<Province> & prov){
            return id == prov->GetId();
        });
    if (provIt != provinces.end()) {
        (*provIt)->SetPopulation(population);
    }
}

void ProcessPacket::SetGold(sf::Packet & packet, Gui & gui, std::shared_ptr<Country> & country)
{
    float gold = 0.0f;
    packet >> gold;

    country->SetGold(gold);

    std::unordered_map<std::string,std::string> values;
    values["countryGold"] = ftos(gold);
    gui.Update(values, "topBar");
}

void ProcessPacket::SetSpeed(sf::Packet & packet, Gui & gui)
{
    int speed = 1;
    packet >> speed;
    std::unordered_map<std::string,std::string> values;
    values["dateSpeed"] = std::to_string(speed);
    gui.Update(values, "topBar");
}


void ProcessPacket::MonthlyUpdate(sf::Packet & packet, const std::string & myCountry, std::vector<std::shared_ptr<Country>> & countries)
{
    int countC = 0;
    packet >> countC;

    for (int i = 0; i < countC; i++) {
        std::string cName;
        packet >> cName;

        auto cIt = std::find_if(countries.begin(), countries.end(), [cName](std::shared_ptr<Country> & c) { return c->GetName() == cName; });
        
        int relC = 0;
        packet >> relC;
        for (int r = 0; r < relC; r++) {
            std::string rcn;
            packet >> rcn;
            int rcr = 0;
            packet >> rcr;

            if (cIt != countries.end())
                (*cIt)->SetRel(rcn, rcr);
        }
    }

    int countImprRel = 0;
    packet >> countImprRel;
    std::vector<std::string> imprRel;
    for (int i = 0; i < countImprRel; i++) {
        std::string s;
        packet >> s;
        imprRel.emplace_back(s);
    }

    auto cIt = std::find_if(countries.begin(), countries.end(), [myCountry](std::shared_ptr<Country> & c) { return c->GetName() == myCountry; });
    if (cIt != countries.end()) {
        (*cIt)->SetImprRel(imprRel);
    }
}

void ProcessPacket::BotPeaceOffer(sf::Packet & packet, Gui & gui, std::vector<PeaceOffer> & peaceOffers, const std::vector<std::shared_ptr<Country>> & countries)
{
    PeaceOffer peaceOffer;
    int lostProvCount, gainProvCount;
    
    packet >> peaceOffer.peaceId;
    packet >> peaceOffer.warId;
    packet >> peaceOffer.offeredBy;
    
    packet >> lostProvCount;
    for (int i = 0; i < lostProvCount; i++) {
        //std::string s, ss;
        int s, ss;
        int d;
        packet >> s;
        packet >> ss;
        packet >> d;
        peaceOffer.lostProv.emplace_back(std::make_tuple(s, ss, d));
    }

    packet >> gainProvCount;
    for (int i = 0; i < gainProvCount; i++) {
        //std::string s, ss;
        int s, ss;
        int d;
        packet >> s;
        packet >> ss;
        packet >> d;
        peaceOffer.gainProv.emplace_back(std::make_tuple(s, ss, d));
    }

    DataObj * obj = new DataObj{"label"};
    obj->values["position:"] = "0.0 0.0 0.1";
    obj->values["bgColor:"] = "0 0 0 0";
    obj->values["size:"] = "50 50";
        
    DataObj * hoverLabel = new DataObj{"label"};
    hoverLabel->values["position:"] = "0.0 0.0 -0.1";
    hoverLabel->values["bgColor:"] = "0 0 0 0";
    hoverLabel->values["size:"] = "50 50";
    hoverLabel->values["hover:"] = "true";
        
    DataObj * popUpLabel = new DataObj{"label"};
    popUpLabel->values["position:"] = "-200.0 3.0 0.1";
    popUpLabel->values["bgColor:"] = "50 15 15 200";
    popUpLabel->values["size:"] = "205 44";
    popUpLabel->values["contentAlign:"] = "center";

    DataObj * icon = new DataObj{"icon"};
    icon->values["position:"] = "0.0 0.0 0.1";
    icon->values["clickEvent:"] = "botPeaceOffer";
    icon->values["size:"] = "50 50";
    icon->values["texturePath:"] = "src/img/peace.png";
    icon->values["impact:"] = std::to_string(peaceOffer.peaceId);
    //Log(peaceOffer.peaceId);
    //Log(std::to_string(peaceOffer.peaceId));
    //icon->values["valueName:"] = "warId";
        
    DataObj * hoverText = new DataObj{"text"};
    hoverText->values["position:"] = "0.0 0.0 0.1";
    hoverText->values["bgColor:"] = "0 0 0 0";
    //hoverText->values["valueName:"] = "countryName";
    hoverText->values["height:"] = "25";
    assert(peaceOffer.offeredBy >= 0 && peaceOffer.offeredBy < countries.size());
    hoverText->values["content:"] = countries.at(peaceOffer.offeredBy)->GetName();//std::to_string(peaceOffer.offeredBy);
    icon->values["content:"] = std::to_string(peaceOffer.offeredBy);
        
    popUpLabel->objects.push_back(hoverText);
    hoverLabel->objects.push_back(popUpLabel);
    obj->objects.push_back(hoverLabel);
    obj->objects.push_back(icon);
        
    int idInGui = gui.AddToList(obj, "notifications", "notificationsList");

    peaceOffer.idInGui = idInGui;
    peaceOffers.push_back(peaceOffer);

    delete obj;

    
}

void ProcessPacket::PeaceDeclined(sf::Packet & packet, Gui & gui)
{
    std::string recipant;
    packet >> recipant;

    DataObj * obj = new DataObj{"label"};
    obj->values["position:"] = "0.0 0.0 0.1";
    obj->values["bgColor:"] = "0 0 0 0";
    obj->values["size:"] = "50 50";
        
    DataObj * hoverLabel = new DataObj{"label"};
    hoverLabel->values["position:"] = "0.0 0.0 -0.1";
    hoverLabel->values["bgColor:"] = "0 0 0 0";
    hoverLabel->values["size:"] = "50 50";
    hoverLabel->values["hover:"] = "true";
        
    DataObj * popUpLabel = new DataObj{"label"};
    popUpLabel->values["position:"] = "-200.0 3.0 0.1";
    popUpLabel->values["bgColor:"] = "50 15 15 200";
    popUpLabel->values["size:"] = "205 44";
    popUpLabel->values["contentAlign:"] = "center";

    DataObj * icon = new DataObj{"icon"};
    icon->values["position:"] = "0.0 0.0 0.1";
    icon->values["clickEvent:"] = "eraseObj";
    icon->values["size:"] = "50 50";
    icon->values["texturePath:"] = "src/img/peace_declined.png";
        
    DataObj * hoverText = new DataObj{"text"};
    hoverText->values["position:"] = "0.0 0.0 0.1";
    hoverText->values["bgColor:"] = "0 0 0 0";
    hoverText->values["height:"] = "25";
    hoverText->values["content:"] = recipant;
        
    popUpLabel->objects.push_back(hoverText);
    hoverLabel->objects.push_back(popUpLabel);
    obj->objects.push_back(hoverLabel);
    obj->objects.push_back(icon);

    int idInGui = gui.AddToList(obj, "notifications", "notificationsList");
    delete obj;
}

