#include "process_packet.h"

#include "num_to_string.h"

void ProcessPacket::DailyUpdate(sf::Packet& packet, std::vector<War>& wars, std::vector<Province>& provinces,
                                std::vector<Country>& countries, TopBarData& topBarData, Map2* map2)
{
    int countryCounter = 0;
    packet >> countryCounter;

    for (int z = 0; z < countryCounter; z++) {
        packet >> topBarData.ctrName;
        packet >> topBarData.gold;
        packet >> topBarData.income;
        packet >> topBarData.armyMaintenance;
        packet >> topBarData.manpower;
        packet >> topBarData.manpRecovery;
        packet >> topBarData.date;
        packet >> topBarData.dateSpeed;

        int provCount;
        packet >> provCount;
        for (int i = 0; i < provCount; i++) {
            int id, manpower, development;
            float treasury;
            float monthIncome, totalMonthIncome;

            packet >> id;
            packet >> manpower;
            packet >> development;
            packet >> treasury;
            packet >> monthIncome;
            packet >> totalMonthIncome;

            assert(id >= 0 && id < provinces.size());
            provinces[id].SetManpower(manpower);
            provinces[id].SetDevelopment(development);
            provinces[id].SetTreasury(treasury);
            provinces[id].SetMonthIncome(monthIncome);
            provinces[id].SetTotalMonthIncome(totalMonthIncome);
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
                attackers.push_back(aId);
            }
            packet >> defendersCount;
            for (int def = 0; def < defendersCount; def++) {
                int dId;
                packet >> dId;
                defenders.push_back(dId);
            }

            for (auto& war : wars) {
                if (war.GetId() == id) {
                    war.SetWarScore(score);
                    break;
                }
            }
        }

        int siegedProvincesCount;
        packet >> siegedProvincesCount;

        map2->occupiedPix.resize(provinces.size() * 4);
        map2->occupyingPix.resize(provinces.size() * 4);
        std::memset(&map2->occupiedPix[0], 0, map2->occupiedPix.size());
        std::memset(&map2->occupyingPix[0], 0, map2->occupyingPix.size());
        for (int i = 0; i < siegedProvincesCount; i++) {
            int provId, siegeCountryId;
            int sieged, siegeSoldiers;
            packet >> provId;
            packet >> sieged;
            packet >> siegeCountryId;
            packet >> siegeSoldiers;
            assert(provId >= 0 && provId < provinces.size());
            assert(siegeCountryId >= 0 && siegeCountryId < countries.size());
            if (sieged == 100) {
                map2->occupiedPix[provId * 4] = countries[siegeCountryId].color.r;
                map2->occupiedPix[provId * 4 + 1] = countries[siegeCountryId].color.g;
                map2->occupiedPix[provId * 4 + 2] = countries[siegeCountryId].color.b;
                map2->occupiedPix[provId * 4 + 3] = countries[siegeCountryId].color.a;
            }
            map2->occupyingPix[provId * 4] = countries[siegeCountryId].color.r;
            map2->occupyingPix[provId * 4 + 1] = countries[siegeCountryId].color.g;
            map2->occupyingPix[provId * 4 + 2] = countries[siegeCountryId].color.b;
            map2->occupyingPix[provId * 4 + 3] = countries[siegeCountryId].color.a;
            provinces[provId].Sieging(countries[siegeCountryId].name, siegeCountryId, sieged, siegeSoldiers);
        }
        if (map2->occupiedText != nullptr)
            delete map2->occupiedText;
        map2->occupiedText = new Texture(&map2->occupiedPix[0], provinces.size(), 1);
        if (map2->occupyingText != nullptr)
            delete map2->occupyingText;
        map2->occupyingText = new Texture(&map2->occupyingPix[0], provinces.size(), 1);

        for (auto& prov : provinces) {
            if (!prov.WasSiegeUpdated()) {
                if (prov.GetSieged() != 0) {
                    if (prov.GetCountryId() < 0)  // its water
                        continue;
                    assert(prov.GetCountryId() >= 0 && prov.GetCountryId() < countries.size());
                }
                prov.ResetSieging();
            }
        }
    }
}

void ProcessPacket::HourlyUpdate(sf::Packet& packet, std::vector<Unit>& units, std::vector<Battle>& battles,
                                 float mapChunkScale, const unsigned char* height, int mapWidth)
{
    units.clear();
    unsigned int unitCount;
    packet >> unitCount;
    units.resize(unitCount);
    for (int i = 0; i < unitCount; i++) {
        int id, soldiersCount, ctrId, provId;
        std::string name;
        glm::vec3 uPos;
        packet >> id;
        packet >> ctrId;
        packet >> name;
        packet >> soldiersCount;
        packet >> uPos.x;
        packet >> uPos.y;
        packet >> uPos.z;
        packet >> provId;
        uPos.z = (float)height[(int)(uPos.x * 3 + uPos.y * mapWidth * 3)];
        uPos.x *= mapChunkScale;
        uPos.y *= mapChunkScale;

        int movesCount = 0;
        packet >> movesCount;

        units[i].movesVec.resize(movesCount);
        for (int j = 0; j < movesCount; j++) {
            glm::vec3 v;
            packet >> v.x;
            packet >> v.y;
            packet >> v.z;
            v.z = (float)height[(int)(v.x * 3 + v.y * mapWidth * 3)];
            v.x *= mapChunkScale;
            v.y *= mapChunkScale;
            units[i].movesVec[j] = v;
        }

        units[i].id = id;
        units[i].name = name;
        units[i].soldiers = soldiersCount;
        units[i].countryId = ctrId;
        units[i].position = uPos;
        units[i].fakePos = uPos;
        units[i].provId = provId;
    }

    int battlesCount;
    packet >> battlesCount;
    battles.resize(battlesCount);
    for (int i = 0; i < battlesCount; i++) {
        packet >> battles[i].id;
        packet >> battles[i].provId;
        packet >> battles[i].attackersSize;
        packet >> battles[i].defendersSize;
        int attCount = 0, defCount = 0;
        packet >> attCount;
        packet >> defCount;
        battles[i].attackers.resize(attCount);
        battles[i].defenders.resize(defCount);
        for (int j = 0; j < attCount; j++) {
            packet >> battles[i].attackers[j];
        }
        for (int j = 0; j < defCount; j++) {
            packet >> battles[i].defenders[j];
        }
    }
}

int ProcessPacket::PeaceAccepted(sf::Packet& packet, std::vector<Province>& provinces,
                                 std::vector<Country>& countries, std::vector<War>& wars, SideBarData& sideBarData)
{
    int provinceCount;
    packet >> provinceCount;

    for (int i = 0; i < provinceCount; i++) {
        int provId, ctrId;
        packet >> provId;
        packet >> ctrId;
        assert(ctrId >= 0 && ctrId < countries.size());
        assert(provId >= 0 && provId < provinces.size());
        provinces[provId].SetOwner(countries[ctrId].name, ctrId);
    }

    int warId;
    packet >> warId;

    auto warIt = std::find_if(wars.begin(), wars.end(), [warId](const War& war) { return war.GetId() == warId; });
    if (warIt == wars.end())
        return -1;

    std::string peaceType;
    packet >> peaceType;
    if (peaceType == "endWar") {
        for (std::size_t i = 0; i < sideBarData.elements.size(); ++i) {
            if (sideBarData.elements[i].val == warId) {
                sideBarData.elements.erase(sideBarData.elements.begin() + i);
                break;
            }
        }
        wars.erase(warIt);
    }
    else {
        int ctr;
        packet >> ctr;
        (*warIt).Erase(ctr);
    }
    return warId;
}

void ProcessPacket::NewWar(sf::Packet& packet, std::vector<War>& wars, int myCountryId,
                           std::vector<Country>& countries, SideBarData& sideBarData)
{
    int id;
    int attackerId, defenderId;

    packet >> id;
    packet >> attackerId;
    packet >> defenderId;

    if (myCountryId == attackerId || myCountryId == defenderId) {
        assert(attackerId >= 0 && attackerId < countries.size());
        assert(defenderId >= 0 && defenderId < countries.size());

        std::string rival =
            (myCountryId == attackerId) ? countries[defenderId].name : countries[attackerId].name;
        sideBarData.elements.push_back(
            SideBarData::Element{.type = SideBarData::IType::WAR, .val = id, .hoverText = rival});
        War war{id, 0};
        war.AddAttacker(countries[attackerId].name, attackerId);
        war.AddDefender(countries[defenderId].name, defenderId);
        wars.emplace_back(war);
    }
}
void ProcessPacket::ProvincePopulation(sf::Packet& packet, std::vector<Province>& provinces)
{
    int id, population;
    packet >> id;
    packet >> population;
    auto provIt = std::find_if(provinces.begin(), provinces.end(),
                               [id](const Province& prov) { return id == prov.id; });
    if (provIt != provinces.end()) {
        provIt->SetPopulation(population);
    }
}

void ProcessPacket::SetGold(sf::Packet& packet, Country* country)
{
    float gold = 0.0f;
    packet >> gold;
    country->gold = gold;
}

void ProcessPacket::SetSpeed(sf::Packet& packet)
{
    int speed = 1;
    packet >> speed;
}

void ProcessPacket::BotPeaceOffer(sf::Packet& packet, std::vector<PeaceOffer>& peaceOffers,
                                  const std::vector<Country>& countries, SideBarData& sideBarData,
                                  const Country* myCountry)
{
    PeaceOffer peaceOffer;
    int lostProvCount, gainProvCount;

    packet >> peaceOffer.peaceId;
    packet >> peaceOffer.warId;
    packet >> peaceOffer.offeredBy;
    peaceOffer.recipant = myCountry->id;

    packet >> lostProvCount;
    for (int i = 0; i < lostProvCount; i++) {
        int s, ss;
        int d;
        packet >> s;   // provid
        packet >> ss;  // ctrid (to who it goes)
        packet >> d;   // development
        peaceOffer.lostProv.emplace_back(std::make_tuple(s, ss, d));
    }

    packet >> gainProvCount;
    for (int i = 0; i < gainProvCount; i++) {
        int s, ss;
        int d;
        packet >> s;
        packet >> ss;
        packet >> d;
        peaceOffer.gainProv.emplace_back(std::make_tuple(s, ss, d));
    }

    for (auto it = peaceOffers.begin(); it != peaceOffers.end(); ++it) {
        if (it->warId == peaceOffer.warId && it->offeredBy == peaceOffer.offeredBy) {
            for (std::size_t i = 0; i < sideBarData.elements.size(); ++i) {
                if (sideBarData.elements[i].val == it->peaceId) {
                    sideBarData.elements.erase(sideBarData.elements.begin() + i);
                    break;
                }
            }
            peaceOffers.erase(it);
            --it;
        }
    }

    assert(peaceOffer.offeredBy >= 0 && peaceOffer.offeredBy < countries.size());
    sideBarData.elements.push_back(SideBarData::Element{.type = SideBarData::IType::PEACE_OFFER,
                                                        .val = peaceOffer.peaceId,
                                                        .hoverText = countries[peaceOffer.offeredBy].name});
    peaceOffers.push_back(peaceOffer);
}

void ProcessPacket::PeaceDeclined(sf::Packet& packet, SideBarData& sideBarData,
                                  const std::vector<Country>& countries)
{
    int recipantid;
    packet >> recipantid;
    assert(recipantid >= 0 && recipantid < countries.size());

    sideBarData.elements.push_back(SideBarData::Element{
        .type = SideBarData::IType::PEACE_DECLINED, .val = 0, .hoverText = countries[recipantid].name});
}

void ProcessPacket::Paused(sf::Packet& packet, bool& game_paused)
{
    packet >> game_paused;
    Log(game_paused);
}
