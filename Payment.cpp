#include "Payment.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Payment::Payment() : paymentId(0), projectId(0), amount(0), date("") {}
Payment::Payment(int pay, int proj, double amt, const string& d) : paymentId(0), projectId(0), amount(0), date("") { makePayment(pay, proj, amt, d); }
Payment::~Payment() {}
void Payment::makePayment(int pay, int proj, double amt, const string& d) {
    if (pay < 0 || proj < 0) throw invalid_argument("ID cannot be negative");
    if (amt <= 0 || amt > 10000000) throw invalid_argument("Amount must be between 1 and 10000000");
    if (!Utils::isValidDate(d)) throw invalid_argument("Date must be a valid DD-MM-YYYY");
    paymentId = pay; projectId = proj; amount = amt; date = d;
}
void Payment::display() const {
    cout << left << setw(5) << paymentId << "Project " << setw(5) << projectId << "Rs. " << setw(12) << fixed << setprecision(2) << amount << date << "\n";
}
string Payment::toRecord() const {
    ostringstream o; o << paymentId << "|" << projectId << "|" << fixed << setprecision(2) << amount << "|" << date;
    return o.str();
}
