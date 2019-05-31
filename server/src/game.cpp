#include "game.h"


Game::Game(std::vector<std::shared_ptr<Client>> & clients, std::vector<std::shared_ptr<Country>> & countries)
    : clients(clients), countries(countries)
{
    Log("Game()");
    // -- load resources
    
    //countries = CountryLoader::Load();
    provinces = ProvinceLoader::Load(map.GetPixels(), map.GetW(), map.GetH());
    // ---
    
    for (auto & client : clients) {
        if (client->GetName() == "boot")
            continue;
        
        std::string name = client->GetCountryName();
        
        auto it = std::find_if(countries.begin(), countries.end(), [name](const std::shared_ptr<Country>c){ return c->GetName() == name; });
        if (it != countries.end()) {
            (*it)->OwnByHuman(true);
            client->SetCountry(*it);
        }
    }
    srand(time(NULL));
}

Game::~Game()
{
}

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
            //hourlyUpdate();
            lastDay = date.GetDay();
            dailyUpdate();
            updateAi();
        }

        if (lastMonth != date.GetMonth()) {
            for (auto & w : wars) {
                w.IncreaseMonth();
            }

            for (auto & c : countries) {
                auto poi = c->GetPoI();
                for (auto p : poi) {
                    auto pIt = std::find_if(provinces.begin(), provinces.end(), [p](Province & pr) { return p == pr.GetId(); });
                    if (pIt == provinces.end())
                        continue;
                }
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
    for (auto & client : clients)
        client->send(packet);
}

void Game::updateAi()
{
    for (auto & c : countries) {
        if (!c->IsOwnedByBoot() || !c->IsActive())
            continue;

        ai_newUnits(c);
        ai_manageProvincesOfInterest(c);
        ai_warDecisions(c);
        ai_units(c);
    }    
}

void Game::ai_newUnits(std::shared_ptr<Country> & c)
{
    float clearIncome = c->GetIncome() - c->GetArmyMaintenance();
    if (c->GetGold() > 1.0f && clearIncome > 0.3f) {
        float monthlyUnitCost = 0.1f; // for 100 soldiers
        float priceForHundred = 0.1f; // unit cost
        float affordFromIncome = clearIncome * monthlyUnitCost * 1000.0f;
        float affordFromCash = c->GetGold() * priceForHundred * 1000.0f;
        //affordFromCash = 0;
        int unitSize = 0;

        if (affordFromIncome > affordFromCash) {
            unitSize = affordFromCash;
        }
        else {
            unitSize = affordFromIncome;
        }

        if (unitSize <= 0)
            return;

        for (auto & prov : provinces) {
            if (prov.GetCountry() == c->GetName()) {
                if (prov.GetManpower() > 0 && prov.GetSieged() == 0) {
                    int temp = unitSize - prov.GetManpower();

                    if (temp < 0) {
                        temp = unitSize;
                        unitSize = 0;
                    }
                    else {
                        temp = prov.GetManpower() - temp;
                        unitSize -= temp;
                    }

                    c->EraseMoney((float)temp / (priceForHundred * 1000.0f));
                    Unit unit{prov.GetName() + " unit", prov.GetUnitPos(), temp, c->GetName(), prov.GetName()};
                    units.emplace_back(std::make_shared<Unit>(unit));
                    DoTheThing::SendNewUnitInfo(units.back(), toSend);
                }

                if (unitSize == 0) {
                    float am = calculateArmyMaintenance(c->GetName());
                    c->SetArmyMaintenance(am);
                    break;
                }
            }
        }
    }
}

void Game::ai_manageProvincesOfInterest(std::shared_ptr<Country> & c)
{
    if (lastDay != 1) // monthly update
        return;

    auto poi = c->GetPoI();
    for (auto p : poi) {
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [p](Province & prov) {
                       return p == prov.GetId();
                      });
        if (provIt == provinces.end())
            continue;
        Log("POI : " << c->GetName() << " -> " << (*provIt).GetName());
        auto cIt = std::find_if(countries.begin(), countries.end(), [cName = provIt->GetCountry()](std::shared_ptr<Country> & country) {
                    return cName == country->GetName();
                   });

        if (cIt == countries.end())
            continue;

        if ((*cIt)->GetName() == c->GetName()) {
            c->ErasePoI(p);
            continue;
        }

        if ((*cIt)->GetRel(c->GetName()) > 15) {
            c->ErasePoI(p);
        }
    }

    // find new poi
    if (poi.size() > 3)
        return;
    for (auto & prov : provinces) {
        if (prov.GetCountry() != c->GetName())
            continue;
        auto neighb = prov.GetNeighbours();
        for (auto n : neighb) {
            auto provIt = std::find_if(provinces.begin(), provinces.end(), [n](Province & pr) { return n == pr.GetId(); });
            if (provIt == provinces.end())
                continue;
            if (provIt->GetCountry() == c->GetName())
                continue;
            auto cIt = std::find_if(countries.begin(), countries.end(), [cName = provIt->GetCountry()](std::shared_ptr<Country> & country) {
                        return cName == country->GetName();
                       });
            if (cIt == countries.end())
                continue;

            if ((*cIt)->GetRel(c->GetName()) <= 0 && c->GetRel((*cIt)->GetName()) <= 0 && std::rand() % 10 == 0) {
                c->AddPoI(provIt->GetId());
            }
        }
        if (c->GetPoISize() >= 3)
            break;
    }
}

void Game::ai_warDecisions(std::shared_ptr<Country> & c)
{
    if (lastDay != 1) // monthly update
        return;
    auto warIt = std::find_if(wars.begin(), wars.end(), [cName = c->GetName()](War & war) { return (war.IsAttacker(cName) || war.IsDefender(cName)); });
    if (warIt == wars.end()) { //try to go into war
        ai_goToWar(c);
    }
    else { // try to make peace
        ai_makePeace(c);
    }
}

void Game::ai_goToWar(std::shared_ptr<Country> & c)
{
    auto poi = c->GetPoI();
    for (auto p : poi) {
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [p](Province & pr) { return p == pr.GetId(); });
        if (provIt == provinces.end())
            continue;
        auto cIt = std::find_if(countries.begin(), countries.end(), [cName = provIt->GetCountry()](std::shared_ptr<Country> & country) {
                    return cName == country->GetName();
                   });
        if (cIt == countries.end())
            continue;

        if (c->IsTruce((*cIt)->GetName(), date))
            continue;

        if ((*cIt)->GetPower() >= c->GetPower())
            continue;

        auto isEnemyAtWar = std::find_if(wars.begin(), wars.end(), [cName = (*cIt)->GetName()](War & war) {
                             return (war.IsAttacker(cName) || war.IsDefender(cName));
                            });
        if (isEnemyAtWar != wars.end())
            continue;

        if (std::rand() % 3 != 0) {
            DoTheThing::DeclareWar(countries, c->GetName(), (*cIt)->GetName(), wars, toSend, date);
            break;
        }
    }
}

void Game::ai_makePeace(std::shared_ptr<Country> & c)
{
    for (auto & w : wars) {
        bool isAttacker = w.IsAttacker(c->GetName()); 
        bool isDefender = w.IsDefender(c->GetName()); 
        if (!isAttacker && !isDefender)
            continue;

        int enemyPower = 0;
        std::string enemyName = "";
        if (isDefender)
            enemyName = w.GetAttackerName();
        if (isAttacker)
            enemyName = w.GetDefenderName();
        auto enemyIt = std::find_if(countries.begin(), countries.end(), [enemyName](std::shared_ptr<Country> & ctr) {
                        return enemyName == ctr->GetName();
                       });
        if (enemyIt == countries.end())
            continue;
        else
            enemyPower = (*enemyIt)->GetPower();
                    
        int myPower = c->GetPower();
        int myWarScore = w.GetAttackerWarScore();
        if (isDefender)
            myWarScore *= -1;

        Log("WarInfo: " << c->GetName() << " -> " << (*enemyIt)->GetName() << ", score=" << myWarScore);
        
        bool peaceSended = false;
        if (myWarScore > 20) {
            auto poi = c->GetPoI();
            std::vector<std::pair<std::string,std::string>> offeredProvinces;
            for (auto & p : poi) {
                auto pIt = std::find_if(provinces.begin(), provinces.end(), [p](Province & pr) { return p == pr.GetId(); });
                if (pIt == provinces.end())
                    continue;
                bool isPossible = false;
                if (isAttacker) 
                    isPossible = w.IsDefender(pIt->GetCountry());
                else 
                    isPossible = w.IsAttacker(pIt->GetCountry());

                if (isPossible) {
                    offeredProvinces.emplace_back(std::make_pair(pIt->GetName(), c->GetName()));
                    Log(pIt->GetName() << "  " << pIt->GetCountry());
                }
            }

            // tu trzeba wyszukac prowincje ktore nie sa w poi

            for (auto & prov : provinces) {
                if (prov.GetCountry() == (*enemyIt)->GetName()) {
                    if (prov.GetSieged() == 100) {
                        if (w.IsAttacker(prov.GetSiegeCountry()) || w.IsDefender(prov.GetSiegeCountry())) {
                            poi.push_back(prov.GetId());
                            c->AddPoI(prov.GetId());
                            if (poi.size() >= 3)
                                break;
                        }
                    }
                }
            }
            
            PeaceOffer peaceOffer;
            peaceOffer.warId = w.GetId();
            peaceOffer.warScore = w.GetAttackerWarScore();
            if (isAttacker)
                peaceOffer.recipant = w.GetDefenderName();
            else
                peaceOffer.recipant = w.GetAttackerName();
            peaceOffer.recipantIsDefender = isAttacker;
            peaceOffer.attackers = w.GetAttackers();
            peaceOffer.defenders = w.GetDefenders();
            peaceOffer.offeredBy = c->GetName();
                        
            auto recipantIt = std::find_if(countries.begin(), countries.end(), [rec = peaceOffer.recipant](std::shared_ptr<Country> & ctr) {
                               return rec == ctr->GetName();
                              });
            if (recipantIt == countries.end())
                continue;

            auto warIt = std::find_if(wars.begin(), wars.end(), [wId = w.GetId()](War & war) {
                          return wId == war.GetId();
                         });
            DoTheThing::SendPeace(wars, provinces, countries, peaceOffer, offeredProvinces, toSend, recipantIt, warIt, date);
        }
    }
}

void Game::ai_mergeUnits(std::shared_ptr<Country> & c)
{
    bool merged = false;
    do {
        merged = false;
        for (auto & u : units) {
            if (u->GetCountry() != c->GetName())
                continue;

            std::vector<int> idsToErase;

            for (auto & uu : units) {
                if (u->GetCountry() == uu->GetCountry() && u->GetId() != uu->GetId() && u->GetPos() == uu->GetPos()) {
                    u->Add(uu);
                    idsToErase.push_back(uu->GetId());
                }
            }

            if (idsToErase.size()) {
                DoTheThing::MergeUnits(units, u->GetId(), idsToErase, toSend);
                merged = true;
                break;
            }
        }
    } while (merged);
}

void Game::ai_units(std::shared_ptr<Country> & c)
{
    for (auto & u : units) {
        if (u->GetCountry() != c->GetName())
            continue;
        std::vector<std::string> atWarWith;
        for (auto & w : wars) {
            if (w.IsAttacker(c->GetName())) {
                auto temp = w.GetDefenders();
                atWarWith.insert(atWarWith.end(), temp.begin(), temp.end());
            }
            else if (w.IsDefender(c->GetName())) {
                auto temp = w.GetAttackers();
                atWarWith.insert(atWarWith.end(), temp.begin(), temp.end());
            }            
        }

        bool merge = ai_unitTryWalkToMerge(u);
        bool walkToFight = false;
        if (!merge)
            walkToFight = ai_unitWalkToFight(u, atWarWith);
        if (!merge && !walkToFight)
            ai_unitWalkToSiege(u, atWarWith);
    }
        
    ai_mergeUnits(c);
}

bool Game::ai_unitTryWalkToMerge(std::shared_ptr<Unit> & u)
{
    if (u->IsMoving() || u->IsSieging() || u->IsInBattle())
        return false;
    for (auto & uu : units) {
        if (uu->GetCountry() != u->GetCountry())
            continue;
        if (uu->GetId() == u->GetId())
            continue;

        auto uPos = u->GetFakePos();
        auto uProvIt = std::find_if(provinces.begin(), provinces.end(), [uPos](Province & pr) { return pr.GetUnitPos() == uPos; });
        int uProvId = (*uProvIt).GetId();
                        
        if (std::rand() % 5 == 0) {
        
            auto uuPos = uu->GetFakePos();
            auto uuProvIt = std::find_if(provinces.begin(), provinces.end(), [uuPos](Province & pr) { return pr.GetUnitPos() == uuPos; });
            int uuProvId = (*uuProvIt).GetId();
            if (uuProvId != uProvId) {
                DoTheThing::MoveUnit(u, provinces, uProvId, uuProvId);
                return true;
            }
        }
    }
    return false;
}

bool Game::ai_unitWalkToFight(std::shared_ptr<Unit> & u, std::vector<std::string> & atWarWith)
{
    if (u->IsMoving() || u->IsSieging() || std::rand() % 2 == 0)
        return false;

    for (auto & opponent : atWarWith) {
        for (auto & uu : units) {
                            
            if (uu->GetCountry() == opponent && u->GetSoldiers() > uu->GetSoldiers() && std::rand() % 5 == 0) {
                auto uuProvIt = std::find_if(provinces.begin(), provinces.end(), [uPos = uu->GetPos()](Province & pr) {
                                 return pr.GetUnitPos() == uPos;
                                });
                auto uProvIt = std::find_if(provinces.begin(), provinces.end(), [uPos = u->GetPos()](Province & pr) {
                                return pr.GetUnitPos() == uPos;
                               });
                if (uuProvIt == provinces.end() || uProvIt == provinces.end())
                    continue;

                DoTheThing::MoveUnit(u, provinces, (*uProvIt).GetId(), (*uuProvIt).GetId());

                return true;
            }
        }
    }
    return false;
}

void Game::ai_unitWalkToSiege(std::shared_ptr<Unit> & u, std::vector<std::string> & atWarWith)
{
    if (atWarWith.size() == 0 || u->IsMoving() || u->IsSieging() || u->IsInBattle())
        return;

    bool provFound = false;
    int provId = 0;
    float dist = 0.0f;
    auto uProvIt = std::find_if(provinces.begin(), provinces.end(), [pPos = u->GetFakePos()](const Province & p) {
                    return p.GetUnitPos() == pPos;
                   });
    if (uProvIt == provinces.end())
        return;

    for (auto & opponent : atWarWith) {
        for (auto & prov : provinces) {
            if (prov.GetCountry() != opponent && prov.GetCountry() != u->GetCountry())
                continue;
            if (prov.GetSiegeCountry() == u->GetCountry())
                continue;
                            
            float tmpDist = pow(prov.GetUnitPos().x - uProvIt->GetUnitPos().x, 2);
            tmpDist += pow(prov.GetUnitPos().y - uProvIt->GetUnitPos().y, 2);
            tmpDist = sqrt(tmpDist);

            if (tmpDist < dist || !provFound) {
                provFound = true;
                dist = tmpDist;
                provId = prov.GetId();
            }
        }
    }
    if (provFound) {
        DoTheThing::MoveUnit(u, provinces, (*uProvIt).GetId(), provId);
    }
}


void Game::hourlyUpdate()
{    
    for (auto & cl : clients) {
        bool boot = (cl->GetName() == "boot");
        std::string clientCountry;
        if (!boot)
            clientCountry = cl->GetCountry()->GetName();
        else
            clientCountry = "boot";
        
        Packet packet(false);
        packet.PushRecipant(clientCountry);
        packet << "hourly";

        std::vector<std::shared_ptr<Unit>> unitsToSend;
        for (auto & u : units) {
            
            auto unitCountry = u->GetCountry();
            if (unitCountry == clientCountry || boot || 1) {
                unitsToSend.emplace_back(u);
                continue;
            }
            else {
                auto warIt = std::find_if(wars.begin(), wars.end(), [clientCountry, unitCountry](const War & war) {
                              return war.ShouldTheyFight(clientCountry, unitCountry);
                             });
                if (warIt != wars.end()) {
                    unitsToSend.emplace_back(u);
                    continue;
                }
            }
        }

        packet << (int)unitsToSend.size();
        for (auto & u : unitsToSend) {
            packet << u->GetId();
            packet << u->GetSoldiers();
            packet << u->GetPos().x;
            packet << u->GetPos().y;
            packet << u->GetPos().z;
            auto moves = u->GetMoves();
            if (!boot) {
                packet << (int)moves.size();
                for (auto & m : moves) {
                    auto pos = m.destiny;
                    packet << m.destiny.x;
                    packet << m.destiny.y;
                    packet << m.destiny.z;
                }
            }
            else
                packet << 0;
        }
        unitsToSend.clear(); // doesnt matter, but i dont use it

        std::vector<int> battlesToSend;
        for (auto & battle : battles) {
            if (battle.IsCountryInFight(clientCountry) || boot) {
                battlesToSend.push_back(battle.GetId());
                continue;
            }
            else {
                auto provName = battle.GetProv();
                auto provIt = std::find_if(provinces.begin(), provinces.end(), [provName](Province &p) {
                               return p.GetName() == provName;
                              });
                if (provIt != provinces.end()) {
                    if ((*provIt).GetCountry() == clientCountry) {
                        battlesToSend.push_back(battle.GetId());
                        continue;            
                    }
                }
            }
        }
        packet << (int)battlesToSend.size();
        for (auto & battle : battles) {
            for (auto bId : battlesToSend) {
                if (battle.GetId() == bId) {
                    packet << battle.GetId();
                    packet << battle.GetAttackersSize();
                    packet << battle.GetDefendersSize();
                    break;
                }
            }
        }
        toSend.push_back(packet);
    }
}

void Game::dailyUpdate()
{
    unitsUpdate();
    provincesUpdate();
    battlesUpdate();
    for (auto & c : countries) {
        c->DeleteExpiredTruces(date);
    }
       
    for (auto & cl : clients) {
        bool boot = (cl->GetName() == "boot");
        Packet packet;
        packet << "daily";

        int counter = 0;
        if (boot) {
            for (auto & c : countries) {
                if (c->IsOwnedByBoot())
                    counter++;
            }
        }
        else
            counter = 1;

        packet << counter;

        for (auto & c : countries) {
            if (boot && !c->IsOwnedByBoot())
                continue;
            if (!boot && cl->GetCountry()->GetName() != c->GetName())
                continue;
            
            packet << c->GetName();

            packet << c->GetGold();
            packet << c->GetIncome();
            packet << c->GetArmyMaintenance();
        
            packet << c->GetManpower();
            packet << c->GetManpowerRecovery();

            packet << date.GetDate();
            packet << date.GetSpeed();
            int provCount = 0;
            for (auto & prov : provinces) {
                if (prov.GetCountry() == c->GetName())
                    provCount++;
            }
            packet << provCount;
            for (auto & prov : provinces) {
                if (prov.GetCountry() == c->GetName()) {
                    packet << prov.GetId();
                    packet << prov.GetManpower();
                    packet << prov.GetDevelopment();
                    packet << prov.GetTreasury();
                    packet << prov.GetMonthIncome();
                    packet << prov.GetTotalMonthIncome();
                }
            }

        
            packet << (int)wars.size();
            for (auto & w : wars) {
                packet << w.GetId();
                packet << w.GetAttackerWarScore();

                auto attackers = w.GetAttackers();
                packet << (int)attackers.size();
                for (auto a : attackers)
                    packet << a;

                auto defenders = w.GetDefenders();
                packet << (int)defenders.size();
                for (auto d : defenders)
                    packet << d;
            }

            int siegedProvincesCount = 0;
            for (auto & prov : provinces)
                if (prov.GetSieged() != 0)
                    siegedProvincesCount++;

            packet << siegedProvincesCount;
            for (auto & prov : provinces) {
                if (prov.GetSieged() != 0) {
                    packet << prov.GetName();
                    packet << prov.GetSieged();
                    packet << prov.GetSiegeCountry();
                    packet << prov.GetSiegeSoldiers();
                }
            }
        
            packet.PushRecipant(c->GetName());
            toSend.push_back(packet);
        }
    }
}

void Game::provincesUpdate()
{
    for (auto & prov : provinces) {
        prov.NewDay();
    }
}

void Game::unitsUpdate()
{
    std::vector<int> toErase;
    for (int i = units.size() - 1; i >= 0; i--) {
        units[i]->Update();
        int killed = units[i]->GetAndResetKilled();
        if (killed) {
            auto fromProvince = units[i]->GetFromProvince();
            auto provIt = std::find_if(provinces.begin(), provinces.end(), [fromProvince](const Province & prov) {
                           return prov.GetName() == fromProvince;
                          });
            if (provIt != provinces.end()) {
                (*provIt).KillPopulation(killed);
                Packet packet{true};
                packet << "ProvincePopulation";
                packet << (*provIt).GetId();
                packet << (*provIt).GetPopulation();
                toSend.emplace_back(packet);
            }
        }
        if (units[i]->GetSoldiers() <= 0)
            toErase.push_back(i);
    }

    for (int i = 0; i < toErase.size(); i++) {
        Packet packet{true};
        packet << "DeleteUnit";
        packet << units[toErase[i]]->GetId();
        toSend.emplace_back(packet);
        units.erase(units.begin() + toErase[i]);
    }

    // sieging
    for (auto & unit : units) {
        unit->SetSieging(false);
    }
    for (auto & prov : provinces) {
        prov.SiegeBreak();
    }
    for (auto & unit : units) {
        if (unit->IsMoving())
            continue;
        auto pos = unit->GetFakePos();
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [pos](const Province & prov) {
                       return prov.GetUnitPos() == pos;
                      });
        if (provIt != provinces.end()) {
            int siegePower = unit->GetSiegePower();
            if (unit->IsInBattle())
                siegePower = 0;
            
            for (auto & w : wars) {
                if (w.ShouldTheyFight(unit->GetCountry(), (*provIt).GetCountry())) {
                    bool t = (*provIt).Siege(siegePower, unit->GetCountry(), unit->GetSoldiers());
                    if (t)
                        w.AddProv((*provIt).GetName(), (*provIt).GetCountry());
                    else {
                        unit->SetSieging(true);
                    }
                }
                else if (w.ShouldTheyFight(unit->GetCountry(), (*provIt).GetSiegedBy())) {
                    unit->SetSieging(true);
                    bool t = (*provIt).Unsiege(siegePower, unit->GetSoldiers(), unit->GetCountry());
                    if (t)
                        w.DeleteProv((*provIt).GetName());
                    else {
                        unit->SetSieging(true);
                    }
                }
            }
        }
    }
}

void Game::battlesUpdate()
{
    std::vector<int> toErase;
    for (int i = 0; i < battles.size(); i++) {
        battles[i].Update();
        if (battles[i].IsEnded())
            toErase.push_back(i);
    }
    for (int i = toErase.size() - 1; i >= 0; i--) {
        std::string winner = battles[toErase[i]].GetWinner();
        if (winner != "none") {
            for (auto & war : wars) {
                if (war.GetId() == battles[toErase[i]].GetWarId()) {
                    war.AddBattleWinner(winner);
                    break;
                }
            }
        }
        Packet packet{true};
        packet << "EndBattle";
        packet << battles[toErase[i]].GetId();
        toSend.emplace_back(packet);
        battles.erase(battles.begin() + toErase[i]);
    }

    for (int i = 0; i < units.size(); i++) {
        bool alreadyFight = false;
        for (auto & b : battles) {
            if (b.GetPosition() == units[i]->GetFakePos()) {
                alreadyFight = b.IsInFight(units[i]);
                if (!alreadyFight && b.ShouldFight(units[i])) {
                    b.AddUnit(units[i]);
                    alreadyFight = true;
                    Packet packet{true};
                    packet << "NewUnitInBattle";
                    packet << b.GetId();
                    packet << units[i]->GetId();
                    packet << b.IsAttacker(units[i]->GetId());
                    toSend.emplace_back(packet);
                    // tu trzeba pakiecik doslac

                    // UPDATE: o jaki mi pakiecik chodzilo?
                }
                break;
            }
        }
        if (!alreadyFight)
            for (int j = 0; j < units.size(); j++) {
                if (j == i)
                    continue;
                if (units[i]->GetFakePos() == units[j]->GetFakePos()) {
                    for (auto w : wars) {
                        std::string a = units[i]->GetCountry();
                        std::string b = units[j]->GetCountry();
                        bool newFight = w.ShouldTheyFight(a, b);
                        if (newFight)
                            newFight = !(units[i]->IsInBattle() || units[j]->IsInBattle());
                        // chyba trzeba sprawdzic czy juz nie jest w jakiejs bitwie
                        
                        if (newFight) {
                            glm::vec3 pos = units[i]->GetFakePos();
                            auto provIt = std::find_if(provinces.begin(), provinces.end(), [pos](const Province & p){
                                    return p.GetUnitPos() == pos;
                                });
                            if (provIt == provinces.end())
                                break;

                            Battle battle{w.GetId(), pos, (*provIt).GetName()};
                            battle.AddAttacker(units[i]); // kto atakuje i kto sie broni
                            battle.AddDefender(units[j]); // jest losowe
                            battles.push_back(battle);
                            Packet packet{true};
                            packet << "NewBattle";
                            packet << battle.GetId();
                            packet << w.GetId();
                            packet << pos.x;
                            packet << pos.y;
                            packet << pos.z;
                            packet << (*provIt).GetName();
                            packet << units[i]->GetId();
                            packet << units[j]->GetId();
                            toSend.emplace_back(packet);
                            break; //return;
                        }
                    }
                }
            }                
    }    
}

void Game::monthlyUpdate()
{
    for (auto & w : wars) {
        w.IncreaseMonth();
    }
    
    for (auto & c : countries) {
        
        MonthCash mc;
        for (auto & p : provinces) {
            if (p.GetCountry() == c->GetName() && p.GetSieged() < 100) {
                float tax = 0.3f;
                mc.income += p.MonthUpdate(tax);
                mc.development += p.GetDevelopment();
                mc.population += p.GetPopulation();
                mc.manpower += p.GetManpower();
                mc.populationGrowth += p.GetPopulationGrowth();
                mc.manpowerRecovery += p.GetManpowerRecovery();
            }
            else if (p.GetSiegeCountry() == c->GetName() && p.GetSieged() > 100) {
                float tax = 0.3f;
                mc.income += p.MonthUpdate(tax);
            }
        }

        mc.armyMaintenance = calculateArmyMaintenance(c->GetName());
        
        c->Cash(mc);

        auto imprRel = c->GetImprRel();
        for (auto & ir : imprRel) {
            c->ImprRel(ir);
        }
    }


    Packet packet{false};
    packet << "monthly";
    packet << (int)countries.size();
    for (auto & c : countries) {
        auto relations = c->GetRelations();
        packet << c->GetName();
        packet << (int)relations.size();
        for (auto & r : relations) {
            packet << r.first;
            packet << r.second;
        }
    }
    for (auto & c : clients) {
        auto cc = c->GetCountry();
        auto imprRel = cc->GetImprRel();
        Packet newP = packet;
        newP.PushRecipant(cc->GetName());
        newP << (int)imprRel.size();
        for (auto & r : imprRel) {
            newP << r;
            Log(cc->GetName() << " " << r);
        }
        toSend.emplace_back(newP);
    }
}

float Game::calculateArmyMaintenance(const std::string & country)
{
    float hundredCost = 0.1f;
    float result = 0.0f;
    for (auto & u : units) {
        if (u->GetCountry() == country) {
            result += (float)u->GetSoldiers() / 100.0f * hundredCost;
        }
    }
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
    for (auto & p : toSend) {
        bool found = false;
        for (auto & cl : clients) {
            if (cl->GetName() == "boot")
                continue;
            if (p.IsRecipant(cl->GetCountry()->GetName())) {
                cl->send(p);
                found = true;
            }
        }

        if (!found) {
            for (auto & cl : clients) {
                if (cl->GetName() == "boot") {
                    cl->send(p);
                    break;
                }
            }
        }
        
    }
    toSend.clear();
}

void Game::receivePackets()
{
    for (auto & client : clients) {
        sf::Packet packet;
        while (client->receive(packet) == sf::Socket::Done) {
            processPacket(client, packet);
        }
    }
}

void Game::processPacket(std::shared_ptr<Client> client, sf::Packet & packet)
{
    std::string type;
    packet >> type;

    if (type == "NewUnit") {
        ProcessPacket::NewUnit(packet, client, countries, provinces, units, toSend);
    }
    else if (type == "UnitMove") {
        ProcessPacket::UnitMove(packet, client, provinces, units);
    }
    else if (type == "DeclareWar") {
        ProcessPacket::DeclareWar(packet, client, countries, wars, toSend, date);
    }
    else if (type == "PeaceOffer") {
        ProcessPacket::OfferPeace(packet, client, wars, provinces, countries, toSend, date);
    }
    else if (type == "MergeUnits") {
        ProcessPacket::MergeUnits(packet, client, units, provinces, toSend);
    }
    else if (type == "Speed") {
        bool faster = false;
        packet >> faster;
        date.Speed(faster);
        Packet newPack{true};
        newPack << "Speed";
        newPack << date.GetSpeed();
        toSend.emplace_back(newPack);
    }
    else if (type == "Stop") {
        if (date.IsPaused())
            date.SetPause(false);
        else
            date.SetPause(true);
    }
    else if (type == "StartImprRel") {
        ProcessPacket::StartImprRel(packet, countries);
    }
    else if (type == "StopImprRel") {
        ProcessPacket::StopImprRel(packet, countries);
    }
}

void Game::createUnit(std::string country, std::string prov, int unitSize)
{
    auto countryIt = std::find_if(countries.begin(), countries.end(), [country](const std::shared_ptr<Country> & c){
            return c->GetName() == country;
        });
    if (countryIt == countries.end())
        return;

    auto provIt = std::find_if(provinces.begin(), provinces.end(), [prov](Province &p){
            return p.GetName() == prov;
        });
    if (provIt == provinces.end())
        return;
        
    if (country != (*provIt).GetCountry())
        return;

    if ((*provIt).GetManpower() < unitSize)
        return;

    (*provIt).EraseManpower(unitSize);
    (*countryIt)->EraseManpower(unitSize);
    
    if ((*countryIt)->GetGold() >= unitSize / 100) {
        (*countryIt)->EraseMoney(unitSize / 100);
        Log(prov);
        Unit unit(prov + " unit", (*provIt).GetUnitPos(), unitSize, country, prov);
        units.push_back(std::make_shared<Unit>(unit));

        Packet newPack(true);
        newPack << "NewUnit"; // type
        newPack << unit.GetId();
        newPack << unit.GetName();
        newPack << unit.GetCountry();
        newPack << unit.GetMorale();
        newPack << unit.GetSoldiers();
        newPack << unit.GetFakePos().x;
        newPack << unit.GetFakePos().y;
        newPack << unit.GetFakePos().z;
        toSend.push_back(newPack);
    }        
}
