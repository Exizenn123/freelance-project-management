#include "Freelancer.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Freelancer::Freelancer() : Person(), skill(""), rate(0) {}
Freelancer::Freelancer(int fid, const string& n, const string& s, double r) : Person(fid, n), skill(""), rate(0) {
    setSkill(s);
    setRate(r);
}
Freelancer::~Freelancer() {}
void Freelancer::setSkill(const string& s) {
    if (!Utils::isValidText(s) || s.size() > 30) throw invalid_argument("Skill must be 1-30 characters and must not contain '|'");
    skill = s;
}
void Freelancer::setRate(double r) {
    if (r < 1 || r > 100000) throw invalid_argument("Rate must be between 1 and 100000");
    rate = r;
}
double Freelancer::calculateFee(double hours) const {
    if (hours <= 0 || hours > 10000) throw invalid_argument("Hours must be between 0 and 10000");
    return rate * hours;
}
void Freelancer::display() const {
    cout << left << setw(5) << id << setw(22) << name << setw(18) << skill << "Rs. " << fixed << setprecision(2) << rate << "/hr\n";
}
string Freelancer::toRecord() const {
    ostringstream o; o << id << "|" << name << "|" << skill << "|" << fixed << setprecision(2) << rate;
    return o.str();
}
