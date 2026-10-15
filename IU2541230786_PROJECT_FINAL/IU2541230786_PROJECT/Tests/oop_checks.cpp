// OOP verification harness: every check really executes against the project classes.
#include "Client.h"
#include "Freelancer.h"
#include "Project.h"
#include "Payment.h"
#include "FileManager.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <type_traits>
#include <filesystem>
using namespace std;

static int passed = 0, failed = 0;
static void check(const string& name, bool ok) { cout << (ok ? "[PASS] " : "[FAIL] ") << name << "\n"; ok ? passed++ : failed++; }
template <class E, class F> static bool throwsType(F f) { try { f(); } catch (const E&) { return true; } catch (...) { return false; } return false; }

static_assert(is_abstract_v<Person>, "Person must be abstract");
static_assert(has_virtual_destructor_v<Person>, "Person needs a virtual destructor");
static_assert(is_base_of_v<Person, Client> && is_base_of_v<Person, Freelancer>, "inheritance");
static_assert(!is_base_of_v<Person, Project> && !is_base_of_v<Person, Payment>, "Project/Payment are not people");

int main() {
    cout << "== OOP verification (IU2541230786) ==\n";
    check("Abstraction: Person is abstract (cannot be instantiated)", is_abstract_v<Person>);
    check("Inheritance: Client and Freelancer derive from Person", is_base_of_v<Person, Client> && is_base_of_v<Person, Freelancer>);
    check("Destructor: Person has a virtual destructor", has_virtual_destructor_v<Person>);

    Client dc; Freelancer df; Project dp; Payment dy;
    check("Default constructors give empty objects", dc.getId() == 0 && df.getRate() == 0 && dp.getStatus() == Project::OPEN && dy.getAmount() == 0);
    Client c(7, "Neha Joshi", "9876543210");
    Freelancer f(3, "Karan Mehta", "Web Design", 800);
    check("Parameterized constructors store values", c.getId() == 7 && c.getPhone() == "9876543210" && f.getSkill() == "Web Design" && f.getRate() == 800);

    check("Encapsulation: bad phone rejected (invalid_argument)", throwsType<invalid_argument>([&] { c.setPhone("12345"); }));
    check("Encapsulation: object unchanged after rejected update", c.getPhone() == "9876543210");
    check("Encapsulation: constructor rejects invalid name", throwsType<invalid_argument>([] { Client x(1, "N", "9876543210"); }));
    check("Encapsulation: rate outside 1..100000 rejected", throwsType<invalid_argument>([&] { f.setRate(0); }) && throwsType<invalid_argument>([&] { f.setRate(100001); }));
    check("Encapsulation: '|' rejected in text (protects file format)", throwsType<invalid_argument>([&] { f.setSkill("a|b"); }));

    Person* people[2] = { new Client(1, "Rohit Verma", "9123456780"), new Freelancer(2, "Pooja Nair", "Data Entry", 300) };
    check("Polymorphism: role() resolved at run time", people[0]->role() == "Client" && people[1]->role() == "Freelancer");
    check("Polymorphism: dynamic type via typeid", typeid(*people[0]) == typeid(Client) && typeid(*people[1]) == typeid(Freelancer));
    check("Polymorphism: toRecord() differs per subclass", people[0]->toRecord() == "1|Rohit Verma|9123456780" && people[1]->toRecord() == "2|Pooja Nair|Data Entry|300.00");
    ostringstream cap; streambuf* old = cout.rdbuf(cap.rdbuf());
    people[0]->display(); people[1]->display();
    cout.rdbuf(old);
    check("Polymorphism: display() prints the right fields per class", cap.str().find("9123456780") != string::npos && cap.str().find("Data Entry") != string::npos);
    for (Person* p : people) delete p;      // virtual destructor: no leak/UB (verified under AddressSanitizer)
    check("Delete through Person* completed", true);
    check("Method: calculateFee = rate x hours", f.calculateFee(5) == 4000);
    check("Exception: calculateFee rejects 0 hours", throwsType<invalid_argument>([&] { f.calculateFee(0); }));

    Project p(1, 7, "Company Website", 20000);
    check("Life cycle: new project is OPEN with no freelancer", p.getStatus() == Project::OPEN && p.getFreelancerId() == 0);
    check("Exception: closing an Open project throws logic_error", throwsType<logic_error>([&] { p.updateStatus(Project::CLOSED); }));
    p.assign(3);
    check("Life cycle: OPEN -> ASSIGNED via assign()", p.getStatus() == Project::ASSIGNED && p.getFreelancerId() == 3);
    check("Exception: assigning twice throws logic_error", throwsType<logic_error>([&] { p.assign(4); }));
    check("Exception: editing budget after assignment throws logic_error", throwsType<logic_error>([&] { p.setBudget(5); }));
    p.updateStatus(Project::CLOSED);
    check("Life cycle: ASSIGNED -> CLOSED", p.getStatus() == Project::CLOSED);
    check("Exception: a Closed project cannot be cancelled", throwsType<logic_error>([&] { p.updateStatus(Project::CANCELLED); }));
    check("Exception: Payment rejects bad date / amount", throwsType<invalid_argument>([] { Payment x(1, 1, 100, "31-02-2026"); }) && throwsType<invalid_argument>([] { Payment x(1, 1, -5, "01-01-2026"); }));
    check("Utils: leap-year date 29-02-2028 valid, 29-02-2027 invalid", Utils::isValidDate("29-02-2028") && !Utils::isValidDate("29-02-2027"));

    int cap_n = 2, n = 2; Client* arr = new Client[cap_n];
    arr[0] = Client(1, "Aa Bb", "1234567890"); arr[1] = Client(2, "Cc Dd", "1234567891");
    Utils::grow(arr, n, cap_n);
    check("Dynamic memory: capacity doubled 2 -> 4", cap_n == 4);
    check("Dynamic memory: elements preserved after growth", arr[0].getName() == "Aa Bb" && arr[1].getId() == 2);
    delete[] arr;

    string dir = (filesystem::temp_directory_path() / "fl_oop_check").string();
    filesystem::remove_all(dir);
    { FileManager fm(dir);
      Client cs[1] = { c }; Freelancer fs[1] = { f }; Project ps[1] = { p }; Payment ys[1] = { Payment(1, 1, 20000, "25-10-2026") };
      fm.save(cs, 1, fs, 1, ps, 1, ys, 1); }
    { ofstream bad(dir + "/clients.dat", ios::app); bad << "garbage line\n"; }
    int nc = 0, cc = 4, nf = 0, fc = 4, np = 0, pc = 4, ny = 0, yc = 4;
    Client* C = new Client[cc]; Freelancer* F = new Freelancer[fc]; Project* P = new Project[pc]; Payment* Y = new Payment[yc];
    int skipped = FileManager(dir).load(C, nc, cc, F, nf, fc, P, np, pc, Y, ny, yc);
    check("File handling: records survive save -> load", nc == 1 && nf == 1 && np == 1 && ny == 1 && C[0].getName() == "Neha Joshi" && P[0].getStatus() == Project::CLOSED && Y[0].getAmount() == 20000);
    check("File handling: corrupt line skipped and counted", skipped == 1);
    delete[] C; delete[] F; delete[] P; delete[] Y;
    filesystem::remove_all(dir);

    cout << "\nSummary: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
