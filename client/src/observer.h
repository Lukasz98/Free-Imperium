#pragma once
#include <vector>
#include <any>

class Observer;

class Subject
{
    std::vector<Observer*> observers;
        
protected:
    Subject() {}
    
    void notify(std::any a);

public:
    virtual ~Subject();
    void EraseObserver(const Observer * o);
    void AddObserver(Observer * oo);

};

class Observer
{
    Subject * subject;
protected:
    Observer() { subject = nullptr; }

public:
    virtual ~Observer();

    void SetSubject(Subject * s);
    void DeleteSubject();
    virtual void Notification(std::any n) = 0;
    
};
