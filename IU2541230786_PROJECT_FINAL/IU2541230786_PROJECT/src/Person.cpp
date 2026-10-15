#include "Person.h"
#include "Utils.h"

Person::Person() : id(0), name("") {}
Person::Person(int pid, const string& n) : id(pid), name("") {
    if (pid < 0) throw invalid_argument("ID cannot be negative");
    setName(n);
}
Person::~Person() {}
void Person::setName(const string& n) {
    if (!Utils::isValidName(n)) throw invalid_argument("Name must be 2-40 letters (spaces and '.' allowed)");
    name = n;
}
