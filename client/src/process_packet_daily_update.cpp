#include "process_packet.h"

void ProcessPacket::DailyUpdate(sf::Packet & packet, Gui & gui, std::vector<War> & wars, std::vector<std::unique_ptr<Province>> & provinces)
{
    std::unordered_map<std::string, std::string> values;
    std::string strData;
    float floatData = 0;
    int intData = 0;
    int countryCounter = 0;
    packet >> countryCounter;

    for (int i = 0; i < countryCounter; i++) {
        packet >> strData;
        values["countryName"] = strData;

        packet >> floatData;
        values["countryGold"] = ftos(floatData);

        packet >> floatData;
        values["countryIncome"] = ftos(floatData);
    
        packet >> floatData;
        values["armyMaintenance"] = ftos(floatData);

        packet >> intData;
        values["countryManpower"] = itos(intData);

        packet >> intData;
        values["countryManpowerRecovery"] = itos(intData);

        packet >> strData;
        values["date"] = strData;
    
        packet >> intData;
        values["dateSpeed"] = std::to_string(intData);
    
        gui.Update(values, "topBar");

        int provCount;
        packet >> provCount;
        for (int i = 0; i < provCount; i++) {
            int id, manpower, development;
            int population;
            //float autonomy, unrest, prosperity, administration
            float treasury;
            float monthIncome, totalMonthIncome;
        
            packet >> id;
            //packet >> population;
            packet >> manpower;
            packet >> development;
            //packet >> autonomy;
            //packet >> unrest;
            //packet >> prosperity;
            //packet >> administration;
            packet >> treasury;
            packet >> monthIncome;
            packet >> totalMonthIncome;

            auto provIt = std::find_if(provinces.begin(), provinces.end(), [id](const std::unique_ptr<Province> & prov){
                                                                               return prov->GetId() == id;
                                                                           });
            if (provIt != provinces.end()) {
                //(*provIt).SetPopulation(population);
                (*provIt)->SetManpower(manpower);
                (*provIt)->SetDevelopment(development);
                //(*provIt).SetAutonomy(autonomy);
                //(*provIt).SetUnrest(unrest);
                //(*provIt).SetProsperity(prosperity);
                //(*provIt).SetAdministration(administration);
                (*provIt)->SetTreasury(treasury);
                (*provIt)->SetMonthIncome(monthIncome);
                (*provIt)->SetTotalMonthIncome(totalMonthIncome);

            }

        
        }
    
        int warsCount = 0;
        packet >> warsCount;
        for (int i = 0; i < warsCount; i++) {
            int id;
            packet >> id;
            int score;
            packet >> score;

            int attackersCount, defendersCount;
            std::vector<std::string> attackers, defenders;
            packet >> attackersCount;
            for (int att = 0; att < attackersCount; att++) {
                std::string str;
                packet >> str;
                attackers.push_back(str);
            }
            packet >> defendersCount;
            for (int def = 0; def < defendersCount; def++) {
                std::string str;
                packet >> str;
                defenders.push_back(str);
            }
            
            for (auto & war : wars) {
                if (war.GetId() == id) {
                    war.SetWarScore(score);
                    break;
                }
            }
        }

        int siegedProvincesCount;
        packet >> siegedProvincesCount;
        for (auto & prov : provinces)
            prov->ResetSieging();

        for (int i = 0; i < siegedProvincesCount; i++) {
            std::string provName, siegeCountry;
            int sieged, siegeSoldiers;
            packet >> provName;
            packet >> sieged;
            packet >> siegeCountry;
            packet >> siegeSoldiers;
            auto provIt = std::find_if(provinces.begin(), provinces.end(), [provName](const std::unique_ptr<Province> & prov){
                                                                               return prov->GetName() == provName;
                                                                           });
            if (provIt != provinces.end()) {
                (*provIt)->Sieging(siegeCountry, sieged, siegeSoldiers);
            }
        }
    }
    
}
