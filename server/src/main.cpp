#include <sys/types.h>
#include <unistd.h>

#include "game.h"

int main()
{
    std::cout << "Server ON\n";

    std::vector<std::shared_ptr<Client>> clients; 

    sf::TcpListener listener;
    listener.setBlocking(false);
    if (listener.listen(55555) != sf::Socket::Done) {
        Log("Listener bind error");
    }

    auto countries = CountryLoader::Load();
    Log("ctrs size="<<countries.size());
    bool room = true, start = false;
    while (room) {
        auto client = std::make_shared<Client>();
        client->setBlocking(false);
        if (listener.accept(*client) == sf::Socket::Done) {
            Log("Client accepted");
            clients.push_back(client);
        }
        
        for (auto & c : clients) {
            sf::Packet packet;
            if (c->receive(packet) == sf::Socket::Done) {
                std::string messg;
                packet >> messg;
                Log(messg);
                if (messg == "Start") {
                    room = false;
                    start = true;
                }
                else if (messg == "name") {
                    packet >> messg;
                    Log(messg);
                    Log((int)clients.size());
                    c->SetName(messg);
                    c->SetCountryName("Atlantyda");
                    
                    Log(messg);

                    {
                        sf::Packet p;
                        p << "Players";
                        p << (int)clients.size();
                        for (auto & cLocal : clients) {
                            p << cLocal->GetName();
                            p << cLocal->GetCountryName();
                            //Log(cLockal->GetName());
                        }
                        for (auto & cLocal : clients)
                            cLocal->send(p);

                    }
                }
                else if (messg == "country") {
                    packet >> messg;
                    c->SetCountryName(messg);

                    {
                        sf::Packet p;
                        p << "Players";
                        p << (int)clients.size();
                        for (auto & cLocal : clients) {
                            p << cLocal->GetName();
                            p << cLocal->GetCountryName();
                            //Log(cLockal->GetName());
                        }
                        for (auto & cLocal : clients)
                            cLocal->send(p);

                    }

                }
            }
        }


        if (start) {
            for (auto & cLocal : clients) {
                sf::Packet packet;
                packet << "Country";
/*
                if (cLocal->GetName() == "boot") {
                    int count = 0;
                    for (auto ccLocal : clients)
                        if (ccLocal->GetCountryName() != "")
                            count++;
                    
                    packet << count;

                    for (auto ccLocal : clients)
                        if (ccLocal->GetName() != "boot")
                            packet << ccLocal->GetCountryName();
                }
                else
                */
                    packet << cLocal->GetCountryName();

                cLocal->send(packet);
            }

            
            sf::Packet packet;
            packet << "Start";
            for (auto & cLocal : clients) {
                cLocal->send(packet);
                Log("Send start to " << cLocal->GetName());
            }
        }
    }
    
    if (start) {
        
        Game game{clients, countries};
        game.Play();

    }

    for (auto & c : clients) {
        c->disconnect();
    }

    listener.close();
}
