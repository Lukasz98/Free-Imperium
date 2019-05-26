#pragma once

#include "war.h"
#include "battle.h"
#include "unit.h"
#include "province.h"
#include "country.h"

#include "path_finder.h"

#include "client.h"
#include "packet.h"

namespace ProcessPacket {

    void NewUnit(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, std::vector<Province> & provinces, std::vector<std::shared_ptr<Unit>> & units, std::vector<Packet> & toSend);

    void UnitMove(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<Province> & provinces, std::vector<std::shared_ptr<Unit>> & units);

    void DeclareWar(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, std::vector<War> & wars, std::vector<Packet> & toSend, const Date & date);

    void OfferPeace(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<War> & wars, std::vector<Province> & provinces, std::vector<std::shared_ptr<Country>> & countries, std::vector<Packet> & toSend, const Date & date);

    void MergeUnits(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Unit>> & units, std::vector<Province> & provinces, std::vector<Packet> & toSend);
    
    void StartImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries);
    void StopImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries);
}


namespace DoTheThing {

    static void MoveUnit(std::shared_ptr<Unit> & unit, const std::vector<Province> & provinces, int startProvId, int endProvId)
    {
        Posibility posibility = PathFinder().Find(provinces, startProvId, endProvId);
        std::vector<Move> moves;
        for (auto p : posibility.path) {
            Move m;
            auto currProvIt = std::find_if(provinces.begin(), provinces.end(), [p] (const Province & pr) {
                               return pr.GetId() == p;
                              });
            glm::vec2 newUnitPos = (*currProvIt).GetUnitPos();
            m.destiny = glm::vec3(newUnitPos, 0.1);
            moves.push_back(m);
        }
        unit->AddMove(moves);        
    }

    static void DeclareWar(std::vector<std::shared_ptr<Country>> & countries, const std::string declaredBy, const std::string declaringTo, std::vector<War> & wars, std::vector<Packet> & toSend, const Date & date)
    {
        bool possible = true;
        possible = !(declaringTo == declaredBy);
        for (auto & war : wars) {
            if (war.ShouldTheyFight(declaredBy, declaringTo)) {
                possible = false;
                break;
            }
        }

        auto enemyIt = std::find_if(countries.begin(), countries.end(), [declaringTo](std::shared_ptr<Country> & c){
                                                                        return c->GetName() == declaringTo;
                                                                    });
        if (enemyIt == countries.end()) {
            return;
            possible = false;
        }

        if ((*enemyIt)->IsTruce(declaredBy, date))
            possible = false;
        
        if (possible) {
            War war;
            war.AddAttacker(declaredBy);
            war.AddDefender(declaringTo);
            wars.emplace_back(war);

            Packet newPacket{true};
            newPacket << "NewWar";
            newPacket << war.GetId();
            newPacket << declaredBy;
            newPacket << declaringTo;
            toSend.emplace_back(newPacket);
        }

    }

    static bool SendPeace(std::vector<War> & wars, std::vector<Province> & provinces, std::vector<std::shared_ptr<Country>> & countries, PeaceOffer & peaceOffer, std::vector<std::pair<std::string,std::string>> & offeredProvinces, std::vector<Packet> & toSend, std::vector<std::shared_ptr<Country>>::iterator & recipantIt, std::vector<War>::iterator & warIt, const Date & date)
    {
        for (auto & offeredProv : offeredProvinces) {
            auto provIt = std::find_if(provinces.begin(), provinces.end(), [offeredProv](const Province & province) { return (province.GetName() == offeredProv.first); });
            
            for (auto & att : peaceOffer.attackers) {
                if (offeredProv.second == att) {
                    if (peaceOffer.recipantIsDefender)
                        peaceOffer.lostProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, (*provIt).GetDevelopment()));
                    else
                        peaceOffer.gainProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, (*provIt).GetDevelopment()));    
                    break;
                }
            }
            for (auto & def : peaceOffer.defenders) {
                if (offeredProv.second == def) {
                    if (peaceOffer.recipantIsDefender)
                        peaceOffer.gainProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, (*provIt).GetDevelopment()));
                    else
                        peaceOffer.lostProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, (*provIt).GetDevelopment()));
                    break;
                }
            }
        }
        if ((*recipantIt)->Apply(peaceOffer, date)) {
            Packet newPacket{true};
            newPacket << "PeaceAccepted";
            newPacket << (int)offeredProvinces.size();
            for (auto & offeredProv : offeredProvinces) {
                auto provIt = std::find_if(provinces.begin(), provinces.end(), [offeredProv](const Province & province) {
                                                                                   return (province.GetName() == offeredProv.first);
                                                                               });
                (*provIt).SetOwner(offeredProv.second);
                newPacket << offeredProv.first;
                newPacket << offeredProv.second;
            }
            for (auto & prov : provinces) {
                if (prov.GetSiegeCountry() == "")
                    continue;
                if (warIt->ShouldTheyFight(prov.GetCountry(), prov.GetSiegeCountry())) {
                    Log("Reset " << prov.GetName());
                    prov.ResetSiege();
                }
            }
        
            newPacket << (*warIt).GetId();
            
            if (peaceOffer.recipantIsDefender) {
                if ((*warIt).GetDefenderName() == (*recipantIt)->GetName()) {
                    if ((*warIt).GetAttackerName() == peaceOffer.offeredBy) {
                        newPacket << "endWar";
                        wars.erase(warIt);
                    }
                    else {
                        newPacket << "erase";
                        newPacket << peaceOffer.offeredBy;
                        (*warIt).Erase(peaceOffer.offeredBy);
                    }
                }
                else  {
                    if ((*warIt).GetAttackerName() == peaceOffer.offeredBy) {
                        newPacket << "erase";
                        newPacket << peaceOffer.recipant;
                        (*warIt).Erase(peaceOffer.recipant);
                    }
                }
            }
            else {
                if ((*warIt).GetAttackerName() == peaceOffer.recipant) {
                    if ((*warIt).GetDefenderName() == peaceOffer.offeredBy) {
                        newPacket << "endWar";
                        wars.erase(warIt);
                    }
                    else {
                        newPacket << "erase";
                        newPacket << peaceOffer.offeredBy;
                        (*warIt).Erase(peaceOffer.offeredBy);
                    }
                }
                else  {
                    if ((*warIt).GetDefenderName() == peaceOffer.offeredBy) {
                        newPacket << "erase";
                        newPacket << peaceOffer.recipant;
                        (*warIt).Erase(peaceOffer.recipant);
                    }
                }
            }
            toSend.emplace_back(newPacket);
            return true;
        }
        return false;
    }

    static void SendNewUnitInfo(const std::shared_ptr<Unit> & unit, std::vector<Packet> & toSend)
    {
        Packet newPack(true);
        newPack << "NewUnit"; // type
        newPack << unit->GetId();
        newPack << unit->GetName();
        newPack << unit->GetCountry();
        newPack << unit->GetMorale();
        newPack << unit->GetSoldiers();
        newPack << unit->GetFakePos().x;
        newPack << unit->GetFakePos().y;
        newPack << unit->GetFakePos().z;
        toSend.emplace_back(newPack);
    }

    static void MergeUnits(std::vector<std::shared_ptr<Unit>> & units, int baseUnitId, std::vector<int> & ids, std::vector<Packet> & toSend)
    {
        auto unitBaseIt = std::find_if(units.begin(), units.end(), [baseUnitId](const std::shared_ptr<Unit> & unit) {
                           return unit->GetId() == baseUnitId;
                          });

        if (unitBaseIt == units.end())
            return;
        
        Packet mergePacket{true};
        mergePacket << "MergeUnits";
        mergePacket << baseUnitId;
        mergePacket << (int)ids.size();
                
        for (auto & currId : ids) {
            auto unitIt = std::find_if(units.begin(), units.end(), [currId](const std::shared_ptr<Unit> & unit) {
                           return unit->GetId() == currId;
                          });
            if (unitIt != units.end()) {
                (*unitBaseIt)->Add((*unitIt));
                mergePacket << currId;
                units.erase(unitIt);
            }
        }

        toSend.emplace_back(mergePacket);
    }
}
