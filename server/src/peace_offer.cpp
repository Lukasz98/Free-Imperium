#include "peace_offer.h"

PeaceOffer::PeaceOffer()
{
    peaceId = FreeId;
    FreeId++;
}

int PeaceOffer::FreeId = 0;
