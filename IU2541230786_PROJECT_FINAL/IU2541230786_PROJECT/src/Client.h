#ifndef CLIENT_H
#define CLIENT_H
#include "Person.h"

// A client who posts projects. Inherits id + name from Person.
class Client : public Person {
private:
    string phone;
public:
    Client();
    Client(int id, const string& name, const string& phone);
    ~Client();
    string getPhone() const { return phone; }
    void setPhone(const string& p);        // validates 10 digits
    void display() const override;
    string toRecord() const override;
    string role() const override { return "Client"; }
};
#endif
