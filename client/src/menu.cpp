#include "menu.h"

Menu::Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    Gui::Menu::Open(resolution);
    //Gui::Prov::Open(resolution);    
    //Gui::ProvSiege::Open(resolution);    
    //Gui::TopBar::Open(std::vector<std::string>{},resolution);
    //Gui::OpenUnitsList();
}

Menu::~Menu()
{
    
}

void Menu::Play()
{
    loop();
    if (startGame) {
        Gui::Menu::Close();
        Room room(window, settings, countries);
        room.Play();
    }
    else if (quit) {
    }
}

void Menu::loop()
{
    while (!window.ShouldClose()) {
        window.Refresh();


        if (window.mouseLClicked && !window.mouseRClicked) {
            ClickEventType cType = Gui::Base::Click(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                                             (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            if (cType == ClickEventType::QUIT_GAME) {
                quit = true;
                break;
            }
            else if (cType == ClickEventType::START_ROOM) {
                startGame = true;
                break;
            }
            else if (cType == ClickEventType::PROV_SWITCH_TAB) {
                Log("switch tab");
                Gui::Prov::SwitchTab();
                Gui::Base::ResetClick();
            }

            //else if (cdata.evType == Gui::ClickEventType::DEL_FROM_UNITS_LIST) {
            //      Gui::DelFromUnitsList(cdata);
            //}
            //else if (cdata.evType != Gui::ClickEventType::NONE) {
            //    Log("gui click");
            //}
        }

        window.mouseLClicked = false;
        window.mouseRClicked = false;
        if (window.mouseL) {
            Gui::Base::Drag(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                                (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}, dt);
        }
        if (window.mouseR) {
        }
        if (window.scrollOffset) {
            Gui::Base::Scroll(window.scrollOffset, glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                        (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            window.scrollOffset = 0;
        }

        Gui::Base::Hover(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                             (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}); 
        Gui::Base::Draw();
        DrawGui();
    }
}


