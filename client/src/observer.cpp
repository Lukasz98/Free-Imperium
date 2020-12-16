#include "observer.h"

Subject::~Subject()
{
    for (auto o : observers) {
        o->DeleteSubject();
    }
}

void Subject::Notify(const std::vector<GuiStruct> & vals)
{
    for (auto o : observers)
        o->Notification(vals);
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

void Observer::Notification(const std::vector<GuiStruct> & vals)
{
    notified = true;
    this->vals.insert(this->vals.end(), vals.begin(), vals.end());
}


