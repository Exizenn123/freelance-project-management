#ifndef PAYMENT_H
#define PAYMENT_H
#include <string>
using namespace std;

// One payment made by a client towards a project.
class Payment {
private:
    int paymentId, projectId;
    double amount;
    string date;                               // DD-MM-YYYY
public:
    Payment();
    Payment(int paymentId, int projectId, double amount, const string& date);   // validates via makePayment()
    ~Payment();
    int getPaymentId() const { return paymentId; }
    int getProjectId() const { return projectId; }
    double getAmount() const { return amount; }
    string getDate() const { return date; }
    void makePayment(int paymentId, int projectId, double amount, const string& date);   // validate + store
    void display() const;
    string toRecord() const;
};
#endif
