#include "Project.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

static void checkTitle(const string& t) {
    if (!Utils::isValidText(t) || t.size() > 50) throw invalid_argument("Title must be 1-50 characters and must not contain '|'");
}
static void checkBudget(double b) {
    if (b < 1 || b > 10000000) throw invalid_argument("Budget must be between 1 and 10000000");
}
Project::Project() : projectId(0), clientId(0), freelancerId(0), title(""), budget(0), status(OPEN) {}
Project::Project(int pid, int cid, const string& t, double b) : projectId(pid), clientId(cid), freelancerId(0), title(t), budget(b), status(OPEN) {
    if (pid < 0 || cid < 0) throw invalid_argument("ID cannot be negative");
    checkTitle(t); checkBudget(b);
}
Project::Project(int pid, int cid, int fid, const string& t, double b, Status st) : projectId(pid), clientId(cid), freelancerId(fid), title(t), budget(b), status(st) {
    if (pid < 0 || cid < 0 || fid < 0) throw invalid_argument("ID cannot be negative");
    checkTitle(t); checkBudget(b);
    if (st < OPEN || st > CANCELLED) throw invalid_argument("Bad status");
    if ((st == ASSIGNED || st == CLOSED) && fid == 0) throw invalid_argument("Assigned/closed project needs a freelancer");
}
Project::~Project() {}
string Project::statusText(Status s) {
    switch (s) { case OPEN: return "Open"; case ASSIGNED: return "Assigned"; case CLOSED: return "Closed"; default: return "Cancelled"; }
}
void Project::setTitle(const string& t) {
    if (status != OPEN) throw logic_error("Only an Open project can be edited");
    checkTitle(t); title = t;
}
void Project::setBudget(double b) {
    if (status != OPEN) throw logic_error("Only an Open project can be edited");
    checkBudget(b); budget = b;
}
void Project::assign(int fid) {
    if (status != OPEN) throw logic_error("Only an 'Open' project can be assigned");
    if (fid <= 0) throw invalid_argument("Invalid freelancer ID");
    freelancerId = fid; status = ASSIGNED;
}
void Project::updateStatus(Status next) {
    bool ok = (status == ASSIGNED && next == CLOSED) || ((status == OPEN || status == ASSIGNED) && next == CANCELLED);
    if (!ok) throw logic_error("Illegal status change: " + statusText(status) + " -> " + statusText(next));
    status = next;
}
void Project::display() const {
    cout << left << setw(5) << projectId << "Client " << setw(4) << clientId << "Freelancer " << setw(4) << freelancerId
         << setw(24) << title << "Rs. " << fixed << setprecision(2) << budget << "  " << statusText() << "\n";
}
string Project::toRecord() const {
    ostringstream o;
    o << projectId << "|" << clientId << "|" << freelancerId << "|" << title << "|" << fixed << setprecision(2) << budget << "|" << static_cast<int>(status);
    return o.str();
}
