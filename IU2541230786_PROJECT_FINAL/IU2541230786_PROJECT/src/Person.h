#ifndef PERSON_H
#define PERSON_H
#include <string>
using namespace std;

// Abstract base class for the people in the system (Client, Freelancer).
class Person {
protected:
    int id;
    string name;
public:
    Person();                              // default constructor
    Person(int id, const string& name);    // parameterized constructor (validates)
    virtual ~Person();                     // virtual destructor: safe delete through Person*

    int getId() const { return id; }
    string getName() const { return name; }
    void setName(const string& n);         // validates, throws invalid_argument

    virtual void display() const = 0;      // runtime polymorphism
    virtual string toRecord() const = 0;   // one line for file storage
    virtual string role() const = 0;
};
#endif
