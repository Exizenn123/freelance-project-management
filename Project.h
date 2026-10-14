#ifndef PROJECT_H
#define PROJECT_H
#include <string>
using namespace std;

// One freelance project and its life cycle: OPEN -> ASSIGNED -> CLOSED (or CANCELLED).
class Project {
public:
    enum Status { OPEN = 0, ASSIGNED = 1, CLOSED = 2, CANCELLED = 3 };
private:
    int projectId, clientId, freelancerId;     // freelancerId 0 = nobody assigned yet
    string title;
    double budget;
    Status status;
public:
    Project();
    Project(int projectId, int clientId, const string& title, double budget);
    Project(int projectId, int clientId, int freelancerId, const string& title, double budget, Status status);  // used when loading files
    ~Project();
    int getProjectId() const { return projectId; }
    int getClientId() const { return clientId; }
    int getFreelancerId() const { return freelancerId; }
    string getTitle() const { return title; }
    double getBudget() const { return budget; }
    Status getStatus() const { return status; }
    static string statusText(Status s);
    string statusText() const { return statusText(status); }
    void setTitle(const string& t);            // only while OPEN
    void setBudget(double b);                  // only while OPEN
    void assign(int freelancerId);             // OPEN -> ASSIGNED
    void updateStatus(Status next);            // legal: ASSIGNED->CLOSED, OPEN/ASSIGNED->CANCELLED
    void display() const;
    string toRecord() const;
};
#endif
