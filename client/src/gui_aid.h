#pragma once

#include <vector>
#include <algorithm>

#include "gui/gui.h"
#include "country.h"
#include "battle.h"
#include "war.h"
#include "prepare_packet.h"
#include "peace_offer.h"

class GuiAid
{
protected:
    GuiAid() { }
    std::vector<sf::Packet> packets;

public:
    virtual ~GuiAid() { }
    std::vector<sf::Packet> GetPackets() { return packets; }
};

class GA_OpenProv : public GuiAid
{
public:
    GA_OpenProv(Gui & gui, const std::vector<std::unique_ptr<Province>> & provs, const GuiClick & event)
    {
        //std::string provName = std::any_cast<std::string>(ev->GetData());
        std::string provName = "xd";
        for (auto & p : provs) {
            if (p->GetName() == provName) {
                break;
            }
        }        
    }
};

class GA_DecreaseValue : public GuiAid
{
public:
    GA_DecreaseValue(Gui & gui, GuiClick & event)
    {
        std::unordered_map<std::string,std::string> values;
        int newValue = std::stoi(event.values["newUnitSize:"]) - 100;
        values["newUnitSize"] = std::to_string(newValue);
        gui.Update(values, event.values["windowType:"]);
    }
};

class GA_IncreaseValue : public GuiAid
{
public:
    GA_IncreaseValue(Gui & gui, GuiClick & event)
    {
        std::unordered_map<std::string,std::string> values;
        int newValue = std::stoi(event.values["newUnitSize:"]) + 100;
        values["newUnitSize"] = std::to_string(newValue);
        gui.Update(values, event.values["windowType:"]);
    }
};

class GA_OpenCountry : public GuiAid
{
public:
    GA_OpenCountry(Gui & gui, const std::vector<std::shared_ptr<Country>> & countries, const std::shared_ptr<Country> & myCountry, const std::vector<War> & wars, const GuiClick & event)
    {
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
            if (c->GetName() == countryName) {
                bool atWar = false;
                for (auto & war : wars) {
                    if (war.ShouldTheyFight(countryName, myCountry->GetName())) {
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
                
                gui.AddWin("src/gui/country.txt");
                gui.Update(values, "country");

                //gui.Update(values, "country");

                //values.clear();
                if (countryName != myCountry->GetName()) {
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
                    gui.AddToList(dobj, "country", "diplomatic");
                    delete dobj;
                }
                if (countryName != myCountry->GetName()) {
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
                    gui.AddToList(dobj, "country", "diplomatic");
                    delete dobj;
                }
                break;
            }
        }        
    }
};

class GA_NewUnit : public GuiAid
{
public:
    GA_NewUnit(const GuiClick & event)
    {
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

        packets.emplace_back(
           PreparePacket::NewUnit(country, prov, std::stoi(size)) 
        );
    }
};

class GA_OpenUnit : public GuiAid
{
public:
    GA_OpenUnit(Gui & gui, GuiClick & event, const std::vector<std::shared_ptr<Unit>> & units)
    {
        std::stringstream uid_ss;
        uid_ss << event.values["unitName:"];
        std::string uid_s;
        uid_ss >> uid_s;
        int unitId = std::stoi(uid_s);
        
        for (auto & unit : units) {
            if (unit->GetId() == unitId) {
                auto values = unit->GetValues();
                gui.AddWin("src/gui/unit.txt");
                gui.Update(values, "unit");

                break;
            }
        }
    }
};

class GA_EraseObj : public GuiAid
{
public:
    GA_EraseObj(Gui & gui, GuiClick & event)
    {
        for (auto & v : event.values)
            Log(v.first << ", " << v.second);
        gui.EraseObj(event.values["windowType:"], std::stoi(event.values["id:"]));
    }
};

class GA_DeclareWar : public GuiAid
{
public:
    GA_DeclareWar(GuiClick & event)
    {
        //auto countryName = std::any_cast<std::string>(ev->GetData());
        Log("WAR WAR");
        std::string countryName = event.values["countryName:"];
        sf::Packet packet;
        packet << "DeclareWar" << countryName;
        packets.push_back(packet);
    }
};

class GA_OpenWar : public GuiAid
{
public:
    GA_OpenWar(Gui & gui, GuiClick & event, const std::vector<War> & wars)
    {
        std::unordered_map<std::string,std::string> values;
        values["warId"] = event.values["impact:"];

        if (values["warId"] == "")
            return;
        
        auto warIt = std::find_if(wars.begin(), wars.end(), [id = std::stoi(values["warId"])](const War & war) { return id == war.GetId(); });
        if (warIt == wars.end())
            return;
        
        values["attackerWarScore"] = std::to_string(warIt->GetAttackerWarScore());

        gui.AddWin("src/gui/war.txt");
        gui.Update(values, "war");

        auto defenders = warIt->GetDefenders();
        auto attackers = warIt->GetAttackers();
        for (auto & d : defenders) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["content:"] = d;
            dobj->objects.back()->values["clickEvent:"] = "openCountry";
            gui.AddToList(dobj, "war", "defenders");
            delete dobj;
        }
        for (auto & a : attackers) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["content:"] = a;
            dobj->objects.back()->values["clickEvent:"] = "openCountry";
            gui.AddToList(dobj, "war", "attackers");
            delete dobj;
        }

    }
};

class GA_OpenBattle : public GuiAid
{
public:
    GA_OpenBattle(Gui & gui, const std::vector<std::unique_ptr<Battle>>::iterator & battleIt, const std::vector<std::unique_ptr<Province>>::iterator & provIt)
    {
        auto values = (*battleIt)->GetValues();
        auto val2 = (*provIt)->GetValues();

        for (auto & v : val2) {
            values[v.first] = v.second;
        }

        gui.AddWin("src/gui/battle.txt");
        gui.Update(values, "battle");
        gui.ObservationOfLastWin((*battleIt).get());
    }
};

class GA_OpenSiegedProv : public GuiAid
{
public:
    GA_OpenSiegedProv(Gui & gui, const std::vector<std::unique_ptr<Province>>::iterator & provIt, const std::vector<War> & wars)
    {
        auto values = (*provIt)->GetValues();
        std::vector<std::string> possibleSiegers;

        gui.AddWin("src/gui/sieged_province.txt");
        gui.Update(values, "siegedProvince");
        gui.ObservationOfLastWin((*provIt).get());

        for (auto & w : wars) {
            if (w.ShouldTheyFight(values["countryName"], values["siegeCountry"])) {
                if (w.IsAttacker(values["siegeCountry"]))
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
            gui.AddToList(obj, "siegedProvince", "siegeCandidates");
            delete obj;
        }        
    }
};

class GA_OfferPeace : public GuiAid
{
public:
    GA_OfferPeace(Gui & gui, const std::vector<War> & wars, GuiClick & event, const std::string & myCountry)
    {
        bool atWar = false;
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
        gui.AddWin("src/gui/offer_peace.txt");
        gui.Update(values, "offerPeace");
        
    }
};


class GA_SendPeaceOffer : public GuiAid
{
public:
    GA_SendPeaceOffer(Gui & gui, const std::vector<War> & wars, GuiClick & event)
    {
        std::vector<std::string> agreements = gui.GetContentOf("offerPeace", "agreement");
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

        sf::Packet packet;
        packet << "PeaceOffer";
        packet << std::stoi(event.values["warId:"]);
        packet << event.values["recipant:"];
        packet << (int)provs.size();
        for (auto & prov : provs) {
            packet << prov.first;
            packet << prov.second;
        }
        packets.emplace_back(packet);
    }
};

class GA_MergeUnits : public GuiAid
{
public:
    GA_MergeUnits(Gui & gui, const GuiClick & event)
    {
        //Log("MERGE UNIT");
        sf::Packet packet;
        packet << "MergeUnits";

        std::vector<std::string> content = gui.GetContentOf("units", "unitName");

        std::vector<int> ids;
        for (auto & unit : content) {
            //Log("Content: " << unit);
            std::stringstream ss;
            ss << unit;
            while (!ss.eof()) {
                std::string c;
                ss >> c;
                Log("in " << c);
                int id = std::stoi(c);
                ids.push_back(id);
                break;
            }
        }
        packet << (int)ids.size();
        for (auto & id : ids)
            packet << id;

        packets.emplace_back(packet);
    }
};

class GA_DateSpeed : public GuiAid
{
public:
    GA_DateSpeed(GuiClick & event)
    {
        sf::Packet packet;
        packet << "Speed";
        //Log(event.values["content:"]);
        if (event.values["content:"] == "faster")
            packet << true;
        else
            packet << false;
        packets.emplace_back(packet);
    }
};


class GA_StartGame : public GuiAid
{
public:
    GA_StartGame()
    {
        sf::Packet packet;
        packet << "Start";
        packets.emplace_back(packet);
    }
};

class GA_SetCountry : public GuiAid
{
public:
    GA_SetCountry(GuiClick & click)
    {
        sf::Packet packet;
        packet << "country";
        packet << click.values["content:"];
        packets.emplace_back(packet);
    }
};

class GA_StartImprRel : public GuiAid
{
public:
    GA_StartImprRel(GuiClick & event, std::shared_ptr<Country> & myCountry)
    {
        std::string country = event.values["countryName:"];
        sf::Packet packet;
        packet << "StartImprRel";
        packet << myCountry->GetName();
        packet << country;
        packets.emplace_back(packet);
        myCountry->StartImprRel(country);
    }
};

class GA_StopImprRel : public GuiAid
{
public:
    GA_StopImprRel(GuiClick & event, std::shared_ptr<Country> & myCountry)
    {
        std::string country = event.values["countryName:"];
        sf::Packet packet;
        packet << "StopImprRel";
        packet << myCountry->GetName();
        packet << country;
        packets.emplace_back(packet);
        myCountry->StopImprRel(country);
    }
};

class GA_BotPeaceOffer : public GuiAid
{
public:
    GA_BotPeaceOffer(GuiClick & event, Gui & gui, std::vector<PeaceOffer> & peaceOffers)
    {
        gui.AddWin("src/gui/bot_peace_offer.txt");
        std::unordered_map<std::string,std::string> values;
        values["peaceId"] = event.values["impact:"];
        gui.Update(values, "botPeaceOffer");

        auto peaceIt = std::find_if(peaceOffers.begin(), peaceOffers.end(), [id = std::stoi(values["peaceId"])](PeaceOffer & po) {
                         return po.peaceId == id;
                       });
        if (peaceIt == peaceOffers.end())
            return;

        for (auto & prov : peaceIt->lostProv) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["content:"] = std::get<0>(prov) + std::string{" to "} + std::get<1>(prov);
            gui.AddToList(dobj, "botPeaceOffer", "lostProvs");
            delete dobj;
        }
        for (auto & prov : peaceIt->gainProv) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["content:"] = std::get<0>(prov) + std::string{" to "} + std::get<1>(prov);
            gui.AddToList(dobj, "botPeaceOffer", "gainProvs");
            delete dobj;
        }
    }
};

class GA_AcceptPeace : public GuiAid
{
public:
    GA_AcceptPeace(GuiClick & event, Gui & gui, std::vector<PeaceOffer> &  peaceOffers)
    {
        int peaceId = std::stoi(event.values["peaceId:"]);

        auto peaceIt = std::find_if(peaceOffers.begin(), peaceOffers.end(), [peaceId](PeaceOffer & po) {
                        return po.peaceId == peaceId;
                       });

        if (peaceIt == peaceOffers.end())
            return;
        
        sf::Packet packet;
        packet << "AcceptPeace";
        packet << peaceId;
        packets.emplace_back(packet);

        gui.EraseWin(event.values["windowType:"]);
        gui.EraseObj("notifications", peaceIt->idInGui);
        peaceOffers.erase(peaceIt);
    }
};

class GA_DeclinePeace : public GuiAid
{
public:
    GA_DeclinePeace(GuiClick & event, Gui & gui, std::vector<PeaceOffer> &  peaceOffers)
    {
        int peaceId = std::stoi(event.values["peaceId:"]);
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

        gui.EraseWin(event.values["windowType:"]);
        gui.EraseObj("notifications", peaceIt->idInGui);
    }
};
