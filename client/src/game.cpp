#include "game.h"


Game::Game(Window & win, sf::TcpSocket & sock, std::string countryName, glm::vec2 res, std::vector<std::shared_ptr<Country>> & countries)
    : Scene{win, res}, socket(sock), resolution(res), countries(countries)
{    
    gui.AddWin("src/gui/top_bar.txt");
    gui.AddWin("src/gui/notifications.txt");

    //glUseProgram(shader.GetProgram());
    windowSize = window.GetSize();
    
    provinces = ProvinceLoader::Load(map.GetPixels(), 1920, 1080);
    std::sort(provinces.begin(), provinces.end(), [](const std::unique_ptr<Province> & p1, const std::unique_ptr<Province> & p2) {
        return p1->GetId() < p2->GetId();
    });
    setCountryMap();

    auto it = std::find_if(countries.begin(), countries.end(), [countryName](const std::shared_ptr<Country> c){return c->GetName() == countryName;});
    if (it != countries.end()) {
        myCountry = *it;
        Log(countryName);
    }
    else
        Log("There is no country for you ;(");
}

Game::~Game()
{
}

void Game::setCountryMap()
{
    std::vector<std::pair<std::string, Color>> cColor;
    std::vector<std::pair<Color, std::string>> pColor;
    for (auto & c : countries)
        cColor.push_back(std::make_pair(c->GetName(), c->GetColor()));
    for (auto & p : provinces) {
        if (p->GetSieged() != 0)
            continue;
        pColor.push_back(std::make_pair(p->GetColor(), p->GetCountry()));
    }
    map.DrawCountries(cColor, pColor);

    for (auto & prov : provinces) {
        if (prov->GetSieged() == 100) {
            assert(prov->GetSiegeCountryId() >= 0 && prov->GetSiegeCountryId() < countries.size());
            auto siegeCountry = countries.at(prov->GetSiegeCountryId());
            //auto scIt = std::find_if(countries.begin(), countries.end(), [cccc = prov->GetSiegeCountry()](std::shared_ptr<Country> & ccc) {
            //             return ccc->GetName() == cccc;
            //            });
            //if (scIt != countries.end())
                map.DrawSieged(prov->GetColor(), siegeCountry->GetColor());
        }
    }
}

void Game::Play()
{
    float lastTime = glfwGetTime(), currTime = 0.0f;
    float  displayTime = 0.0f; 
    float framesTime = 0.0f;
    bool display = true;
    int frames = 0;
    while (!window.ShouldClose()) {
        
        
        receivePackets();
        updateGui();
        sendPackets();
        updateBattles();

        for (auto & u : units)
            u->UpdateDt(dt);
        
        glm::mat4  matrix = camera.GetMat();    
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));    

    if (display)
    {
        window.Refresh();
        input();
                map.Draw();

        {
            bool selected = false;
            for (auto & u : units) {
                if (!selected) {
                    if (gui.IsOpen("unit")) {
                        auto winVals = gui.GetWinValues("unit");
                        auto it = winVals.find("id");
                        if (it != winVals.end()) {
                            int uId = std::stoi(it->second);
                            if (uId == u->GetId()) {
                                selected = true;
                                u->Draw(true);
                            }
                        }
                    }
                    if (!selected)
                        u->Draw();
       
                }
                else
                    u->Draw();
            }
        }
        
        //matrix = glm::ortho(0.0f, windowSize.x, 0.0f, windowSize.y);
        //matrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
        //matrix = glm::ortho(0.0f, (float)windowSize.x, 0.0f, (float)windowSize.y);
        matrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);
        glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

        gui.Draw();        
        window.Update();
        frames++;
    }
        dt = glfwGetTime() - currTime;

//        float freeTime = 1.0f / 60.0f - dt;
        //if (freeTime > 0) {
            //std::this_thread::sleep_for(std::chrono::milliseconds((int)(freeTime * 1000.0f)));

 //       }
displayTime += dt;
    framesTime += dt;
if (displayTime < 1.0f / 100.0f) {
    display = false;
}
else {
    //frames ++;
    drawDt = displayTime;
    //Log("DrawDt: " << drawDt);
    displayTime = 0.0f;
    display = true;
}
//Log("Dt: "<<dt );
    if (framesTime >= 1.0f) {
        framesTime = 0.0f;
        Log("FPS: " << frames);
        frames = 0;
    }
        currTime = glfwGetTime();
 //       if (currTime - lastTime >= 1.0f) {
  //          Log(frames);
   //         frames = 0;
    //        lastTime = glfwGetTime();
     //   }
    }   
}

void Game::receivePackets()
{
    sf::Packet packet;
    while (socket.receive(packet) == sf::Socket::Done) {
        processPacket(packet);
    }
}

void Game::processPacket(sf::Packet packet)
{
    std::string type;
    packet >> type;
    
    //if (type != "hourly" && type != "daily")
    //Log(type);
    if (type == "daily") {
        ProcessPacket::DailyUpdate(packet, gui, wars, provinces, countries, map);
    }
    else if (type == "NewUnit") {
        ProcessPacket::NewUnit(packet, units, countries, myCountry->GetName());
    }
    else if (type == "hourly") {
        ProcessPacket::HourlyUpdate(packet, gui, units, battles);
    }
    else if (type == "NewBattle") {
        ProcessPacket::NewBattle(packet, units, battles, provinces);
    }
    else if (type == "EndBattle") {
        ProcessPacket::EndBattle(packet, battles);
    }
    else if (type == "DeleteUnit") {
        ProcessPacket::DeleteUnit(packet, units);
    }
    else if (type == "NewUnitInBattle") {
        ProcessPacket::NewUnitInBattle(packet, units, battles);        
    }
    else if (type == "PeaceAccepted") {
        ProcessPacket::PeaceAccepted(packet, gui, provinces, countries, wars);
        setCountryMap();
    }
    else if (type == "MergeUnits") {
        ProcessPacket::MergeUnits(packet, units);
    }
    else if (type == "ProvincePopulation") {
        ProcessPacket::ProvincePopulation(packet, provinces); 
    }
    else if (type == "NewWar") {
        ProcessPacket::NewWar(packet, gui, wars, myCountry->GetId(), countries); 
    }
    else if (type == "SetGold") {
        ProcessPacket::SetGold(packet, gui, myCountry);
    }
    else if (type == "Speed") {
        ProcessPacket::SetSpeed(packet, gui);
    }
    else if (type == "monthly") {
        ProcessPacket::MonthlyUpdate(packet, myCountry->GetName(), countries);
    }
    else if (type == "BotPeaceOffer") {
        ProcessPacket::BotPeaceOffer(packet, gui, peaceOffers, countries);
    }
    else if (type == "PeaceDeclined") {
        ProcessPacket::PeaceDeclined(packet, gui);
    }
}

void Game::input()
{
    if (window.keys['A']) camera.MoveHor(-1, drawDt);
    if (window.keys['D']) camera.MoveHor(1, drawDt);
    if (window.keys['W']) camera.MoveVer(1, drawDt);
    if (window.keys['S']) camera.MoveVer(-1, drawDt);
    if (window.keys['R']) {
        window.keys['R'] = false;
        gui.Reload();
        gui.AddWin("src/gui/top_bar.txt");
        gui.AddWin("src/gui/notifications.txt");        
    }
    if (window.keys['P']) {
        window.keys['P'] = false;
        sf::Packet packet;
        packet << "Stop";
        toSend.emplace_back(packet);
    }
    
    camera.Update(window.xMouse, window.yMouse);
    
    if (window.scrollOffset) {
        if (!gui.Scroll(window.scrollOffset, window.xMouse, windowSize.y - window.yMouse))
            camera.Scroll(window.scrollOffset);

        window.scrollOffset = 0.0f;
    }

    glm::vec2 mouseInWorld = camera.GetMouseInWorld();

    if (window.mouseL && !window.mouseR) {
        //auto guiEvent = gui.Click(window.xMouse, windowSize.y - window.yMouse);
//std::cout << window.xMouse * (resolution.x / windowSize.x) << " " << (windowSize.y - window.yMouse) * (resolution.y / windowSize.y) << '\n';
	    auto guiEvent = gui.Click(window.xMouse * (resolution.x / windowSize.x), (windowSize.y - window.yMouse) * (resolution.y / windowSize.y));
        bool wasGuiClicked = guiEvent.values.size();
        
        if (!wasGuiClicked) {
            if (!unitClick(mouseInWorld)) {
                if (!provClick(mouseInWorld)) {
                }
            }
        }
        else {
            processGuiEvent(guiEvent);
        }
        window.mouseL = false;
    }
    else if (window.mouseR) {
        try {
            if (gui.IsOpen("unit")) {
                auto values = gui.GetWinValues("unit");
                unitMove(values, mouseInWorld);
            }
        }
        catch(std::exception e) {
            //Log(e.what());
        }
        window.mouseR = false;
    }
    else {
        gui.Hover(window.xMouse * resolution.x / windowSize.x, (windowSize.y - window.yMouse) * (resolution.y / windowSize.y));
      //gui.Hover(window.xMouse, windowSize.y - window.yMouse);
    }
}

bool Game::provClick(glm::vec2 mouseInWorld)
{
    Color provinceColor = map.ClickOnProvince(mouseInWorld.x /4, mouseInWorld.y /4);
    auto provIt = std::find_if(provinces.begin(), provinces.end(), [provinceColor](std::unique_ptr<Province> & p) { return p->GetColor() == provinceColor; });
    if (provIt == provinces.end())
        return false;
    
    //glm::vec3 unitPos{(*provIt)->GetUnitPos(), 0.1};
    auto battleIt = std::find_if(battles.begin(), battles.end(), [pId = (*provIt)->GetId()](const std::unique_ptr<Battle> &b){
        return (b->GetProvId() == pId);
    });

    if (gui.IsOpen("offerPeace")) {
        if ((*provIt)->GetSiegeCountryId() == -1)
            return true;

        DataObj * obj = new DataObj{"label"};
        obj->objects.push_back(new DataObj{"text"});
        obj->objects.back()->values["valueName:"] = "agreement";
        obj->objects.back()->values["content:"] = (*provIt)->GetName();
        obj->objects.back()->values["content:"] += " to ";
        assert((*provIt)->GetSiegeCountryId() >= 0 && (*provIt)->GetSiegeCountryId() < countries.size());
        obj->objects.back()->values["content:"] += countries.at((*provIt)->GetSiegeCountryId())->GetName();
        obj->objects.push_back(new DataObj{"icon"});
        obj->objects.back()->values["clickEvent:"] = "eraseObj";
        obj->objects.back()->values["size:"] = "30 30";
        obj->objects.back()->values["texturePath:"] = "src/img/minus.png";
        gui.AddToList(obj, "offerPeace", "peaceAgreements");
        delete obj;
        return true;
    }

    if (battleIt != battles.end()) {
        GA_OpenBattle battle{gui, battleIt, provIt};
        return true;
    }

    if ((*provIt)->GetSieged() == 0) {
        gui.AddWin("src/gui/province.txt");
        auto values = (*provIt)->GetValues();
        gui.Update(values, "province");
        gui.ObservationOfLastWin((*provIt).get());
        return true;
    }
    else {
        GA_OpenSiegedProv spr(gui, provIt, wars);
        return true;
    }        
}

bool Game::unitClick(glm::vec2 mouseInWorld)
{
    std::vector<std::unordered_map<std::string,std::string>> data;
    Subject * unitForObserver = nullptr;
    for (auto & u : units) {
        if (u->Click(mouseInWorld.x, mouseInWorld.y)) {
            auto d = u->GetValues();
            data.push_back(d);
            unitForObserver = u.get();
        }
    }

    if (data.size() == 1) {
        gui.AddWin("src/gui/unit.txt");
        gui.ObservationOfLastWin(unitForObserver);
        gui.Update(data[0], "unit");
        return true;
    }
    else if (data.size() > 1) {
        gui.AddWin("src/gui/units.txt");
        for (auto & d : data) {
            DataObj * dobj = new DataObj{"label"};
            dobj->objects.push_back(new DataObj{"text"});
            dobj->objects.back()->values["valueName:"] = "unitName";
            dobj->objects.back()->values["content:"] = "";
            for (auto & val : d) {
                if (val.first == "id")
                    dobj->objects.back()->values["content:"] += val.second + " ";
                else if (val.first == "name")
                    dobj->objects.back()->values["content:"] += val.second + " ";
            }            
            dobj->objects.back()->values["clickEvent:"] = "openUnit";
            
            dobj->objects.push_back(new DataObj{"icon"});
            dobj->objects.back()->values["clickEvent:"] = "eraseObj";
            dobj->objects.back()->values["size:"] = "30 30";
            dobj->objects.back()->values["texturePath:"] = "src/img/plus.png";

            gui.AddToList(dobj, "units", "units");
            delete dobj;
        }
        return true;
    }
    return false;
}

void Game::unitMove(std::unordered_map<std::string,std::string> & values, glm::vec2 mouseInWorld)
{
    Color provinceColor = map.ClickOnProvince(mouseInWorld.x /4, mouseInWorld.y /4);
    //auto it = std::find(provinces.begin(), provinces.end(), provinceColor);
    auto it = std::find_if(provinces.begin(), provinces.end(), [provinceColor](std::unique_ptr<Province> & p) { return p->GetColor() == provinceColor; });
    if (it != provinces.end()) {    
        sf::Packet packet;
        packet << "UnitMove";
        for (auto & val : values)
            if (val.first == "id") {
                packet << std::stoi(val.second);
                break;
            }

        packet << (*it)->GetId();

        toSend.push_back(packet);
    }
}

void Game::processGuiEvent(GuiClick & click)
{
    GuiAid * aid = nullptr;
    std::string evType = click.GetEventName();
    if (evType == "openCountry") {
        aid = new GA_OpenCountry(gui, countries, myCountry, wars, click);        
    }
    else if (evType == "newUnit") {
        aid = new GA_NewUnit(click, provinces);
    }
    else if (evType == "decreaseValue") {
        aid = new GA_DecreaseValue(gui, click);
    }
    else if (evType == "increaseValue") {
        aid = new GA_IncreaseValue(gui, click);
    }
    else if (evType == "openUnit") {
        aid = new GA_OpenUnit(gui, click, units);
    }
    else if (evType == "eraseObj") {
        aid = new GA_EraseObj(gui, click);
    }
    else if (evType == "mergeUnits") {
        aid = new GA_MergeUnits(gui, click);
    }
    else if (evType == "declareWar") {
        aid = new GA_DeclareWar(click, countries);
    }
    else if (evType == "offerPeace") {
        aid = new GA_OfferPeace(gui, wars, click, myCountry->GetName());
    }
    else if (evType == "sendPeaceOffer") {
        aid = new GA_SendPeaceOffer(gui, wars, countries, provinces, click);
    }
    else if (evType == "dateSpeed") {
        aid = new GA_DateSpeed(click);
    }
    else if (evType == "startImprRel") {
        aid = new GA_StartImprRel(click, myCountry, countries);
    }
    else if (evType == "stopImprRel") {
        aid = new GA_StopImprRel(click, myCountry, countries);
    }
    else if (evType == "openWar") {
        aid = new GA_OpenWar(gui, click, wars);
    }
    else if (evType == "botPeaceOffer") {
        aid = new GA_BotPeaceOffer(click, gui, peaceOffers, countries, provinces);
    }
    else if (evType == "acceptPeace") {
        aid = new GA_AcceptPeace(click, gui, peaceOffers);
    }
    else if (evType == "declinePeace") {
        aid = new GA_DeclinePeace(click, gui, peaceOffers);
    }

    if (aid != nullptr) {
        auto packets = aid->GetPackets();
        toSend.insert(toSend.end(), packets.begin(), packets.end());
        delete aid;
    }
}

void Game::sendPackets()
{
    for (auto & p : toSend) {
        socket.send(p);
    }
    toSend.clear();
}

void Game::updateBattles()
{
    for (auto & b : battles) {
        b->Update();
    }
}

void Game::updateGui()
{
    /*
    SiegedProvWinD sprwD;
    try {
        sprwD = std::any_cast<SiegedProvWinD>(gui.GetWinData(WinType::sieged_prov_win));
        int id = sprwD.id;
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const Province & prov){
                return prov.GetId() == id;
            });
        if (provIt != provinces.end()) {
            GA_UpdateSiegedProv(gui, provIt);
        }
    }
    catch(std::exception e) {
    }
    */
}
