#include "process_packet.h"

void ProcessPacket::NewUnit(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, 
                            std::vector<std::shared_ptr<Province>> & provinces, std::vector<Packet> & toSend)
{

    int ctrId, provId;
    int unitSize;
    packet >> ctrId >> provId >> unitSize;

        auto country = client->GetCountry();
        if (country->GetId() != ctrId)
            return;

    assert(provId >= 0 && provId < provinces.size());
    auto prov = provinces.at(provId);
        
    if (country->GetId() != prov->GetCountryId())
        return;

    if (prov->GetManpower() < unitSize)
        return;


    float priceForHundred = 0.1f;
    float price = (float)unitSize / (priceForHundred * 1000.0f);
    if (country->GetGold() >= price) {
        country->EraseMoney(price);
        prov->EraseManpower(unitSize);
        country->EraseManpower(unitSize);

        auto unit = std::make_shared<Unit>(prov->GetName() + " unit", prov->GetUnitPos(), unitSize, country->GetName(), provId, ctrId, provId);
        DoTheThing::AddUnit(unit, prov, country);
        DoTheThing::SendNewUnitInfo(unit, toSend);

        Packet pack{};
        pack.PushRecipant(ctrId);
        pack << "SetGold";
        pack << country->GetGold();
        toSend.emplace_back(pack);
    }        

}

void ProcessPacket::UnitMove(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Province>> & provinces)
{
    int uId;
    int provId;
    packet >> uId;
    packet >> provId;

    auto units = client->GetCountry()->GetUnits();
    auto unitIt = std::find_if(units.begin(), units.end(), [uId](const std::shared_ptr<Unit> & u){
            return u->GetId() == uId;
        });
    if (unitIt == units.end())
        return;

    if ((*unitIt)->GetCountryId() != client->GetCountry()->GetId())
        return;

    assert(provId >= 0 && provId < provinces.size());
    auto prov = provinces.at(provId);    

    assert((*unitIt)->GetProvId() >= 0 && (*unitIt)->GetProvId() < provinces.size());
    auto unitProv = provinces.at((*unitIt)->GetProvId());

    DoTheThing::MoveUnit(*unitIt, provinces, unitProv, prov);
}

void ProcessPacket::DeclareWar(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Country>> & countries, 
                                std::vector<War*> & wars, std::vector<Packet> & toSend)
{
    int declaringTo;
    packet >> declaringTo;
    
    auto declaredBy = client->GetCountry()->GetId();

    DoTheThing::DeclareWar(countries, declaredBy, declaringTo, wars, toSend);
}

void ProcessPacket::OfferPeace(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<War*> & wars, 
                                std::vector<std::shared_ptr<Province>> & provinces, 
                                std::vector<std::shared_ptr<Country>> & countries, std::vector<Packet> & toSend, const Date & date, 
                                std::vector<PeaceOffer> & offersForHumans)
{
    int warId, provincesCount;
    int recipantId;
    std::vector<std::pair<int, int>> offeredProvinces;
    packet >> warId;
    packet >> recipantId;
    packet >> provincesCount;
    for (int i = 0; i < provincesCount; i++) {
        std::pair<int, int> prov;
        packet >> prov.first;
        packet >> prov.second;
        offeredProvinces.emplace_back(prov);
    }

    bool offeredForDefenders = false;
    int offeredBy = client->GetCountry()->GetId();
    auto warIt = std::find_if(wars.begin(), wars.end(), [warId](const War * war) {
            return war->GetId() == warId;
        });
    if (warIt == wars.end())
        return;

    if (!(*warIt)->ShouldTheyFight(offeredBy, recipantId))
        return;

    offeredForDefenders = (*warIt)->IsAttacker(offeredBy);

    bool possible = true;
    for (auto prov : offeredProvinces) {
        if (prov.first < 0 || prov.first >= provinces.size()) {
            possible = false;
            break;
        }
    }

    if (!possible)
        return;

    assert(recipantId >= 0 && recipantId < countries.size());
    auto recipant = countries.at(recipantId);

    PeaceOffer peaceOffer;
    peaceOffer.warId = warId;
    peaceOffer.warScore = (*warIt)->GetAttackerWarScore();
    peaceOffer.recipant = recipantId;
    peaceOffer.recipantIsDefender = offeredForDefenders;
    peaceOffer.attackers = (*warIt)->GetAttackers();
    peaceOffer.defenders = (*warIt)->GetDefenders();
    peaceOffer.offeredBy = offeredBy;

    auto ccc = client->GetCountry();
    if (DoTheThing::SendPeace(wars, provinces, countries, peaceOffer, offeredProvinces, toSend, recipant, warIt, date, offersForHumans)) {
        Truce truce{date};
        truce.enemy = recipantId;
        truce.year++;
        client->GetCountry()->AddTruce(truce);
    }

}

void ProcessPacket::MergeUnits(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<std::shared_ptr<Province>> & provinces, 
                                std::vector<Packet> & toSend)
{
    int countUnits;
    std::vector<int> unitsIds;
    packet >> countUnits;
    for (int i = 0; i < countUnits; i++) {
        int id;
        packet >> id;
        unitsIds.push_back(id);
    }

    auto units = client->GetCountry()->GetUnits();

    bool possible = true;
    std::string whatProvinceTheyAre = "";
    std::vector<std::shared_ptr<Unit>> mergingUnits;
    auto provLegit = provinces.at(0);
    for (auto id : unitsIds) {
        auto unitIt = std::find_if(units.begin(), units.end(), [id](const std::shared_ptr<Unit> & unit) {
                return unit->GetId() == id;
            });
        if (unitIt == units.end()) {
            continue;
        }
        if ((*unitIt)->GetCountry() != client->GetCountry()->GetName()) {
            continue;
        }
        assert((*unitIt)->GetProvId() >= 0 && (*unitIt)->GetProvId() < provinces.size());
        auto prov = provinces.at((*unitIt)->GetProvId());
        if (whatProvinceTheyAre == "")
            whatProvinceTheyAre = prov->GetName();
        else if (whatProvinceTheyAre != prov->GetName()) {
            possible = false;
            break;
        }

        if ((*unitIt)->IsInBattle()) {
            possible = false;
            break;
        }
        if (possible) {
            mergingUnits.push_back(*unitIt);
            provLegit = prov;
        }
    }

    if (unitsIds.size() > 1 && possible) {
        // merging
        auto baseUnit = mergingUnits.back();
        mergingUnits.pop_back();
        auto country = client->GetCountry();
        DoTheThing::MergeUnits(mergingUnits, baseUnit, country, provLegit, toSend);
    }

}

void ProcessPacket::StartImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries)
{
    int c1, c2;
    packet >> c1;
    packet >> c2;

    assert(c1 >= 0 && c1 < countries.size());
    auto ctr = countries.at(c1);
    ctr->StartImprRel(c2);
}

void ProcessPacket::StopImprRel(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries)
{
    int c1, c2;
    packet >> c1;
    packet >> c2;

    assert(c1 >= 0 && c1 < countries.size());
    auto ctr = countries.at(c1);
    ctr->StopImprRel(c2);
}

void ProcessPacket::AcceptedPeace(sf::Packet & packet, std::vector<std::shared_ptr<Country>> & countries, std::vector<War*> & wars,
            std::vector<std::shared_ptr<Province>> & provinces,
            std::vector<PeaceOffer> & offersForHumans, std::vector<Packet> & toSend)
{
    int peaceId;
    packet >> peaceId;

    auto offerIt = std::find_if(offersForHumans.begin(), offersForHumans.end(), [peaceId](PeaceOffer & p) { return p.peaceId == peaceId; });
    if (offerIt == offersForHumans.end())
        return;
    auto warIt = std::find_if(wars.begin(), wars.end(), [warId = offerIt->warId](War * w) { return w->GetId() == warId; });
    if (warIt == wars.end())
        return;
    assert(offerIt->recipant >= 0 && offerIt->recipant < countries.size());
    auto recipant = countries.at(offerIt->recipant);

    PeaceOffer peaceOffer = *offerIt;
    offersForHumans.erase(offerIt);
    DoTheThing::RealizeAcceptedPeace(countries, provinces, peaceOffer, toSend, recipant, warIt, wars);
}

void ProcessPacket::DeclinePeace(sf::Packet & packet, std::vector<PeaceOffer> & offersForHumans, std::vector<std::shared_ptr<Client>> & clients, 
                                std::vector<Packet> & toSend)
{
    int peaceId = 0;
    packet >> peaceId;

    auto peaceIt = std::find_if(offersForHumans.begin(), offersForHumans.end(), [peaceId](PeaceOffer & p) {
                     return p.peaceId == peaceId;
                   });
    if (peaceIt == offersForHumans.end())
        return;

    for (auto & c : clients) {
        if (peaceIt->offeredBy == c->GetCountry()->GetId()) {
            DoTheThing::NotifyOfDeclinedPeace(c->GetCountry()->GetId(), peaceIt->recipant, toSend);
        }
    }

    offersForHumans.erase(peaceIt);
    
}



