#include "gui_aid.h"

//void GuiAid::DecreaseValue()
void GuiAid::DecreaseValue()
{
    std::unordered_map<std::string,std::string> values;
    //int newValue = std::stoi(event.values["newUnitSize:"]) - 100;
    //values["newUnitSize"] = std::to_string(newValue);
    //gui.Update(values, event.values["windowType:"]);
}

//void GuiAid::IncreaseValue()
void GuiAid::IncreaseValue()
{
    std::unordered_map<std::string,std::string> values;
    //int newValue = std::stoi(event.values["newUnitSize:"]) + 100;
    //values["newUnitSize"] = std::to_string(newValue);
    //gui.Update(values, event.values["windowType:"]);
}

void GuiAid::OpenCountry(const std::vector<std::shared_ptr<Country>> & countries, const std::shared_ptr<Country> & myCountry, 
                 const std::vector<War> & wars)
{
/*
    std::string countryName = "";

    for (auto & e : event.values) {
        if (e.first == "countryName:") {
            countryName = e.second;
            break;
        }
    }

    if (countryName == "") {
        auto find = event.values.find("content:");
        if (find != event.values.end() && find->second != "")
            countryName = find->second;
            //countryName = event.values["content:"];
    }
        
    for (auto & c : countries) {
        if (c->GetName() != countryName)
            continue;
        bool atWar = false;
        for (auto & war : wars) {
            if (war.ShouldTheyFight(c->GetId(), myCountry->GetId())) {
                atWar = true;
                break;
            }
        }

        std::unordered_map<std::string,std::string> values;
        values["countryName"] = countryName;

        if (myCountry->GetName() != c->GetName()) {
            values["myOpinion"] = std::to_string(myCountry->GetRel(c->GetName()));
            values["hisOpinion"] = std::to_string(c->GetRel(myCountry->GetName()));
        }
                
        //gui.AddWin("src/gui/country.txt");
        //gui.Update(values, "country");


        if (c->GetId() != myCountry->GetId()) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["valueName:"] = "diploOption";
            if (atWar) {
                dobj->objects.back()->values["content:"] = "Offer peace";
                dobj->objects.back()->values["clickEvent:"] = "offerPeace";
            }
            else {
                dobj->objects.back()->values["content:"] = "Declare war";
                dobj->objects.back()->values["clickEvent:"] = "declareWar";
            }
            //gui.AddToList(dobj, "country", "diplomatic");
            delete dobj;
        }
        if (c->GetId() != myCountry->GetId()) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["valueName:"] = "diploOption";
            if (myCountry->IsImprRel(countryName)) {
                dobj->objects.back()->values["content:"] = "Stop improving relations";
                dobj->objects.back()->values["clickEvent:"] = "stopImprRel";
            }
            else {
                dobj->objects.back()->values["content:"] = "Improve relations";
                dobj->objects.back()->values["clickEvent:"] = "startImprRel";
            }
            //gui.AddToList(dobj, "country", "diplomatic");
            delete dobj;
        }
        break;
    }
*/
}

void GuiAid::NewUnit(std::vector<std::unique_ptr<Province>> & provinces, std::vector<sf::Packet> & packets)
{
/*
    std::string country, prov, size;
    auto valIt = event.values.find("countryName:");
    if (valIt != event.values.end())
        country = valIt->second;

    valIt = event.values.find("provName:");
    if (valIt != event.values.end())
        prov = valIt->second;

    valIt = event.values.find("newUnitSize:");
    if (valIt != event.values.end())
        size = valIt->second;

    auto pIt = std::find_if(provinces.begin(), provinces.end(), [prov](const std::unique_ptr<Province> & p){
        return p->GetName() == prov;
    });

    packets.emplace_back(
       PreparePacket::NewUnit((*pIt)->GetCountryId(), (*pIt)->GetId(), std::stoi(size)) 
    );
*/
}

void GuiAid::OpenUnit(const std::vector<std::shared_ptr<Unit>> & units)
{
/*
std::stringstream uid_ss;
    uid_ss << event.values["unitName:"];
    std::string uid_s;
    uid_ss >> uid_s;
   
    for (auto & v : event.values) {
        Log(v.first << ",,,"<<v.second);
    }

    int unitId = 0;
    auto uIdIt = event.values.find("unitId");
    if (uIdIt == event.values.end()) {
        Log("DUPA");
        return;
    }

    unitId = std::stoi(uIdIt->second);

    for (auto & unit : units) {
        if (unit->GetId() == unitId) {
            auto values = unit->GetValues();
            //gui.AddWin("src/gui/unit.txt");
            //gui.Update(values, "unit");

            break;
        }
    }
    */
}

void GuiAid::EraseObj()
{

  //  for (auto & v : event.values)
    //    Log(v.first << ", " << v.second);
    //gui.EraseObj(event.values["windowType:"], std::stoi(event.values["id:"]));
}


void GuiAid::DeclareWar(std::vector<std::shared_ptr<Country>> & countries, std::vector<sf::Packet> & packets)
{
/*
    Log("WAR WAR");
 
    auto ctr = std::find_if(countries.begin(), countries.end(), [c = event.values["countryName:"]] (std::shared_ptr<Country>& ct) {
        return c == ct->GetName();
    });
    //std::string countryName = event.values["countryName:"];
    sf::Packet packet;
    packet << "DeclareWar"; 
    packet << (*ctr)->GetId();//countryName;
    packets.push_back(packet);
*/
}

void GuiAid::OpenWar(const std::vector<War> & wars)
{
  /*  std::unordered_map<std::string,std::string> values;
    values["warId"] = event.values["impact:"];

    if (values["warId"] == "")
        return;
    
    auto warIt = std::find_if(wars.begin(), wars.end(), [id = std::stoi(values["warId"])](const War & war) { return id == war.GetId(); });
    if (warIt == wars.end())
        return;
    
    values["attackerWarScore"] = std::to_string(warIt->GetAttackerWarScore());

    //gui.AddWin("src/gui/war.txt");
    //gui.Update(values, "war");

    auto defenders = warIt->GetDefenders();
    auto attackers = warIt->GetAttackers();
    for (auto & d : defenders) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        dobj->objects.back()->values["content:"] = d;
        dobj->objects.back()->values["clickEvent:"] = "openCountry";
        //gui.AddToList(dobj, "war", "defenders");
        delete dobj;
    }
    for (auto & a : attackers) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        dobj->objects.back()->values["content:"] = a;
        dobj->objects.back()->values["clickEvent:"] = "openCountry";
        //gui.AddToList(dobj, "war", "attackers");
        delete dobj;
    }*/
}


void GuiAid::OpenBattle(const std::vector<std::unique_ptr<Battle>>::iterator & battleIt, 
                        const std::vector<std::unique_ptr<Province>>::iterator & provIt)
{
/*
    auto values = (*battleIt)->GetValues();
    auto val2 = (*provIt)->GetValues();

    for (auto & v : val2) {
        values[v.first] = v.second;
    }

    //gui.AddWin("src/gui/battle.txt");
    //gui.Update(values, "battle");
    //gui.ObservationOfLastWin((*battleIt).get());
*/
}

void GuiAid::OpenSiegedProv(const std::vector<std::unique_ptr<Province>>::iterator & provIt, const std::vector<War> & wars)
{
/*    auto values = (*provIt)->GetValues();
    std::vector<std::string> possibleSiegers;

    //gui.AddWin("src/gui/sieged_province.txt");
    //gui.Update(values, "siegedProvince");
    //gui.ObservationOfLastWin((*provIt).get());

    for (auto & w : wars) {
        if (w.ShouldTheyFight((*provIt)->GetCountryId(), (*provIt)->GetSiegeCountryId())) {
        //if (w.ShouldTheyFight(values["countryName"], values["siegeCountry"])) {
            if (w.IsAttacker((*provIt)->GetSiegeCountryId()))
            //if (w.IsAttacker(values["siegeCountry"]))
                possibleSiegers = w.GetAttackers();
            else
                possibleSiegers = w.GetDefenders();
            break;
        }
    }

    for (auto & pS : possibleSiegers) {
        DataObj * obj = new DataObj{"label"};
        obj->objects.push_back(new DataObj{"text"});
        obj->objects.back()->values["clickEvent:"] = "setSieger";
        obj->objects.back()->values["valueName:"] = "sieger";
        obj->objects.back()->values["content:"] = pS;
        //gui.AddToList(obj, "siegedProvince", "siegeCandidates");
        delete obj;
    }        
*/
}

void GuiAid::OfferPeace(const std::vector<War> & wars, const std::string & myCountry)
{
/*    bool atWar = false;
    int warId = 0;
    int warScore = 0;
    for (auto & w : wars) {
        if (w.ShouldTheyFight(event.values["countryName:"], myCountry)) {
            atWar = true;
            warId = w.GetId();
            warScore = w.GetAttackerWarScore();
            break;
        }
    }
    if (!atWar)
        return;

    std::unordered_map<std::string,std::string> values;
    values["warId"] = std::to_string(warId);
    values["recipant"] = event.values["countryName:"];
    values["offeredBy"] = myCountry;
    values["warScore"] = std::to_string(warScore);
    //gui.AddWin("src/gui/offer_peace.txt");
    //gui.Update(values, "offerPeace");
*/}

void GuiAid::SendPeaceOffer(const std::vector<War> & wars, std::vector<std::shared_ptr<Country>> & countries,
                            std::vector<std::unique_ptr<Province>> & provinces, std::vector<sf::Packet> & packets)
{
  /*  std::vector<std::string> agreements = gui.GetContentOf("offerPeace", "agreement");
    //Log("AGREEMENTS: " << agreements);

    std::vector<std::pair<std::string,std::string>> provs;
    for (auto & agree : agreements) { 
        std::stringstream ss;
        ss << agree;
        std::string word;
        std::string firstVal, secVal;
        while (!ss.eof()) {
            std::string sss;
            ss >> sss;
            if (sss == "to") {
                word = sss;
            }
            else if (word.size() == 0) {
                if (firstVal.size())
                    firstVal += " " + sss;
                else
                    firstVal = sss;
            }
            else {
                if (secVal.size())
                    secVal += " " + sss;
                else
                    secVal = sss;
            }
        }
        provs.emplace_back(std::make_pair(firstVal, secVal));
    }

    std::vector<std::pair<int,int>> pcids;

    for (auto & prov : provs) {
        auto pIt = std::find_if(provinces.begin(), provinces.end(), [p = prov.first](std::unique_ptr<Province> & pr ) {
            return pr->GetName() == p;
        });
        auto cIt = std::find_if(countries.begin(), countries.end(), [p = prov.second](std::shared_ptr<Country> & c ) {
            return c->GetName() == p;
        });
        pcids.push_back(std::make_pair((*pIt)->GetId(), (*cIt)->GetId()));
    }

    auto recipant = std::find_if(countries.begin(), countries.end(), [p = event.values["recipant:"]] (std::shared_ptr<Country> & c ) {
        return c->GetName() == p;
    });
    

    sf::Packet packet;
    packet << "PeaceOffer";
    packet << std::stoi(event.values["warId:"]);
    packet << (*recipant)->GetId(); //event.values["recipant:"];
    packet << (int)provs.size();
    for (auto & prov : pcids) {
        packet << prov.first;
        packet << prov.second;
    }
    packets.emplace_back(packet);
*/}

void GuiAid::MergeUnits(std::vector<sf::Packet> & packets)
{
  /*  //Log("MERGE UNIT");
    sf::Packet packet;
    packet << "MergeUnits";

    //std::vector<std::string> content = gui.GetContentOf("units", "unitName");

    std::vector<int> ids;
    
    auto listVals = gui.GetValuesOfListContents("units", "units");
    for (auto & map : listVals) {
        //for (auto & el : map) {
            auto uIdIt = map.find("unitId");
            if (uIdIt == map.end())
                continue;
            ids.push_back(std::stoi(uIdIt->second));
            //Log(el.first<<"<<<"<<el.second);
        //}
    }
    

    packet << (int)ids.size();
    for (auto & id : ids)
        packet << id;

    packets.emplace_back(packet);
*/}

void GuiAid::DateSpeed(std::vector<sf::Packet> & packets)
{
  /*  sf::Packet packet;
    packet << "Speed";
    //Log(event.values["content:"]);
    if (event.values["content:"] == "faster")
        packet << true;
    else
        packet << false;
    packets.emplace_back(packet);
*/}

void GuiAid::StartGame(std::vector<sf::Packet> & packets)
{
  //  sf::Packet packet;
    //packet << "Start";
    //packets.emplace_back(packet);
}

void GuiAid::SetCountry(std::vector<sf::Packet> & packets)
{
    //sf::Packet packet;
    //packet << "country";
    //packet << click.values["content:"];
//    packets.emplace_back(packet);
}

void GuiAid::StartImprRel(std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries, 
                          std::vector<sf::Packet> & packets)
{
  /*  std::string country = event.values["countryName:"];
    sf::Packet packet;
    
    auto ctr = std::find_if(countries.begin(), countries.end(), [country] (std::shared_ptr<Country>& ct) {
        return ct->GetName() == country;
    });
    
    packet << "StartImprRel";
    packet << myCountry->GetId();
    packet << (*ctr)->GetId();
    packets.emplace_back(packet);
    myCountry->StartImprRel(country);
*/}

void GuiAid::StopImprRel(std::shared_ptr<Country> & myCountry, std::vector<std::shared_ptr<Country>> & countries,
                         std::vector<sf::Packet> & packets)
{
 /*   std::string country = event.values["countryName:"];
    sf::Packet packet;
    packet << "StopImprRel";
    
    auto ctr = std::find_if(countries.begin(), countries.end(), [country] (std::shared_ptr<Country>& ct) {
        return ct->GetName() == country;
    });
    
    packet << myCountry->GetId();
    packet << (*ctr)->GetId();
    packets.emplace_back(packet);
    myCountry->StopImprRel(country);
*/}

void GuiAid::BotPeaceOffer(std::vector<PeaceOffer> & peaceOffers, const std::vector<std::shared_ptr<Country>> & countries,
                    const std::vector<std::unique_ptr<Province>> & provinces)
{
  /*  //gui.AddWin("src/gui/bot_peace_offer.txt");
    std::unordered_map<std::string,std::string> values;
    values["peaceId"] = event.values["impact:"];
    //gui.Update(values, "botPeaceOffer");

    auto peaceIt = std::find_if(peaceOffers.begin(), peaceOffers.end(), [id = std::stoi(values["peaceId"])](PeaceOffer & po) {
                     return po.peaceId == id;
                   });
    if (peaceIt == peaceOffers.end())
        return;

    for (auto & prov : peaceIt->lostProv) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        assert(std::get<0>(prov) >= 0 && std::get<0>(prov) < provinces.size());
        assert(std::get<1>(prov) >= 0 && std::get<1>(prov) < countries.size());
        std::string provName = provinces.at(std::get<0>(prov))->GetName();
        std::string ctrName = countries.at(std::get<1>(prov))->GetName();
        dobj->objects.back()->values["content:"] = provName + std::string{" to "} + ctrName;
        //gui.AddToList(dobj, "botPeaceOffer", "lostProvs");
        delete dobj;
    }
    for (auto & prov : peaceIt->gainProv) {
        DataObj * dobj = new DataObj{"label"};
        dobj->objects.push_back(new DataObj{"text"});
        assert(std::get<0>(prov) >= 0 && std::get<0>(prov) < provinces.size());
        assert(std::get<1>(prov) >= 0 && std::get<1>(prov) < countries.size());
        std::string provName = provinces.at(std::get<0>(prov))->GetName();
        std::string ctrName = countries.at(std::get<1>(prov))->GetName();
        dobj->objects.back()->values["content:"] = provName + std::string{" to "} + ctrName;
        //gui.AddToList(dobj, "botPeaceOffer", "gainProvs");
        delete dobj;
    }
*/}

void GuiAid::AcceptPeace(std::vector<PeaceOffer> &  peaceOffers, std::vector<sf::Packet> & packets)
{
  /*  int peaceId = std::stoi(event.values["peaceId:"]);

    auto peaceIt = std::find_if(peaceOffers.begin(), peaceOffers.end(), [peaceId](PeaceOffer & po) {
                    return po.peaceId == peaceId;
                   });

    if (peaceIt == peaceOffers.end())
        return;
    
    sf::Packet packet;
    packet << "AcceptPeace";
    packet << peaceId;
    packets.emplace_back(packet);

    //gui.EraseWin(event.values["windowType:"]);
    //gui.EraseObj("notifications", peaceIt->idInGui);
    peaceOffers.erase(peaceIt);
*/}

void GuiAid::DeclinePeace(std::vector<PeaceOffer> &  peaceOffers,
                  std::vector<sf::Packet> & packets)
{
  /*  int peaceId = std::stoi(event.values["peaceId:"]);
    Log("PeaceId = " << peaceId);
    auto peaceIt = std::find_if(peaceOffers.begin(), peaceOffers.end(), [peaceId](PeaceOffer & po) {
                    return po.peaceId == peaceId;
                   });

    if (peaceIt == peaceOffers.end())
        return;

    peaceOffers.erase(peaceIt);
    
    sf::Packet packet;
    packet << "DeclinePeace";
    packet << peaceId;
    packets.emplace_back(packet);

    //gui.EraseWin(event.values["windowType:"]);
    //gui.EraseObj("notifications", peaceIt->idInGui);
*/}




