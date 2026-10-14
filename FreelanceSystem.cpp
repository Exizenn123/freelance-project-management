#include "FreelanceSystem.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
using namespace std;

static const int START_CAP = 4;   // small on purpose so that array growth is exercised

FreelanceSystem::FreelanceSystem()
    : clients(new Client[START_CAP]), clientCount(0), clientCap(START_CAP),
      freelancers(new Freelancer[START_CAP]), freelancerCount(0), freelancerCap(START_CAP),
      projects(new Project[START_CAP]), projectCount(0), projectCap(START_CAP),
      payments(new Payment[START_CAP]), paymentCount(0), paymentCap(START_CAP), fm("Data") {}

FreelanceSystem::~FreelanceSystem() {
    delete[] clients; delete[] freelancers; delete[] projects; delete[] payments;
}

// ---------- lookups ----------
int FreelanceSystem::findClient(int id) const { for (int i = 0; i < clientCount; i++) if (clients[i].getId() == id) return i; return -1; }
int FreelanceSystem::findFreelancer(int id) const { for (int i = 0; i < freelancerCount; i++) if (freelancers[i].getId() == id) return i; return -1; }
int FreelanceSystem::findProject(int id) const { for (int i = 0; i < projectCount; i++) if (projects[i].getProjectId() == id) return i; return -1; }
double FreelanceSystem::paidFor(int projectId) const {
    double sum = 0;
    for (int i = 0; i < paymentCount; i++) if (payments[i].getProjectId() == projectId) sum += payments[i].getAmount();
    return sum;
}
int FreelanceSystem::activeProjectOf(int fid) const {
    for (int i = 0; i < projectCount; i++)
        if (projects[i].getFreelancerId() == fid && projects[i].getStatus() == Project::ASSIGNED) return projects[i].getProjectId();
    return 0;
}
int FreelanceSystem::nextClientId() const { int m = 0; for (int i = 0; i < clientCount; i++) m = max(m, clients[i].getId()); return m + 1; }
int FreelanceSystem::nextFreelancerId() const { int m = 0; for (int i = 0; i < freelancerCount; i++) m = max(m, freelancers[i].getId()); return m + 1; }
int FreelanceSystem::nextProjectId() const { int m = 0; for (int i = 0; i < projectCount; i++) m = max(m, projects[i].getProjectId()); return m + 1; }
int FreelanceSystem::nextPaymentId() const { int m = 0; for (int i = 0; i < paymentCount; i++) m = max(m, payments[i].getPaymentId()); return m + 1; }

// ---------- add ----------
void FreelanceSystem::addClient() {
    string name = Utils::readLine("Name: "), phone = Utils::readLine("Phone (10 digits): ");
    Client c(nextClientId(), name, phone);                  // constructor validates
    if (clientCount == clientCap) Utils::grow(clients, clientCount, clientCap);
    clients[clientCount++] = c;
    cout << "Client added with ID " << c.getId() << "\n";
}
void FreelanceSystem::addFreelancer() {
    string name = Utils::readLine("Name: "), skill = Utils::readLine("Skill: ");
    double rate = Utils::readDouble("Hourly rate (Rs.): ");
    Freelancer f(nextFreelancerId(), name, skill, rate);
    if (freelancerCount == freelancerCap) Utils::grow(freelancers, freelancerCount, freelancerCap);
    freelancers[freelancerCount++] = f;
    cout << "Freelancer added with ID " << f.getId() << "\n";
}

// ---------- display ----------
void FreelanceSystem::displayClients() const {
    cout << "\nID   Name                  Phone\n";
    for (int i = 0; i < clientCount; i++) clients[i].display();
    if (!clientCount) cout << "(no clients)\n";
}
void FreelanceSystem::displayFreelancers() const {
    cout << "\nID   Name                  Skill             Rate\n";
    for (int i = 0; i < freelancerCount; i++) freelancers[i].display();
    if (!freelancerCount) cout << "(no freelancers)\n";
}
void FreelanceSystem::printProjectHeader() const {
    cout << "\nID   Client            Freelancer        Title                     Budget       Paid         Status\n";
}
void FreelanceSystem::printProjectRow(int i) const {
    const Project& p = projects[i];
    int c = findClient(p.getClientId()), f = findFreelancer(p.getFreelancerId());
    cout << left << setw(5) << p.getProjectId() << setw(18) << (c >= 0 ? clients[c].getName() : "?")
         << setw(18) << (f >= 0 ? freelancers[f].getName() : "-") << setw(26) << p.getTitle()
         << setw(13) << fixed << setprecision(2) << p.getBudget() << setw(13) << paidFor(p.getProjectId()) << p.statusText() << "\n";
}
void FreelanceSystem::displayProjects() const {
    printProjectHeader();
    for (int i = 0; i < projectCount; i++) printProjectRow(i);
    if (!projectCount) cout << "(no projects)\n";
}
void FreelanceSystem::displayPayments() const {
    cout << "\nID   Project       Amount           Date\n";
    for (int i = 0; i < paymentCount; i++) payments[i].display();
    if (!paymentCount) cout << "(no payments)\n";
}
// Runtime polymorphism: one array of Person* holds Clients and Freelancers; the right display() runs.
void FreelanceSystem::displayEveryone() const {
    int n = clientCount + freelancerCount;
    Person** all = new Person*[n > 0 ? n : 1];
    int k = 0;
    for (int i = 0; i < clientCount; i++) all[k++] = &clients[i];
    for (int i = 0; i < freelancerCount; i++) all[k++] = &freelancers[i];
    cout << "\nAll people (via Person*):\n";
    for (int i = 0; i < n; i++) { cout << all[i]->role() << ": "; all[i]->display(); }
    if (!n) cout << "(nobody yet)\n";
    delete[] all;                                  // only the pointer array: the objects are owned elsewhere
}

// ---------- search ----------
void FreelanceSystem::searchClient() const {
    string key = Utils::toLower(Utils::readLine("Name contains: ")); bool any = false;
    for (int i = 0; i < clientCount; i++)
        if (Utils::toLower(clients[i].getName()).find(key) != string::npos) { if (!any) cout << "\nID   Name                  Phone\n"; clients[i].display(); any = true; }
    if (!any) cout << "No matching client.\n";
}
void FreelanceSystem::searchFreelancer() const {
    string key = Utils::toLower(Utils::readLine("Skill contains: ")); bool any = false;
    for (int i = 0; i < freelancerCount; i++)
        if (Utils::toLower(freelancers[i].getSkill()).find(key) != string::npos) { if (!any) cout << "\nID   Name                  Skill             Rate\n"; freelancers[i].display(); any = true; }
    if (!any) cout << "No matching freelancer.\n";
}
void FreelanceSystem::searchProject() const {
    string key = Utils::toLower(Utils::readLine("Title contains: ")); bool any = false;
    for (int i = 0; i < projectCount; i++)
        if (Utils::toLower(projects[i].getTitle()).find(key) != string::npos) { if (!any) printProjectHeader(); printProjectRow(i); any = true; }
    if (!any) cout << "No matching project.\n";
}
void FreelanceSystem::searchByStatus() const {
    int s = Utils::readInt("Status (0 Open, 1 Assigned, 2 Closed, 3 Cancelled): ");
    if (s < 0 || s > 3) throw invalid_argument("Status must be 0-3");
    bool any = false;
    for (int i = 0; i < projectCount; i++)
        if (static_cast<int>(projects[i].getStatus()) == s) { if (!any) printProjectHeader(); printProjectRow(i); any = true; }
    if (!any) cout << "No project with that status.\n";
}

// ---------- update ----------
void FreelanceSystem::updateClient() {
    int i = findClient(Utils::readInt("Client ID: ")); if (i < 0) throw logic_error("Client not found");
    clients[i].setPhone(Utils::readLine("New phone: ")); cout << "Client updated.\n";
}
void FreelanceSystem::updateFreelancer() {
    int i = findFreelancer(Utils::readInt("Freelancer ID: ")); if (i < 0) throw logic_error("Freelancer not found");
    freelancers[i].setRate(Utils::readDouble("New hourly rate: ")); cout << "Freelancer updated.\n";
}
void FreelanceSystem::updateProjectBudget() {
    int i = findProject(Utils::readInt("Project ID: ")); if (i < 0) throw logic_error("Project not found");
    projects[i].setBudget(Utils::readDouble("New budget: ")); cout << "Project updated.\n";
}
void FreelanceSystem::updateProjectTitle() {
    int i = findProject(Utils::readInt("Project ID: ")); if (i < 0) throw logic_error("Project not found");
    projects[i].setTitle(Utils::readLine("New title: ")); cout << "Project updated.\n";
}

// ---------- delete (shifts the array left; refuses when other records depend on the item) ----------
void FreelanceSystem::deleteClient() {
    int id = Utils::readInt("Client ID: "), i = findClient(id); if (i < 0) throw logic_error("Client not found");
    for (int k = 0; k < projectCount; k++) if (projects[k].getClientId() == id) throw logic_error("Cannot delete: client has a project.");
    for (int k = i; k < clientCount - 1; k++) clients[k] = clients[k + 1];
    clientCount--; cout << "Client deleted.\n";
}
void FreelanceSystem::deleteFreelancer() {
    int id = Utils::readInt("Freelancer ID: "), i = findFreelancer(id); if (i < 0) throw logic_error("Freelancer not found");
    for (int k = 0; k < projectCount; k++) if (projects[k].getFreelancerId() == id) throw logic_error("Cannot delete: freelancer is linked to a project.");
    for (int k = i; k < freelancerCount - 1; k++) freelancers[k] = freelancers[k + 1];
    freelancerCount--; cout << "Freelancer deleted.\n";
}
void FreelanceSystem::deleteProject() {
    int id = Utils::readInt("Project ID: "), i = findProject(id); if (i < 0) throw logic_error("Project not found");
    Project::Status st = projects[i].getStatus();
    if (st == Project::ASSIGNED || st == Project::CLOSED) throw logic_error("Cannot delete: project is Assigned or Closed.");
    if (paidFor(id) > 0) throw logic_error("Cannot delete: project has payments.");
    for (int k = i; k < projectCount - 1; k++) projects[k] = projects[k + 1];
    projectCount--; cout << "Project deleted.\n";
}

// ---------- main transaction / process ----------
void FreelanceSystem::createProject() {
    int cid = Utils::readInt("Client ID: ");
    string title = Utils::readLine("Project title: "); double budget = Utils::readDouble("Budget (Rs.): ");
    if (findClient(cid) < 0) throw logic_error("Client not found");
    Project p(nextProjectId(), cid, title, budget);          // validates title and budget
    if (projectCount == projectCap) Utils::grow(projects, projectCount, projectCap);
    projects[projectCount++] = p;
    cout << "Project created with ID " << p.getProjectId() << " (status Open)\n";
}
void FreelanceSystem::assignFreelancer() {
    int pid = Utils::readInt("Project ID: "), fid = Utils::readInt("Freelancer ID: ");
    int pi = findProject(pid), fi = findFreelancer(fid);
    if (pi < 0) throw logic_error("Project not found");
    if (fi < 0) throw logic_error("Freelancer not found");
    if (projects[pi].getStatus() != Project::OPEN) throw logic_error("Only an 'Open' project can be assigned");
    int busy = activeProjectOf(fid);
    if (busy) throw logic_error("Freelancer is already working on project " + to_string(busy));
    projects[pi].assign(fid);
    cout << "Freelancer assigned. Project " << pid << " is now Assigned.\n";
}
void FreelanceSystem::makePayment() {
    int pid = Utils::readInt("Project ID: ");
    double amount = Utils::readDouble("Amount (Rs.): "); string date = Utils::readLine("Date (DD-MM-YYYY): ");
    int pi = findProject(pid); if (pi < 0) throw logic_error("Project not found");
    if (projects[pi].getStatus() != Project::ASSIGNED) throw logic_error("Payments are accepted only for an Assigned project");
    Payment pay(nextPaymentId(), pid, amount, date);         // validates amount and date
    double balance = projects[pi].getBudget() - paidFor(pid);
    if (pay.getAmount() > balance + 0.0001) { ostringstream o; o << fixed << setprecision(2) << balance; throw logic_error("Payment exceeds the budget (balance Rs. " + o.str() + ")"); }
    if (paymentCount == paymentCap) Utils::grow(payments, paymentCount, paymentCap);
    payments[paymentCount++] = pay;
    cout << "Payment recorded. Payment ID " << pay.getPaymentId() << ". Paid Rs. " << fixed << setprecision(2) << paidFor(pid)
         << " of Rs. " << projects[pi].getBudget() << "\n";
}
void FreelanceSystem::closeProject() {
    int pid = Utils::readInt("Project ID: "), pi = findProject(pid); if (pi < 0) throw logic_error("Project not found");
    if (projects[pi].getStatus() != Project::ASSIGNED) throw logic_error("Only an 'Assigned' project can be closed");
    double due = projects[pi].getBudget() - paidFor(pid);
    if (due > 0.0001) { ostringstream o; o << fixed << setprecision(2) << due; throw logic_error("Cannot close: Rs. " + o.str() + " still unpaid"); }
    projects[pi].updateStatus(Project::CLOSED);
    cout << "Project closed. Total paid Rs. " << fixed << setprecision(2) << paidFor(pid) << "\n";
}
void FreelanceSystem::cancelProject() {
    int pid = Utils::readInt("Project ID: "), pi = findProject(pid); if (pi < 0) throw logic_error("Project not found");
    Project::Status st = projects[pi].getStatus();
    if (st != Project::OPEN && st != Project::ASSIGNED) throw logic_error("Only an 'Open' or 'Assigned' project can be cancelled");
    if (paidFor(pid) > 0) throw logic_error("Cannot cancel: project already has payments");
    projects[pi].updateStatus(Project::CANCELLED);
    cout << "Project cancelled.\n";
}
void FreelanceSystem::estimateFee() const {
    int fi = findFreelancer(Utils::readInt("Freelancer ID: ")); if (fi < 0) throw logic_error("Freelancer not found");
    double h = Utils::readDouble("Estimated hours: ");
    cout << "Estimated fee for " << freelancers[fi].getName() << ": Rs. " << fixed << setprecision(2) << freelancers[fi].calculateFee(h) << "\n";
}

// ---------- report ----------
void FreelanceSystem::report() const {
    int cnt[4] = {0, 0, 0, 0}; double budget = 0, paid = 0; map<string, double> earned;
    for (int i = 0; i < projectCount; i++) {
        cnt[static_cast<int>(projects[i].getStatus())]++;
        if (projects[i].getStatus() == Project::CANCELLED) continue;
        budget += projects[i].getBudget();
        double p = paidFor(projects[i].getProjectId()); paid += p;
        int f = findFreelancer(projects[i].getFreelancerId());
        if (f >= 0 && p > 0) earned[freelancers[f].getName()] += p;
    }
    cout << "\n========== SUMMARY REPORT ==========\n"
         << "Clients: " << clientCount << "   Freelancers: " << freelancerCount << "   Projects: " << projectCount << "   Payments: " << paymentCount << "\n"
         << "Projects : " << projectCount << " (Open " << cnt[0] << ", Assigned " << cnt[1] << ", Closed " << cnt[2] << ", Cancelled " << cnt[3] << ")\n"
         << fixed << setprecision(2)
         << "Total budget (not cancelled): Rs. " << budget << "\n"
         << "Total paid: Rs. " << paid << "\n"
         << "Outstanding balance: Rs. " << budget - paid << "\n";
    if (!earned.empty()) {
        cout << "Earnings per freelancer:\n";
        for (const auto& kv : earned) cout << "  " << left << setw(22) << kv.first << "Rs. " << kv.second << "\n";
    }
    cout << "====================================\n";
}

// ---------- file handling ----------
void FreelanceSystem::saveData() const {
    fm.save(clients, clientCount, freelancers, freelancerCount, projects, projectCount, payments, paymentCount);
    cout << "Data saved.\n";
}
void FreelanceSystem::loadData() {
    int skipped = fm.load(clients, clientCount, clientCap, freelancers, freelancerCount, freelancerCap,
                          projects, projectCount, projectCap, payments, paymentCount, paymentCap);
    cout << "Data loaded: " << clientCount << " clients, " << freelancerCount << " freelancers, "
         << projectCount << " projects, " << paymentCount << " payments.\n";
    if (skipped) cout << "Warning: " << skipped << " corrupt line(s) skipped.\n";
}

// ---------- menus ----------
void FreelanceSystem::addMenu() {
    cout << "\n-- Add --\n1. Client\n2. Freelancer\n3. Project\n";
    switch (Utils::readInt("Choice: ")) { case 1: addClient(); break; case 2: addFreelancer(); break; case 3: createProject(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::displayMenu() {
    cout << "\n-- Display --\n1. Clients\n2. Freelancers\n3. Projects\n4. Payments\n5. Everyone (people)\n";
    switch (Utils::readInt("Choice: ")) {
        case 1: displayClients(); break; case 2: displayFreelancers(); break; case 3: displayProjects(); break;
        case 4: displayPayments(); break; case 5: displayEveryone(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::searchMenu() {
    cout << "\n-- Search --\n1. Client by name\n2. Freelancer by skill\n3. Project by title\n4. Projects by status\n";
    switch (Utils::readInt("Choice: ")) {
        case 1: searchClient(); break; case 2: searchFreelancer(); break; case 3: searchProject(); break;
        case 4: searchByStatus(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::updateMenu() {
    cout << "\n-- Update --\n1. Client phone\n2. Freelancer rate\n3. Project budget\n4. Project title\n";
    switch (Utils::readInt("Choice: ")) {
        case 1: updateClient(); break; case 2: updateFreelancer(); break; case 3: updateProjectBudget(); break;
        case 4: updateProjectTitle(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::deleteMenu() {
    cout << "\n-- Delete --\n1. Client\n2. Freelancer\n3. Project\n";
    switch (Utils::readInt("Choice: ")) { case 1: deleteClient(); break; case 2: deleteFreelancer(); break; case 3: deleteProject(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::transactionMenu() {
    cout << "\n-- Transaction --\n1. Create project\n2. Assign freelancer\n3. Record payment\n4. Close project\n5. Cancel project\n6. Estimate freelancer fee\n";
    switch (Utils::readInt("Choice: ")) {
        case 1: createProject(); break; case 2: assignFreelancer(); break; case 3: makePayment(); break;
        case 4: closeProject(); break; case 5: cancelProject(); break; case 6: estimateFee(); break; default: cout << "Invalid choice.\n"; }
}
void FreelanceSystem::saveLoadMenu() {
    cout << "\n-- Save / Load --\n1. Save now\n2. Load from files (replaces current data)\n";
    int c = Utils::readInt("Choice: ");
    if (c == 1) saveData(); else if (c == 2) loadData(); else cout << "Invalid choice.\n";
}

void FreelanceSystem::run() {
    try { loadData(); } catch (const exception& e) { cout << "Load problem: " << e.what() << "\n"; }
    bool running = true;
    while (running) {
        cout << "\n===== FREELANCE PROJECT MANAGEMENT =====\n1. Add Record\n2. Display Records\n3. Search Record\n4. Update Record\n5. Delete Record\n"
                "6. Main Transaction (Project / Assign / Pay / Close)\n7. Report\n8. Save / Load\n9. Exit\n";
        try {
            switch (Utils::readInt("Enter choice: ")) {
                case 1: addMenu(); break;  case 2: displayMenu(); break; case 3: searchMenu(); break;
                case 4: updateMenu(); break; case 5: deleteMenu(); break; case 6: transactionMenu(); break;
                case 7: report(); break;   case 8: saveLoadMenu(); break;
                case 9: running = false; break;
                default: cout << "Invalid choice. Enter 1-9.\n";
            }
        } catch (const invalid_argument& e) { cout << "Error: " << e.what() << "\n"; }
          catch (const logic_error& e) { cout << "Error: " << e.what() << "\n"; }
          catch (const runtime_error& e) {
              if (string(e.what()) == "Input ended") running = false; else cout << "Error: " << e.what() << "\n"; }
    }
    try { saveData(); } catch (const exception& e) { cout << "Could not save: " << e.what() << "\n"; }
    cout << "Goodbye!\n";
}
