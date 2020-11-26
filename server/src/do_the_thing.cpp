#include "do_the_thing.h"

void DoTheThing::AddUnit(std::shared_ptr<Unit> & u, std::shared_ptr<Province> & prov, std::shared_ptr<Country> & country)
{
    prov->AddUnit(u);
    country->AddUnit(u);
}

void DoTheThing::EraseUnit(std::shared_ptr<Unit> & u, std::shared_ptr<Province> & prov, std::shared_ptr<Country> & country, std::vector<Packet> & toSend) 
{
    prov->EraseUnit(u);
    country->EraseUnit(u);

    Packet packet{true};
    packet << "DeleteUnit";
    packet << u->GetId();
    toSend.emplace_back(packet);
}

void DoTheThing::NotifyOfDeclinedPeace(int clientCountryId, int recipantId, std::vector<Packet> & toSend)
{
    Packet packet;
    packet.PushRecipant(clientCountryId);
    packet << "PeaceDeclined";
    packet << recipantId;
    toSend.emplace_back(packet);
}


void DoTheThing::MoveUnit(std::shared_ptr<Unit> & unit, const std::vector<std::shared_ptr<Province>> & provinces, std::shared_ptr<Province> & startProv, std::shared_ptr<Province> & endProv)
{
    Posibility posibility = PathFinder().Find(provinces, startProv->GetId(), endProv->GetId());
    std::vector<Move> moves;
    for (auto p : posibility.path) {
        Move m;
        assert(p >= 0 && p < provinces.size());
        auto currProv = provinces.at(p);
        m.destiny = currProv->GetUnitPos();
        m.destinyId = p;
        moves.push_back(m);
    }
    unit->AddMove(moves);        
}

void DoTheThing::DeclareWar(std::vector<std::shared_ptr<Country>> & countries, int declaredBy, int declaringTo, 
                        std::vector<War*> & wars, std::vector<Packet> & toSend)
{
    bool possible = true;
    possible = !(declaringTo == declaredBy);
    for (auto & war : wars) {
        if (war->ShouldTheyFight(declaredBy, declaringTo)) {
            possible = false;
            break;
        }
    }

    assert(declaringTo >= 0 && declaringTo < countries.size());
    auto enemy = countries.at(declaringTo);
    
    assert(declaredBy >= 0 && declaredBy < countries.size());
    auto attacker = countries.at(declaredBy);

    if (enemy->IsTruce(declaredBy))
        possible = false;
        
    if (possible) {
        War * war = new War();
        war->AddAttacker(declaredBy);
        war->AddDefender(declaringTo);
        wars.emplace_back(war);

        enemy->AddWar(war);
        attacker->AddWar(war);
        // dodac jeszcze sojusznikom
        

        Packet newPacket{true};
        newPacket << "NewWar";
        newPacket << war->GetId();
        newPacket << declaredBy;
        newPacket << declaringTo;
        toSend.emplace_back(newPacket);
    }

}

void DoTheThing::RealizeAcceptedPeace(std::vector<std::shared_ptr<Country>> & countries, std::vector<std::shared_ptr<Province>> & provinces, 
                                        PeaceOffer & peaceOffer, std::vector<Packet> & toSend, std::shared_ptr<Country> & recipant, 
                                        std::vector<War*>::iterator & warIt, std::vector<War*> & wars)
{
LogFunkStart;
    std::vector<std::shared_ptr<Country>> participants;
    std::vector<std::shared_ptr<Province>> possibleProvs;

    for (auto & c : countries) {
        if (c->IsWarIn((*warIt))) {
            participants.push_back(c);
            auto tmp = c->GetProvs();
            possibleProvs.insert(possibleProvs.end(), tmp.begin(), tmp.end());
        }
    }
    
    Packet newPacket{true};
    newPacket << "PeaceAccepted";
    newPacket << (int)(peaceOffer.lostProv.size() + peaceOffer.gainProv.size());

    auto tmp = peaceOffer.lostProv;
    tmp.insert(tmp.begin(), peaceOffer.gainProv.begin(), peaceOffer.gainProv.end());
    for (auto & offeredProv : tmp) {
        newPacket << std::get<0>(offeredProv);
        newPacket << std::get<1>(offeredProv);
    }

    newPacket << (*warIt)->GetId();
            
    if (peaceOffer.recipantIsDefender) {
        if ((*warIt)->GetDefenderId() == recipant->GetId()) {
            if ((*warIt)->GetAttackerId() == peaceOffer.offeredBy) {
                newPacket << "endWar";
                for (auto & part : participants) {
                    part->RemoveWar(*warIt);
                }
                Log("EraseWar -> " << (*warIt)->GetId());
                delete (*warIt);
                wars.erase(warIt);
            }
            else {
                newPacket << "erase";
                newPacket << peaceOffer.offeredBy;
                recipant->RemoveWar(*warIt);        
                for (auto & prov : possibleProvs) {
                    prov->EraseSiegeUnitsCtr(recipant->GetId());
                }
                (*warIt)->Erase(peaceOffer.offeredBy);
            }
        }
        else  {
            if ((*warIt)->GetAttackerId() == peaceOffer.offeredBy) {
                newPacket << "erase";
                newPacket << peaceOffer.recipant;
                recipant->RemoveWar(*warIt);        
                for (auto & prov : possibleProvs) {
                    prov->EraseSiegeUnitsCtr(recipant->GetId());
                }
                (*warIt)->Erase(peaceOffer.recipant);
            }
        }
    }
    else {
        if ((*warIt)->GetAttackerId() == peaceOffer.recipant) {
            if ((*warIt)->GetDefenderId() == peaceOffer.offeredBy) {
                newPacket << "endWar";
                for (auto & part : participants) {
                    part->RemoveWar(*warIt);
                }
                Log("EraseWar -> " << (*warIt)->GetId());
                delete (*warIt);
                wars.erase(warIt);
            }
            else {
                newPacket << "erase";
                newPacket << peaceOffer.offeredBy;
                recipant->RemoveWar(*warIt);        
                for (auto & prov : possibleProvs) {
                    prov->EraseSiegeUnitsCtr(recipant->GetId());
                }
               (*warIt)->Erase(peaceOffer.offeredBy);
            }
        }
        else  {
            if ((*warIt)->GetDefenderId() == peaceOffer.offeredBy) {
                newPacket << "erase";
                newPacket << peaceOffer.recipant;
                recipant->RemoveWar(*warIt);        
                for (auto & prov : possibleProvs) {
                    prov->EraseSiegeUnitsCtr(recipant->GetId());
                }
                (*warIt)->Erase(peaceOffer.recipant);
            }
        }
    }
    toSend.emplace_back(newPacket);


    for (auto & offeredProv : tmp) {
        assert(std::get<0>(offeredProv) >= 0 && std::get<0>(offeredProv) < provinces.size());
        auto prov = provinces.at(std::get<0>(offeredProv));
        assert(prov->GetCountryId() >= 0 && prov->GetCountryId() < countries.size());
        auto ctrLoser = countries.at(prov->GetCountryId());
        
        assert(std::get<1>(offeredProv) >= 0 && std::get<1>(offeredProv) < countries.size());
        auto ctrWinner = countries.at(std::get<1>(offeredProv));
        
        ctrLoser->RemoveProvince(prov);
        prov->SetOwner(ctrWinner->GetName(), ctrWinner->GetId());
        ctrWinner->AddProvince(prov);
        
    }



LogFunkEnd;
}
 
bool DoTheThing::SendPeace(std::vector<War*> & wars, std::vector<std::shared_ptr<Province>> & provinces, std::vector<std::shared_ptr<Country>> & countries, 
                            PeaceOffer & peaceOffer, std::vector<std::pair<int, int>> & offeredProvinces, std::vector<Packet> & toSend, 
                            std::shared_ptr<Country> & recipant, std::vector<War*>::iterator & warIt, const Date & date, 
                            std::vector<PeaceOffer> & offersForHumans)
{
    LogFunkStart;
    for (auto &offeredProv : offeredProvinces) {
        assert(offeredProv.first >= 0 && offeredProv.first < provinces.size());
        auto prov = provinces.at(offeredProv.first);

        for (auto &att : peaceOffer.attackers) {
            if (offeredProv.second == att) {
                if (peaceOffer.recipantIsDefender)
                    peaceOffer.lostProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, prov->GetDevelopment()));
                else
                    peaceOffer.gainProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, prov->GetDevelopment()));
                break;
            }
        }
        for (auto &def : peaceOffer.defenders) {
            if (offeredProv.second == def) {
                if (peaceOffer.recipantIsDefender)
                    peaceOffer.gainProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, prov->GetDevelopment()));
                else
                    peaceOffer.lostProv.push_back(std::make_tuple(offeredProv.first, offeredProv.second, prov->GetDevelopment()));
                break;
            }
        }
    }

    if (recipant->IsOwnedByBoot() && recipant->Apply(peaceOffer, date)) {
        DoTheThing::RealizeAcceptedPeace(countries, provinces, peaceOffer, toSend, recipant, warIt, wars);
    LogFunkEnd;
        return true;
    }
    else if (recipant->IsOwnedByBoot() == false) {
        offersForHumans.push_back(peaceOffer);
        Packet packet{false};
        packet.PushRecipant(recipant->GetId());
        packet << "BotPeaceOffer";
        packet << peaceOffer.peaceId;
        packet << (*warIt)->GetId();
        packet << peaceOffer.offeredBy;
        packet << (int)peaceOffer.lostProv.size();
        for (auto & lp : peaceOffer.lostProv) {
            packet << std::get<0>(lp);
            packet << std::get<1>(lp);
            packet << std::get<2>(lp);
        }
        packet << (int)peaceOffer.gainProv.size();
        for (auto & gp : peaceOffer.gainProv) {
            packet << std::get<0>(gp);
            packet << std::get<1>(gp);
            packet << std::get<2>(gp);
        }
        toSend.push_back(packet);
    }
    else if (recipant->IsOwnedByBoot()) {
        for (auto &c : countries) {
            if (c->GetId() != peaceOffer.offeredBy)
                continue;
            if (c->IsOwnedByBoot())
                break;
            DoTheThing::NotifyOfDeclinedPeace(peaceOffer.offeredBy, peaceOffer.recipant, toSend);
        }
    }
LogFunkEnd;
    return false;
}

void DoTheThing::SendNewUnitInfo(const std::shared_ptr<Unit> & unit, std::vector<Packet> & toSend)
{
    Packet newPack(true);
    newPack << "NewUnit"; // type
    newPack << unit->GetId();
    newPack << unit->GetName();
    newPack << unit->GetCountryId();
    newPack << unit->GetMorale();
    newPack << unit->GetSoldiers();
    newPack << unit->GetFakePos().x;
    newPack << unit->GetFakePos().y;
    newPack << unit->GetFakePos().z;
    newPack << unit->GetProvId();
    toSend.emplace_back(newPack);
}

void DoTheThing::MergeUnits(std::vector<std::shared_ptr<Unit>> & unitsToErase, std::shared_ptr<Unit> & baseUnit, std::shared_ptr<Country> & country,
                             std::shared_ptr<Province> & prov, std::vector<Packet> & toSend)
{
    Packet mergePacket{true};
    mergePacket << "MergeUnits";
    mergePacket << baseUnit->GetId();
    mergePacket << (int)unitsToErase.size();
 
    for (auto u : unitsToErase) {
        mergePacket << u->GetId();
        baseUnit->Add(u);
    }

    toSend.emplace_back(mergePacket);

    for (auto u : unitsToErase) {
        DoTheThing::EraseUnit(u, prov, country, toSend);
    }


}





