#include "menu.h"

Menu::Menu(Window & window, Settings & s, std::vector<std::shared_ptr<Country>> & countries)
    : Scene(window, s.resolution), settings(s), countries(countries)
{
    Gui::OpenMenuScreen(resolution);
    //Gui::OpenUnitsList();
}

Menu::~Menu()
{
    
}

void Menu::Play()
{
    loop();
    if (startGame) {
        Gui::CloseMenuScreen();
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
            Gui::ClickData cdata = Gui::Click(
            glm::vec2{window.xMouse * resolution.x / window.GetSize().x, ( window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            if (cdata.evType == Gui::ClickEventType::QUIT_GAME) {
                quit = true;
                break;
            }
            else if (cdata.evType == Gui::ClickEventType::START_ROOM) {
                startGame = true;
                break;
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
            Gui::Drag(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                                (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}, dt);
        }
        if (window.mouseR) {
        }
        if (window.scrollOffset) {
            Gui::Scroll(window.scrollOffset, glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                        (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y});
            window.scrollOffset = 0;
        }

        Gui::Hover(glm::vec2{window.xMouse * resolution.x / window.GetSize().x, 
                             (window.GetSize().y - window.yMouse) * resolution.y / window.GetSize().y}); 
        Gui::Draw();
        DrawGui();
    }
}


