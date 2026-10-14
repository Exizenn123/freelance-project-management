#include "Client.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

Client::Client() : Person(), phone("") {}
Client::Client(int cid, const string& n, const string& ph) : Person(cid, n), phone("") { setPhone(ph); }
Client::~Client() {}
void Client::setPhone(const string& p) {
    if (!Utils::isValidPhone(p)) throw invalid_argument("Phone must be exactly 10 digits");
    phone = p;
}
void Client::display() const {
    cout << left << setw(5) << id << setw(22) << name << phone << "\n";
}
string Client::toRecord() const { return to_string(id) + "|" + name + "|" + phone; }
