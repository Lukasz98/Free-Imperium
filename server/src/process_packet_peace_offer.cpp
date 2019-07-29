#include "process_packet.h"

void ProcessPacket::OfferPeace(sf::Packet & packet, std::shared_ptr<Client> & client, std::vector<War> & wars, std::vector<Province> & provinces, std::vector<std::shared_ptr<Country>> & countries, std::vector<Packet> & toSend, const Date & date, std::vector<PeaceOffer> & offersForHumans)
{
    int warId, provincesCount;
    std::string recipant;
    std::vector<std::pair<std::string, std::string>> offeredProvinces;
    packet >> warId;
    packet >> recipant;
    packet >> provincesCount;
    for (int i = 0; i < provincesCount; i++) {
        std::pair<std::string, std::string> prov;
        packet >> prov.first;
        packet >> prov.second;
        offeredProvinces.emplace_back(prov);
    }

    bool offeredForDefenders = false;
    std::string offeredBy = client->GetCountry()->GetName();
    auto warIt = std::find_if(wars.begin(), wars.end(), [warId](const War & war) {
            return war.GetId() == warId;
        });
    if (warIt == wars.end())
        return;

    if (!(*warIt).ShouldTheyFight(offeredBy, recipant))
        return;

    offeredForDefenders = (*warIt).IsAttacker(offeredBy);

    bool possible = true;
    for (auto & prov : offeredProvinces) {
        auto provIt = std::find_if(provinces.begin(), provinces.end(), [prov](const Province & province) {
                return (province.GetName() == prov.first);// && (province.GetCountry() == prov.second);
            });
        if (provIt == provinces.end()) {
            possible = false;
            break;
        }
    }

    if (!possible)
        return;

    auto recipantIt = std::find_if(countries.begin(), countries.end(), [recipant](const std::shared_ptr<Country> country) {
            return country->GetName() == recipant;
        });
    if (recipantIt == countries.end())
        return;

    PeaceOffer peaceOffer;
    peaceOffer.warId = warId;
    peaceOffer.warScore = (*warIt).GetAttackerWarScore();
    peaceOffer.recipant = recipant;
    peaceOffer.recipantIsDefender = offeredForDefenders;
    peaceOffer.attackers = (*warIt).GetAttackers();
    peaceOffer.defenders = (*warIt).GetDefenders();
    peaceOffer.offeredBy = offeredBy;

    if (DoTheThing::SendPeace(wars, provinces, countries, peaceOffer, offeredProvinces, toSend, recipantIt, warIt, date, offersForHumans)) {
        Truce truce{date};
        truce.enemy = recipant;
        truce.year++;
        client->GetCountry()->AddTruce(truce);
    }

}
