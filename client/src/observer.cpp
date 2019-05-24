#include "observer.h"

Subject::~Subject()
{
    for (auto o : observers) {
        o->DeleteSubject();
    }
}

void Subject::notify(std::any a)
{
    for (auto o : observers)
        o->Notification(a);
}

void Subject::EraseObserver(const Observer * o)
{
    for (auto i = observers.begin(); i < observers.end(); i++) {
        if (*i == o) {
            observers.erase(i);
            break;
        }
    }
}

void Subject::AddObserver(Observer * oo)
{
    for (auto o : observers)
        if (o == oo)
            break;
    observers.push_back(oo);
}

Observer::~Observer()
{
    if (subject != nullptr)
        subject->EraseObserver(this);
}

void Observer::SetSubject(Subject * s)
{
    subject = s;
}

void Observer::DeleteSubject()
{
    subject = nullptr;
}
