#pragma once
#include <memory>
#include <cassert>
#include "log.h"
#include "country.h"
#include "province.h"
#include "unit.h"
#include "war.h"
#include "packet.h"
#include "path_finder.h"

namespace DoTheThing {

    void AddUnit(std::shared_ptr<Unit> & u, std::shared_ptr<Province> & prov, std::shared_ptr<Country> & country);

    void EraseUnit(std::shared_ptr<Unit> & u, std::shared_ptr<Province> & prov, std::shared_ptr<Country> & country, std::vector<Packet> & toSend);

    void NotifyOfDeclinedPeace(int clientCountryId, int recipantId, std::vector<Packet> & toSend);
    
    void MoveUnit(std::shared_ptr<Unit> & unit, const std::vector<std::shared_ptr<Province>> & provinces, 
                    std::shared_ptr<Province> & startProv, std::shared_ptr<Province> & endProv);

    void DeclareWar(std::vector<std::shared_ptr<Country>> & countries, int declaredBy, int declaringTo, 
                    std::vector<War*> & wars, std::vector<Packet> & toSend);
   
    void RealizeAcceptedPeace(std::vector<std::shared_ptr<Country>> & countries, std::vector<std::shared_ptr<Province>> & provinces, PeaceOffer & peaceOffer, std::vector<Packet> & toSend, 
                              std::shared_ptr<Country> & recipant, std::vector<War*>::iterator & warIt, 
                              std::vector<War*> & wars);
   
    bool SendPeace(std::vector<War*> & wars, std::vector<std::shared_ptr<Province>> & provinces, std::vector<std::shared_ptr<Country>> & countries, 
                    PeaceOffer & peaceOffer, std::vector<std::pair<int, int>> & offeredProvinces, 
                    std::vector<Packet> & toSend, std::shared_ptr<Country> & recipant, std::vector<War*>::iterator & warIt, 
                    const Date & date, std::vector<PeaceOffer> & offersForHumans);
    
    void SendNewUnitInfo(const std::shared_ptr<Unit> & unit, std::vector<Packet> & toSend);
    
    void MergeUnits(std::vector<std::shared_ptr<Unit>> & unitsToErase, std::shared_ptr<Unit> & baseUnit, std::shared_ptr<Country> & country, 
                    std::shared_ptr<Province> & prov, std::vector<Packet> & toSend);
    
}
