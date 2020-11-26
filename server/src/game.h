#pragma once
#include <vector>
#include <memory>
#include <algorithm>

#include <cstdlib> // gen random
#include <ctime> // gen random

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include "log.h"
#include "date.h"
#include "country.h"
#include "province.h"
#include "country_loader.h"
#include "province_loader.h"
#include "map.h"
#include "unit.h"
#include "path_finder.h"
#include "war.h"
#include "battle.h"
#include "client.h"
#include "packet.h"
#include "process_packet.h"

#include <chrono>
#include <thread>


//
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
 
inline int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}
//



class Game
{
    
    std::vector<std::shared_ptr<Client>> & clients;
    TimeManager date;
    int lastDay = 50;
    int lastHour = 50;
    int lastMonth = 1;
    sf::Clock clock;
    float dt;

    std::vector<Packet> toSend;
    std::vector<std::shared_ptr<Country>> & countries;
    std::vector<std::shared_ptr<Province>> provinces;
    std::vector<War*> wars;
    std::vector<Battle> battles;
    Map map;

    std::vector<PeaceOffer> offersForHumans;
    
    void hourlyUpdate();

    void dailyUpdate();
    void monthlyUpdate();
    float calculateArmyMaintenance(std::shared_ptr<Country> & country);

    void updateAi();
    void ai_newUnits(std::shared_ptr<Country> & c);
    void ai_manageProvincesOfInterest(std::shared_ptr<Country> & c);
    void ai_warDecisions(std::shared_ptr<Country> & c);
    void ai_goToWar(std::shared_ptr<Country> & c);
    void ai_makePeace(std::shared_ptr<Country> & c);
    void ai_mergeUnits(std::shared_ptr<Country> & c);
    void ai_units(std::shared_ptr<Country> & c);
    bool ai_unitTryWalkToMerge(std::shared_ptr<Unit> & u, std::shared_ptr<Country> & c);
    bool ai_unitWalkToFight(std::shared_ptr<Unit> & u, const std::vector<int> & atWarWith);
    void ai_unitWalkToSiege(std::shared_ptr<Unit> & u, const std::vector<int> & atWarWith);

    void provincesUpdate();
    void unitsUpdate(std::shared_ptr<Country> & country);
    void battlesUpdate();
    void siegingUpdate();
    
    void manageTime();
    void sendPackets();
    void receivePackets();
    void processPacket(std::shared_ptr<Client> client, sf::Packet & p);

    void createUnit(int countryId, int provId, int unitSize);

public:
    Game(std::vector<std::shared_ptr<Client>> & clients, std::vector<std::shared_ptr<Country>> & countries);
    ~Game();

    void Play();
    
};
