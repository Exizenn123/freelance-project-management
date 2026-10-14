#ifndef FREELANCESYSTEM_H
#define FREELANCESYSTEM_H
#include "Client.h"
#include "Freelancer.h"
#include "Project.h"
#include "Payment.h"
#include "FileManager.h"

// Controller class: owns the dynamic arrays and implements every menu operation and business rule.
class FreelanceSystem {
private:
    Client* clients;         int clientCount, clientCap;
    Freelancer* freelancers; int freelancerCount, freelancerCap;
    Project* projects;       int projectCount, projectCap;
    Payment* payments;       int paymentCount, paymentCap;
    FileManager fm;

    int findClient(int id) const;
    int findFreelancer(int id) const;
    int findProject(int id) const;
    double paidFor(int projectId) const;           // sum of payments of one project
    int activeProjectOf(int freelancerId) const;   // id of the Assigned project the freelancer is on, or 0
    int nextClientId() const;  int nextFreelancerId() const;  int nextProjectId() const;  int nextPaymentId() const;
    void printProjectHeader() const;
    void printProjectRow(int index) const;

    // menu actions
    void addClient();  void addFreelancer();
    void displayClients() const;  void displayFreelancers() const;  void displayProjects() const;
    void displayPayments() const; void displayEveryone() const;
    void searchClient() const; void searchFreelancer() const; void searchProject() const; void searchByStatus() const;
    void updateClient(); void updateFreelancer(); void updateProjectBudget(); void updateProjectTitle();
    void deleteClient(); void deleteFreelancer(); void deleteProject();
    void estimateFee() const;
    void addMenu(); void displayMenu(); void searchMenu(); void updateMenu(); void deleteMenu();
    void transactionMenu(); void saveLoadMenu();
public:
    FreelanceSystem();
    ~FreelanceSystem();
    FreelanceSystem(const FreelanceSystem&) = delete;              // owns raw arrays: copying is forbidden
    FreelanceSystem& operator=(const FreelanceSystem&) = delete;

    // main transaction / process
    void createProject();
    void assignFreelancer();
    void makePayment();
    void closeProject();
    void cancelProject();

    void report() const;
    void saveData() const;
    void loadData();
    void run();
};
#endif
