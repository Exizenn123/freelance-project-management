#ifndef FREELANCER_H
#define FREELANCER_H
#include "Person.h"

// A freelancer who works on projects. Inherits id + name from Person.
class Freelancer : public Person {
private:
    string skill;
    double rate;                           // charge per hour (Rs.)
public:
    Freelancer();
    Freelancer(int id, const string& name, const string& skill, double rate);
    ~Freelancer();
    string getSkill() const { return skill; }
    double getRate() const { return rate; }
    void setSkill(const string& s);
    void setRate(double r);                // 1 .. 100000
    double calculateFee(double hours) const;   // rate x hours
    void display() const override;
    string toRecord() const override;
    string role() const override { return "Freelancer"; }
};
#endif
