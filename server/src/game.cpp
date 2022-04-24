#include "game.h"

Game::Game(std::vector<std::shared_ptr<Client>>& clients, std::vector<std::shared_ptr<Country>>& _countries)
    : clients(clients), countries(_countries)
{
    Log("Game()");

    // both provs and ctr id's have to start from 0 to n;
    // id's of provs and ctrs are used are vector indexes
    // provs and ctrs arent removed during game

    std::map<unsigned int, int> colorToId;
    provinces = ProvinceLoader::Load(colorToId);
    std::sort(provinces.begin(), provinces.end(),
              [](const std::shared_ptr<Province>& c1, const std::shared_ptr<Province>& c2) {
                  return c1->GetId() < c2->GetId();
              });
    Log(provinces.at(0)->GetId() << ", " << provinces.back()->GetId());

    Log("countries size=" << _countries.size());
    for (auto& p : provinces) {
        if (p->GetCountryId() >= countries.size() || p->GetCountryId() < 0)
            continue;
        countries[p->GetCountryId()]->AddProvince(p);
    }

    for (auto& client : clients) {
        std::string name = client->GetCountryName();

        auto it = std::find_if(countries.begin(), countries.end(),
                               [name](const std::shared_ptr<Country> c) { return c->GetName() == name; });
        if (it != countries.end()) {
            (*it)->OwnByHuman(true);
            client->SetCountry(*it);
        }
        else {
            Log("country " << name << " doesnt exits");
        }
    }
    srand(time(NULL));
}

Game::~Game() {}

void Game::Play()
{
    bool play = true;
    while (play) {
        if (lastHour != date.GetHour()) {
            lastHour = date.GetHour();
            if (lastHour % 5 == 0)
                hourlyUpdate();
        }

        if (lastDay != date.GetDay()) {
            lastDay = date.GetDay();
            dailyUpdate();
            updateAi();
        }

        if (lastMonth != date.GetMonth()) {
            for (auto w : wars) {
                w->IncreaseMonth();
            }

            lastMonth = date.GetMonth();
            monthlyUpdate();
        }

        if (kbhit())
            play = false;

        sendPackets();
        receivePackets();
        manageTime();
    }

    sf::Packet packet;
    packet << "End";
    for (auto& client : clients) client->send(packet);
}

void Game::updateAi()
{
    LogFunkStart;
    for (auto& c : countries) {
        if (!c->IsOwnedByBoot() || !c->IsActive())
            continue;

        ai_newUnits(c);
        ai_manageProvincesOfInterest(c);
        ai_warDecisions(c);
        ai_units(c);
    }
    LogFunkEnd;
}

void Game::ai_newUnits(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    float clearIncome = c->GetIncome() - c->GetArmyMaintenance();
    if (c->GetGold() > 1.0f && clearIncome > 0.3f) {
        float monthlyUnitCost = 0.1f;  // for 100 soldiers
        float priceForHundred = 0.1f;  // unit cost
        float affordFromIncome = clearIncome * monthlyUnitCost * 1000.0f;
        float affordFromCash = c->GetGold() * priceForHundred * 1000.0f;
        int unitSize = 0;

        if (affordFromIncome > affordFromCash) {
            unitSize = affordFromCash;
        }
        else {
            unitSize = affordFromIncome;
        }

        if (unitSize <= 0)
            return;

        auto provs = c->GetProvs();
        for (auto& prov : provs) {
            if (prov->GetManpower() > 0 && prov->GetSiegeLevel() == 0) {
                int temp = unitSize - prov->GetManpower();

                if (temp < 0) {
                    temp = unitSize;
                    unitSize = 0;
                }
                else {
                    temp = prov->GetManpower() - temp;
                    unitSize -= temp;
                }

                c->EraseMoney((float)temp / (priceForHundred * 1000.0f));
                auto unit = std::make_shared<Unit>(prov->GetName() + " unit", prov->GetUnitPos(), temp,
                                                   c->GetName(), prov->GetId(), c->GetId(), prov->GetId());
                DoTheThing::AddUnit(unit, prov, c);
                DoTheThing::SendNewUnitInfo(unit, toSend);
            }

            if (unitSize == 0) {
                float am = calculateArmyMaintenance(c);
                c->SetArmyMaintenance(am);
                break;
            }
        }
    }
    LogFunkEnd;
}

void Game::ai_manageProvincesOfInterest(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    if (lastDay != 1)  // monthly update
        return;

    auto poi = c->GetPoI();
    for (auto p : poi) {
        assert(p >= 0 && p < provinces.size());
        auto prov = provinces.at(p);
        assert(prov->GetCountryId() >= 0 && prov->GetCountryId() < countries.size());
        auto provC = countries.at(prov->GetCountryId());
        Log("POI : " << c->GetName() << " -> " << prov->GetName());

        if (provC->GetName() == c->GetName()) {
            c->ErasePoI(p);
            continue;
        }

        if (provC->GetRel(c->GetId()) > 15) {
            c->ErasePoI(p);
        }
    }

    // find new poi
    if (poi.size() > 3)
        return;
    auto cProvs = c->GetProvs();
    for (auto& prov : cProvs) {
        auto neighb = prov->GetNeighbours();
        for (auto n : neighb) {
            assert(n >= 0 && n < provinces.size());
            auto nProv = provinces.at(n);
            if (nProv->GetCountryId() == c->GetId())
                continue;
            assert(nProv->GetCountryId() >= 0 && nProv->GetCountryId() < countries.size());
            auto nProvCountry = countries.at(nProv->GetCountryId());
            if (nProvCountry->GetRel(c->GetId()) <= 0 && c->GetRel(nProvCountry->GetId()) <= 0 &&
                std::rand() % 10 == 0) {
                c->AddPoI(nProv->GetId());
            }
        }
        if (c->GetPoISize() >= 3)
            break;
    }
    LogFunkEnd;
}

void Game::ai_warDecisions(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    if (lastDay != 1)  // monthly update
        return;
    auto warIt = std::find_if(wars.begin(), wars.end(), [cId = c->GetId()](War* war) {
        return (war->IsAttacker(cId) || war->IsDefender(cId));
    });
    if (warIt == wars.end()) {  // try to go into war
        ai_goToWar(c);
    }
    else {  // try to make peace
        ai_makePeace(c);
    }
    LogFunkEnd;
}

void Game::ai_goToWar(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    auto poi = c->GetPoI();
    for (auto p : poi) {
        assert(p >= 0 && p < provinces.size());
        auto prov = provinces.at(p);
        assert(prov->GetCountryId() >= 0 && prov->GetCountryId() < countries.size());
        auto provCtr = countries.at(prov->GetCountryId());

        if (c->IsTruce(provCtr->GetId()))
            continue;

        if (provCtr->GetPower() >= c->GetPower())
            continue;

        auto isEnemyAtWar = std::find_if(wars.begin(), wars.end(), [cId = provCtr->GetId()](War* war) {
            return (war->IsAttacker(cId) || war->IsDefender(cId));
        });
        if (isEnemyAtWar != wars.end())
            continue;

        if (std::rand() % 3 != 0) {
            DoTheThing::DeclareWar(countries, c->GetId(), provCtr->GetId(), wars, toSend);
            break;
        }
    }
    LogFunkEnd;
}

void Game::ai_makePeace(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    for (auto w : wars) {
        bool isAttacker = w->IsAttacker(c->GetId());
        bool isDefender = w->IsDefender(c->GetId());
        if (!isAttacker && !isDefender)
            continue;

        int enemyId = -1;
        if (isDefender)
            enemyId = w->GetAttackerId();
        if (isAttacker)
            enemyId = w->GetDefenderId();
        assert(enemyId >= 0 && enemyId < countries.size());
        auto enemy = countries.at(enemyId);

        int myWarScore = w->GetAttackerWarScore();
        if (isDefender)
            myWarScore *= -1;

        Log("WarInfo: " << c->GetName() << " -> " << enemy->GetName() << ", score=" << myWarScore);

        if (myWarScore > 20) {
            auto poi = c->GetPoI();
            std::vector<std::pair<int, int>> offeredProvinces;  // provId, recipant ctrId

            for (auto& p : poi) {
                assert(p >= 0 && p < provinces.size());
                auto prov = provinces.at(p);
                bool isPossible = false;
                if (isAttacker)
                    isPossible = w->IsDefender(prov->GetCountryId());
                else
                    isPossible = w->IsAttacker(prov->GetCountryId());

                if (isPossible) {
                    offeredProvinces.emplace_back(std::make_pair(prov->GetId(), c->GetId()));
                    Log(prov->GetName() << "  " << prov->GetCountry());
                }
            }

            // search for provinces not in poi
            auto enemyProvs = enemy->GetProvs();
            for (auto& prov : enemyProvs) {
                if (prov->GetSiegeLevel() == 100) {
                    if (w->IsAttacker(prov->GetSiegeCountryId()) || w->IsDefender(prov->GetSiegeCountryId())) {
                        poi.push_back(prov->GetId());
                        c->AddPoI(prov->GetId());
                        if (poi.size() >= 3)
                            break;
                    }
                }
            }

            PeaceOffer peaceOffer;
            peaceOffer.warId = w->GetId();
            peaceOffer.warScore = w->GetAttackerWarScore();
            if (isAttacker)
                peaceOffer.recipant = w->GetDefenderId();
            else
                peaceOffer.recipant = w->GetAttackerId();
            peaceOffer.recipantIsDefender = isAttacker;
            peaceOffer.attackers = w->GetAttackers();
            peaceOffer.defenders = w->GetDefenders();
            peaceOffer.offeredBy = c->GetId();

            assert(peaceOffer.recipant >= 0 && peaceOffer.recipant < countries.size());
            auto recipant = countries.at(peaceOffer.recipant);

            auto warIt = std::find_if(wars.begin(), wars.end(),
                                      [wId = w->GetId()](War* war) { return wId == war->GetId(); });
            if (warIt == wars.end())
                continue;
            DoTheThing::SendPeace(wars, provinces, countries, peaceOffer, offeredProvinces, toSend, recipant,
                                  warIt, date, offersForHumans);
        }
    }
    LogFunkEnd;
}

void Game::ai_mergeUnits(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    bool merged = false;
    do {
        merged = false;
        auto units = c->GetUnits();
        for (auto& u : units) {
            if (u->IsInBattle())
                continue;
            std::vector<std::shared_ptr<Unit>> unitsToErase;

            for (auto& uu : units) {
                if (uu->IsInBattle())
                    continue;
                if (u->GetId() != uu->GetId() && u->GetProvId() == uu->GetProvId()) {
                    unitsToErase.push_back(uu);
                }
            }

            if (unitsToErase.size()) {
                assert(u->GetProvId() >= 0 && u->GetProvId() < provinces.size());
                auto prov = provinces.at(u->GetProvId());
                DoTheThing::MergeUnits(unitsToErase, u, c, prov, toSend);
                merged = true;
                break;
            }
        }
    } while (merged);
    LogFunkEnd;
}

void Game::ai_units(std::shared_ptr<Country>& c)
{
    LogFunkStart;
    auto units = c->GetUnits();
    for (auto& u : units) {
        std::vector<int> atWarWith;
        for (auto w : wars) {
            if (w->IsAttacker(c->GetId())) {
                auto temp = w->GetDefenders();
                atWarWith.insert(atWarWith.end(), temp.begin(), temp.end());
            }
            else if (w->IsDefender(c->GetId())) {
                auto temp = w->GetAttackers();
                atWarWith.insert(atWarWith.end(), temp.begin(), temp.end());
            }
        }

        bool merge = ai_unitTryWalkToMerge(u, c);
        bool walkToFight = false;
        if (!merge)
            walkToFight = ai_unitWalkToFight(u, atWarWith);
        if (!merge && !walkToFight)
            ai_unitWalkToSiege(u, atWarWith);
    }

    ai_mergeUnits(c);
    LogFunkEnd;
}

bool Game::ai_unitTryWalkToMerge(std::shared_ptr<Unit>& u, std::shared_ptr<Country>& c)
{
    LogFunkStart;
    if (u->IsMoving() || u->IsSieging() || u->IsInBattle())
        return false;
    auto units = c->GetUnits();
    for (auto& uu : units) {
        if (uu->GetId() == u->GetId())
            continue;

        auto uPos = u->GetFakePos();
        assert(u->GetProvId() >= 0 && u->GetProvId() < provinces.size());
        auto uProv = provinces.at(u->GetProvId());
        if (std::rand() % 5 == 0) {
            auto uuPos = uu->GetFakePos();
            assert(uu->GetProvId() >= 0 && uu->GetProvId() < provinces.size());
            auto uuProv = provinces.at(uu->GetProvId());
            if (uuProv->GetId() != uProv->GetId()) {
                DoTheThing::MoveUnit(u, provinces, uProv, uuProv);
                return true;
            }
        }
    }
    LogFunkEnd;
    return false;
}

bool Game::ai_unitWalkToFight(std::shared_ptr<Unit>& u, const std::vector<int>& atWarWith)
{
    LogFunkStart;
    if (u->IsMoving() || u->IsSieging() || std::rand() % 2 == 0)
        return false;

    for (auto opponentId : atWarWith) {
        assert(opponentId >= 0 && opponentId < countries.size());
        auto opponent = countries.at(opponentId);
        auto units = opponent->GetUnits();
        for (auto& uu : units) {
            if (u->GetSoldiers() > uu->GetSoldiers() && std::rand() % 5 == 0) {
                assert(uu->GetProvId() >= 0 && uu->GetProvId() < provinces.size());
                auto uuProv = provinces.at(uu->GetProvId());
                assert(u->GetProvId() >= 0 && u->GetProvId() < provinces.size());
                auto uProv = provinces.at(u->GetProvId());

                DoTheThing::MoveUnit(u, provinces, uProv, uuProv);

                return true;
            }
        }
    }
    LogFunkEnd;
    return false;
}

void Game::ai_unitWalkToSiege(std::shared_ptr<Unit>& u, const std::vector<int>& atWarWith)
{
    LogFunkStart;
    if (atWarWith.size() == 0 || u->IsMoving() || u->IsSieging() || u->IsInBattle())
        return;

    bool provFound = false;
    float dist = 0.0f;

    int provEndId = -1;
    for (auto& opponentId : atWarWith) {
        assert(opponentId >= 0 && opponentId < countries.size());
        auto opponent = countries.at(opponentId);
        auto provs = opponent->GetProvs();
        for (auto& prov : provs) {
            if (prov->GetSiegeCountryId() == u->GetCountryId())
                continue;
            if (prov->GetId() == u->GetProvId())
                continue;

            float tmpDist = pow(prov->GetUnitPos().x - u->GetFakePos().x, 2);
            tmpDist += pow(prov->GetUnitPos().y - u->GetFakePos().y, 2);
            tmpDist = sqrt(tmpDist);

            if (tmpDist < dist || !provFound) {
                provFound = true;
                dist = tmpDist;
                provEndId = prov->GetId();
            }
        }
    }
    assert(u->GetCountryId() >= 0 && u->GetCountryId() < countries.size());
    auto ownProvs = countries.at(u->GetCountryId())->GetProvs();
    for (auto& prov : ownProvs) {
        if (prov->GetSiegeLevel() > 0) {
            float tmpDist = pow(prov->GetUnitPos().x - u->GetFakePos().x, 2);
            tmpDist += pow(prov->GetUnitPos().y - u->GetFakePos().y, 2);
            tmpDist = sqrt(tmpDist);

            if (tmpDist < dist || !provFound) {
                provFound = true;
                dist = tmpDist;
                provEndId = prov->GetId();
            }
        }
    }
    if (provFound) {
        assert(u->GetProvId() >= 0 && u->GetProvId() < provinces.size());
        assert(provEndId >= 0 && provEndId < provinces.size());
        DoTheThing::MoveUnit(u, provinces, provinces.at(u->GetProvId()), provinces.at(provEndId));
    }
    LogFunkEnd;
}

void Game::hourlyUpdate()
{
    LogFunkStart;
    for (auto& cl : clients) {
        Packet packet(false);
        packet.PushRecipant(cl->GetCountry()->GetId());
        packet << "hourly";

        std::vector<std::shared_ptr<Unit>> unitsToSend;
        for (auto& country : countries) {
            int ok = 0;
            if (country->GetId() == cl->GetCountry()->GetId()) {
                ok = 1;
            }
            else {
                for (auto w : wars) {
                    if (w->ShouldTheyFight(country->GetId(), cl->GetCountry()->GetId())) {
                        ok = 1;
                        break;
                    }
                }
                if (cl->GetCountry()->GetName() == "Spectator")
                    ok = 1;
            }

            if (ok == 0)
                continue;

            auto units = country->GetUnits();
            unitsToSend.insert(unitsToSend.end(), units.begin(), units.end());
        }

        packet << (int)unitsToSend.size();
        for (auto& u : unitsToSend) {
            packet << u->GetId();
            packet << u->GetCountryId();
            packet << u->GetName();
            packet << u->GetSoldiers();
            packet << u->GetPos().x;
            packet << u->GetPos().y;
            packet << u->GetPos().z;
            packet << u->GetProvId();
            auto moves = u->GetMoves();
            packet << (int)moves.size();
            for (auto& m : moves) {
                packet << m.destiny.x;
                packet << m.destiny.y;
                packet << m.destiny.z;
            }
        }
        unitsToSend.clear();

        std::vector<int> battlesToSend;
        for (auto& battle : battles) {
            if (battle.IsCountryInFight(cl->GetCountry()->GetId())) {
                battlesToSend.push_back(battle.GetId());
                continue;
            }
            else {
                assert(battle.GetProvId() >= 0 && battle.GetProvId() < provinces.size());
                auto prov = provinces.at(battle.GetProvId());
                if (prov->GetCountryId() == cl->GetCountry()->GetId()) {
                    battlesToSend.push_back(battle.GetId());
                    continue;
                }
                else if (cl->GetCountry()->GetName() == "Spectator")
                    battlesToSend.push_back(battle.GetId());
            }
        }
        packet << (int)battlesToSend.size();
        for (auto& battle : battles) {
            for (auto bId : battlesToSend) {
                if (battle.GetId() == bId) {
                    packet << battle.GetId();
                    packet << battle.GetProvId();
                    packet << battle.GetAttackersSize();
                    packet << battle.GetDefendersSize();
                    auto atts = battle.GetAttackers();
                    auto defs = battle.GetDefenders();

                    packet << (int)atts.size();
                    packet << (int)defs.size();
                    for (auto& a : atts) packet << a->GetId();
                    for (auto& d : defs) packet << d->GetId();
                    break;
                }
            }
        }
        toSend.push_back(packet);
    }
    LogFunkEnd;
}

void Game::dailyUpdate()
{
    LogFunkStart;
    for (auto& country : countries) {
        unitsUpdate(country);
    }
    provincesUpdate();
    battlesUpdate();
    siegingUpdate();
    for (auto& c : countries) {
        c->DeleteExpiredTruces(date);
    }
    for (auto& cl : clients) {
        Packet packet;
        packet << "daily";

        int counter = 1;

        packet << counter;

        auto c = cl->GetCountry();

        packet << c->GetName();

        packet << c->GetGold();
        packet << c->GetIncome();
        packet << c->GetArmyMaintenance();

        packet << c->GetManpower();
        packet << c->GetManpowerRecovery();

        packet << date.GetDate();
        packet << date.GetSpeed();

        auto cProvs = c->GetProvs();
        int provCount = cProvs.size();
        packet << provCount;
        for (auto& prov : cProvs) {
            packet << prov->GetId();
            packet << prov->GetManpower();
            packet << prov->GetDevelopment();
            packet << prov->GetTreasury();
            packet << prov->GetMonthIncome();
            packet << prov->GetTotalMonthIncome();
        }

        packet << (int)wars.size();
        for (auto w : wars) {
            packet << w->GetId();
            packet << w->GetAttackerWarScore();

            auto attackers = w->GetAttackers();
            packet << (int)attackers.size();
            for (auto a : attackers) packet << a;
            auto defenders = w->GetDefenders();
            packet << (int)defenders.size();
            for (auto d : defenders) packet << d;
        }

        int siegedProvincesCount = 0;
        for (auto& prov : provinces)
            if (prov->GetSiegeLevel() != 0)
                siegedProvincesCount++;

        packet << siegedProvincesCount;
        for (auto& prov : provinces) {
            if (prov->GetSiegeLevel() != 0) {
                packet << prov->GetId();
                packet << prov->GetSiegeLevel();
                packet << prov->GetSiegeCountryId();
                packet << prov->GetSiegeSoldiersCount();
            }
        }

        packet.PushRecipant(c->GetId());
        toSend.push_back(packet);
    }
    LogFunkEnd;
}

void Game::provincesUpdate()
{
    LogFunkStart;
    for (auto& c : countries) {
        c->UpdateSiegesForWar();
    }
    LogFunkEnd;
}

void Game::unitsUpdate(std::shared_ptr<Country>& country)
{
    LogFunkStart;
    auto units = country->GetUnits();
    std::vector<std::vector<std::shared_ptr<Unit>>::iterator> toErase;
    for (auto it = units.begin(); it < units.end(); it++) {
        assert((*it)->GetProvId() >= 0 && (*it)->GetProvId() < provinces.size());
        auto currentProv = provinces.at((*it)->GetProvId());
        bool unitMoved = (*it)->Update();
        if (unitMoved) {
            currentProv->EraseUnit(*it);
            assert((*it)->GetProvId() >= 0 && (*it)->GetProvId() < provinces.size());
            auto newProv = provinces.at((*it)->GetProvId());
            newProv->AddUnit(*it);
        }
        int killed = (*it)->GetAndResetKilled();
        if (killed) {
            auto fromProvince = (*it)->GetFromProvinceId();
            assert(fromProvince >= 0 && fromProvince < provinces.size());
            auto prov = provinces.at(fromProvince);
            prov->KillPopulation(killed);
            Packet packet{true};
            packet << "ProvincePopulation";
            packet << prov->GetId();
            packet << prov->GetPopulation();
            toSend.emplace_back(packet);
        }
        if ((*it)->GetSoldiers() <= 0)
            toErase.push_back(it);
    }

    for (int i = toErase.size() - 1; i >= 0; i--) {
        assert((*toErase[i])->GetProvId() >= 0 && (*toErase[i])->GetProvId() < provinces.size());
        auto prov = provinces.at((*toErase[i])->GetProvId());
        DoTheThing::EraseUnit(*toErase[i], prov, country, toSend);
    }
    LogFunkEnd;
}

void Game::siegingUpdate()
{
    LogFunkStart;
    for (auto& prov : provinces) {
        auto units = prov->GetUnits();
        for (auto& u : units) {
            for (auto w : wars) {
                if (w->ShouldTheyFight(u->GetCountryId(), prov->GetCountryId())) {
                    prov->Sieging(u);
                    u->SetSieging(true);
                    break;
                }
            }
        }
        prov->UpdateSiege();
    }
    LogFunkEnd;
}

void Game::battlesUpdate()
{
    LogFunkStart;
    std::vector<std::vector<Battle>::iterator> toErase;
    for (auto it = battles.begin(); it < battles.end(); it++) {
        it->Update();
        if (it->IsEnded())
            toErase.push_back(it);
    }
    for (int i = toErase.size() - 1; i >= 0; i--) {
        auto it = toErase[i];
        int winnerId = it->GetWinnerId();
        if (winnerId != -1) {
            for (auto war : wars) {
                if (war->GetId() == it->GetWarId()) {
                    war->AddBattleWinner(winnerId);
                    break;
                }
            }
        }
        battles.erase(it);
    }

    for (auto& province : provinces) {
        auto units = province->GetUnits();
        for (auto& unit : units) {
            if (unit->IsInBattle())
                continue;
            bool isInFight = false;
            for (auto& battle : battles) {
                if (unit->GetProvId() != battle.GetProvId())
                    continue;
                isInFight = battle.IsInFight(unit);

                if (!isInFight && battle.ShouldFight(unit)) {
                    isInFight = true;
                    battle.AddUnit(unit);
                    break;
                }
            }
            if (!isInFight) {
                for (auto& uu : units) {
                    if (unit->GetId() == uu->GetId())
                        continue;
                    for (auto war : wars) {
                        if (war->ShouldTheyFight(unit->GetCountryId(), uu->GetCountryId()) == false)
                            continue;

                        if (unit->IsInBattle() || uu->IsInBattle())
                            continue;

                        auto provPos = province->GetUnitPos();
                        Battle battle{war->GetId(), provPos, province->GetId()};
                        battle.AddAttacker(unit);
                        battle.AddDefender(uu);
                        battles.push_back(battle);
                        break;
                    }
                }
            }
        }
    }
    LogFunkEnd;
}

void Game::monthlyUpdate()
{
    LogFunkStart;
    for (auto w : wars) {
        w->IncreaseMonth();
    }

    for (auto& c : countries) {
        auto mc = c->MonthUpdate();
        mc.armyMaintenance = calculateArmyMaintenance(c);
        c->Cash(mc);
    }
/*
    Packet packet{false};
    packet << "monthly";
    packet << (int)countries.size();
    for (auto& c : countries) {
        auto relations = c->GetRelations();
        packet << c->GetName();
        packet << (int)relations.size();
        for (auto& r : relations) {
            packet << r.first;
            packet << r.second;
        }
    }
    for (auto& c : clients) {
        auto cc = c->GetCountry();
        auto imprRel = cc->GetImprRel();
        Packet newP = packet;
        newP.PushRecipant(cc->GetId());
        newP << (int)imprRel.size();
        for (auto& r : imprRel) {
            newP << r;
            Log(cc->GetName() << " " << r);
        }
        toSend.emplace_back(newP);
    }
    */
    LogFunkEnd;
}

float Game::calculateArmyMaintenance(std::shared_ptr<Country>& country)
{
    LogFunkStart;
    float hundredCost = 0.1f;
    float result = 0.0f;
    auto units = country->GetUnits();
    for (auto& u : units) {
        result += (float)u->GetSoldiers() / 100.0f * hundredCost;
    }
    LogFunkEnd;
    return result;
}

void Game::manageTime()
{
    dt = clock.getElapsedTime().asSeconds();
    float frameGoal = 60.0f;
#if 1
    if (dt < 1.0f / frameGoal) {
        float timeForSleep = (1.0f / frameGoal - dt) * 1000.0f;
        std::this_thread::sleep_for(std::chrono::milliseconds((int)timeForSleep));
        dt = clock.getElapsedTime().asSeconds();
    }
#endif
    clock.restart();
    date.Update(dt);
}

void Game::sendPackets()
{
    for (auto& p : toSend) {
        for (auto& cl : clients) {
            if (p.IsRecipant(cl->GetCountry()->GetId())) {
                cl->send(p);
            }
        }
    }
    toSend.clear();
}

void Game::receivePackets()
{
    for (auto& client : clients) {
        sf::Packet packet;
        while (client->receive(packet) == sf::Socket::Done) {
            processPacket(client, packet);
        }
    }
}

void Game::processPacket(std::shared_ptr<Client> client, sf::Packet& packet)
{
    std::string type;
    packet >> type;
    Log(type);

    if (type == "NewUnit") {
        ProcessPacket::NewUnit(packet, client, countries, provinces, toSend);
    }
    else if (type == "UnitMove") {
        ProcessPacket::UnitMove(packet, client, provinces);
    }
    else if (type == "DeclareWar") {
        ProcessPacket::DeclareWar(packet, client, countries, wars, toSend);
    }
    else if (type == "PeaceOffer") {
        ProcessPacket::OfferPeace(packet, client, wars, provinces, countries, toSend, date, offersForHumans);
    }
    else if (type == "MergeUnits") {
        ProcessPacket::MergeUnits(packet, client, provinces, toSend);
    }
    else if (type == "Speed") {
        int val;
        packet >> val;
        if (val > 0 && val <= 5) {
            date.Speed(val);
            Packet newPack{true};
            newPack << "Speed";
            newPack << date.GetSpeed();
            toSend.emplace_back(newPack);
        }
    }
    else if (type == "Stop") {
        Packet newPack{true};
        newPack << "Pause";
        if (date.IsPaused())
            date.SetPause(false);
        else
            date.SetPause(true);
        newPack << date.IsPaused();
        toSend.emplace_back(newPack);
    }
    else if (type == "StartImprRel") {
        ProcessPacket::StartImprRel(packet, countries);
    }
    else if (type == "StopImprRel") {
        ProcessPacket::StopImprRel(packet, countries);
    }
    else if (type == "AcceptPeace") {
        ProcessPacket::AcceptedPeace(packet, countries, wars, provinces, offersForHumans, toSend);
    }
    else if (type == "DeclinePeace") {
        ProcessPacket::DeclinePeace(packet, offersForHumans, clients, toSend);
    }
}

void Game::createUnit(int countryId, int provId, int unitSize)
{
    LogFunkStart;
    assert(countryId >= 0 && countryId < countries.size());
    auto country = countries.at(countryId);

    assert(provId >= 0 && provId < provinces.size());
    auto prov = provinces.at(provId);

    if (countryId != prov->GetCountryId())
        return;

    if (prov->GetManpower() < unitSize)
        return;

    prov->EraseManpower(unitSize);
    country->EraseManpower(unitSize);

    if (country->GetGold() >= unitSize / 100) {
        country->EraseMoney(unitSize / 100);
        auto unit = std::make_shared<Unit>(prov->GetName() + " unit", prov->GetUnitPos(), unitSize,
                                           country->GetName(), provId, countryId, provId);
        DoTheThing::AddUnit(unit, prov, country);
        Packet newPack(true);
        newPack << "NewUnit";  // type
        newPack << unit->GetId();
        newPack << unit->GetName();
        newPack << unit->GetCountry();
        newPack << unit->GetMorale();
        newPack << unit->GetSoldiers();
        newPack << unit->GetFakePos().x;
        newPack << unit->GetFakePos().y;
        newPack << unit->GetFakePos().z;
        toSend.push_back(newPack);
    }
    LogFunkEnd;
}

