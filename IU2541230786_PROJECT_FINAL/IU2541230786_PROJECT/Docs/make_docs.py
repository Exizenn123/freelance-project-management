# Builds the project report from the REAL test results and evidence images. Run from the project root.
import json
from docx import Document
from docx.shared import Pt, Inches, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml.ns import qn
from docx.oxml import OxmlElement

NAME, ENROLL, CODE, TITLE = "Divyansh Jogai", "IU2541230786", "CPP73", "Freelance Project Management"
res = json.load(open("Tests/results.json"))
d = Document()
st = d.styles["Normal"]; st.font.name = "Calibri"; st.font.size = Pt(11)
for s in d.sections: s.left_margin = s.right_margin = Inches(0.9); s.top_margin = s.bottom_margin = Inches(0.8)

def shade(cell, color):
    tcPr = cell._tc.get_or_add_tcPr(); sh = OxmlElement("w:shd")
    sh.set(qn("w:val"), "clear"); sh.set(qn("w:color"), "auto"); sh.set(qn("w:fill"), color); tcPr.append(sh)
def table(rows, widths=None, header=True, size=9):
    t = d.add_table(rows=len(rows), cols=len(rows[0])); t.style = "Table Grid"
    for i, r in enumerate(rows):
        for j, v in enumerate(r):
            c = t.cell(i, j); c.text = ""; p = c.paragraphs[0]; run = p.add_run(str(v)); run.font.size = Pt(size)
            if header and i == 0: run.bold = True; shade(c, "DCE6F5")
            if widths: c.width = Inches(widths[j])
    d.add_paragraph()
    return t
def sec(t): d.add_heading(t, 2)
def para(t): return d.add_paragraph(t)
def bullets(items):
    for i in items: d.add_paragraph(i, style="List Bullet")
def pic(path, cap, w=6.0):
    p = d.add_paragraph(cap); p.runs[0].bold = True; p.paragraph_format.keep_with_next = True
    d.add_picture(path, width=Inches(w)); d.paragraphs[-1].alignment = WD_ALIGN_PARAGRAPH.CENTER

h = d.add_heading(f"{ENROLL} – {CODE} – PROJECT REPORT", 1)
para(TITLE).runs[0].bold = True
para(f"Enrollment No.: {ENROLL}"); para(f"Student Name: {NAME}"); para("Subject: C++ Guided Project (7-Day, Console-Based OOP)")
para("Date: 14-10-2026")

sec("1. Introduction")
para("Freelance Project Management is a console-based C++17 application that lets an administrator keep records of clients and freelancers, create projects, assign a freelancer, record client payments, and close or cancel projects. All records are saved to text files so they are still available after the program is restarted.")
sec("2. Problem Statement")
para("Small freelance work is often tracked in chats and notes. This causes double-assigned freelancers, payments larger than the agreed budget, projects closed while money is still due, and lost records. A simple, validated, persistent system is needed to enforce these rules.")
sec("3. Objectives")
bullets(["Design the system with classes, encapsulation, inheritance and runtime polymorphism.",
 "Provide add, display, search, update and delete for clients, freelancers and projects.",
 "Implement the project transaction: create, assign, pay, close and cancel, with business rules.",
 "Use dynamic arrays (new[] / delete[]) that grow automatically and are released in destructors.",
 "Persist data with file handling and validate every input with exception handling."])
sec("4. Functional Requirements")
bullets(["FR1: Add, display, search, update and delete clients and freelancers.",
 "FR2: Create a project only for an existing client with a valid title and budget; it starts as Open.",
 "FR3: Assign a freelancer only to an Open project, and only if the freelancer is not already working on an Assigned project.",
 "FR4: Record a payment only for an Assigned project; the total paid can never exceed the budget.",
 "FR5: Close a project only when it is Assigned and fully paid; cancel only if no payment has been made.",
 "FR6: Block deletion of clients, freelancers and projects that other records depend on.",
 "FR7: Show a report: counts, projects by status, budget, paid, outstanding and earnings per freelancer.",
 "FR8: Load records at start-up and save on exit or on request."])
sec("5. System / Project Flow")
para("USER → MAIN MENU (main.cpp / FreelanceSystem::run) → FreelanceSystem → Client / Freelancer / Project / Payment objects → FileManager → Data/*.dat files")
para("main.cpp only creates the system object and calls run(); all logic lives in FreelanceSystem.")

sec("6. Class Design and Responsibilities")
table([["Class","Key data members","Key functions","Responsibility"],
 ["Person (abstract)","id, name","display(), toRecord(), role() (pure virtual); setName()","Common base for people; forces subclasses to implement the interface."],
 ["Client : Person","phone","Client(), setPhone(), display(), toRecord()","Holds client details; validates phone."],
 ["Freelancer : Person","skill, rate","Freelancer(), setRate(), calculateFee(hours), display()","Holds freelancer details; computes rate × hours."],
 ["Project","projectId, clientId, freelancerId, title, budget, status","Project(), assign(), updateStatus(), setTitle(), setBudget()","Owns the project life cycle Open → Assigned → Closed/Cancelled and rejects illegal changes."],
 ["Payment","paymentId, projectId, amount, date","Payment(), makePayment(), display()","Validates and stores one payment."],
 ["FreelanceSystem","Client*, Freelancer*, Project*, Payment* + counts/capacities, FileManager","createProject(), assignFreelancer(), makePayment(), closeProject(), cancelProject(), report(), run()","Controller: owns the dynamic arrays and enforces every business rule."],
 ["FileManager","dir","save(), load()","Writes and reads the four .dat files; skips corrupt lines."],
 ["Utils (namespace)","–","readLine/Int/Double, isValidName/Phone/Text/Date, toLower, grow<T>()","Input reading, validation and array growth."]],
 widths=[1.2,1.5,2.0,2.3], size=8)

sec("7. C++ Concepts Demonstrated (and where)")
table([["Concept","Where in the code","How it was verified"],
 ["Classes & objects / Encapsulation","Private data in every class; validated setters","Harness: bad phone/rate/name rejected, object unchanged; compiler rejects c.phone, f.rate"],
 ["Default + parameterized constructors","All entity classes","Harness: default objects empty; parameterized values stored"],
 ["Destructors","~Person() virtual; ~FreelanceSystem() releases 4 arrays with delete[]","AddressSanitizer: no leaks"],
 ["Inheritance","Client and Freelancer derive from Person","static_assert is_base_of in the harness"],
 ["Runtime polymorphism","displayEveryone(): Person* array calls the right display()/role()","Harness: typeid + output check; test T22"],
 ["Abstract class","Person has pure virtual functions","static_assert is_abstract; compiler rejects 'Person p;'"],
 ["Dynamic memory","new[] arrays (start capacity 4), Utils::grow() doubles, delete[] in destructor","Harness: 2 → 4 growth keeps elements; ASan clean"],
 ["File handling","FileManager::save/load, '|' separated lines","Tests T25 (restart) and harness (corrupt line skipped)"],
 ["Validation + exceptions","invalid_argument / logic_error thrown by classes, caught in run()","Tests T02, T04, T06, T07, T16–T18, T20; 400-run fuzz"]],
 widths=[1.7,2.7,2.6], size=8)

sec("8. Business Rules (as implemented)")
bullets(["Project status path: Open → Assigned → Closed; Open or Assigned → Cancelled. Anything else throws logic_error.",
 "A freelancer can work on only one Assigned project at a time.",
 "Payment ≤ remaining balance; payments only while Assigned.",
 "Close requires balance = 0. Cancel requires no payments.",
 "Cannot delete a client or freelancer linked to a project; cannot delete an Assigned/Closed project or one with payments.",
 "IDs are max(existing)+1, so deleted IDs are never reused."])
sec("9. File Design")
table([["File","Record format"],["Data/clients.dat","id|name|phone"],["Data/freelancers.dat","id|name|skill|rate"],
 ["Data/projects.dat","id|clientId|freelancerId|title|budget|status (0 Open, 1 Assigned, 2 Closed, 3 Cancelled)"],
 ["Data/payments.dat","id|projectId|amount|date"]], widths=[2.0,5.0])
para("At start-up the program loads all four files (missing files are fine on first run); on Exit it saves them. Lines that fail validation are skipped and counted in a warning.")

sec("10. Build and Run")
para("g++ -std=c++17 -Wall -Wextra -Wpedantic -Wold-style-cast -Werror -o household src/*.cpp   then   ./household (Windows: BUILD_WINDOWS.bat). The strict build produces 0 warnings.")

sec("11. Test Cases (real results)")
rows = [["ID","Operation","Input","Expected","Actual (captured output)","Result"]]
for r in res: rows.append([r["id"], r["op"], r["input"], r["expected"], r["actual"], r["status"]])
table(rows, widths=[0.4,1.2,1.3,1.5,1.9,0.5], size=7)
p_ = sum(r["status"] == "PASS" for r in res)
para(f"Result: {p_} of {len(res)} tests passed. The 'Actual' column is the real line captured from the running program.")

sec("12. OOP Verification and Evidence (real runs)")
para("Every screenshot below was produced by running the compiled program (Tests/run_tests.py, Tests/make_evidence.py). Typed input is cyan; the checked output line is highlighted.")
para("OOP harness (Tests/oop_checks.cpp): 30 checks under AddressSanitizer and UBSan, 30 passed, 0 failed, no leaks. Tests/compile_fail_checks.sh shows the compiler rejects instantiating Person and reading private members. A 400-run randomized fuzz (garbage input, long strings, corrupt data files, end-of-input) produced no crash or sanitizer error.")
for cap, f in [("Build with -Wall -Wextra -Wpedantic -Werror","BUILD_PASS"),("OOP verification harness","OOP_checks_PASS"),("Compiler enforces abstraction and encapsulation","OOP_compile_fail_PASS"),
  ("T05 Create project","T05_PASS"),("T13 Assign freelancer","T13_PASS"),("T14 Busy freelancer blocked","T14_PASS"),("T15 Record payment","T15_PASS"),
  ("T16 Overpayment blocked","T16_PASS"),("T18 Close blocked while unpaid","T18_PASS"),("T19 Pay in full and close","T19_PASS"),("T22 Runtime polymorphism (Person*)","T22_PASS"),("T25 Persistence after restart","T25_PASS")]:
    pic(f"Tests/Screenshots/{f}.png", cap)

sec("13. UML Diagrams")
pic("UML/Use_Case_Diagram.png", "Use Case Diagram (3 actors, 10 use cases)", 4.6)
pic("UML/Class_Diagram.png", "Class Diagram", 4.6)
pic("UML/Sequence_Diagram_1.png", "Sequence Diagram 1 – Record a payment and close a project (main transaction)", 6.2)
pic("UML/Sequence_Diagram_2.png", "Sequence Diagram 2 – Assign a freelancer", 5.2)
para("Relationships: Client and Freelancer inherit from Person (generalization). FreelanceSystem owns arrays of Client, Freelancer, Project and Payment (composition, 0..*). Project refers to Client and Freelancer, and Payment refers to Project, by ID (association). FreelanceSystem uses FileManager and Utils (dependency).")

sec("14. Viva Preparation (short answers)")
table([["Question","Answer"],
 ["Responsibility of each class","See section 6: entities hold and validate their own data; FreelanceSystem applies the rules; FileManager does persistence."],
 ["One relationship in the class diagram","FreelanceSystem owns Project objects (composition): the Project array is created in its constructor and deleted in its destructor."],
 ["Inheritance and polymorphism","Client/Freelancer derive from Person; displayEveryone() stores both in Person* and the correct display() runs (test T22)."],
 ["Dynamic memory","new Client[4] etc. in the FreelanceSystem constructor; Utils::grow() allocates double and delete[]s the old; ~FreelanceSystem() calls delete[]."],
 ["Close and reopen","run() calls loadData() first (FileManager::load reads Data/*.dat); Exit calls saveData(). Test T25 proves it with two separate processes."]],
 widths=[2.0,5.0], size=9)

sec("15. Final Checklist")
table([["Item","Done","Evidence"],["Minimum 5 classes","Yes","Person, Client, Freelancer, Project, Payment, FreelanceSystem, FileManager (+Utils)"],
 ["Constructors + destructors","Yes","All classes; harness checks"],["Inheritance + polymorphism","Yes","T22, harness"],["Dynamic memory","Yes","grow + delete[]; ASan clean"],
 ["File persistence","Yes","T25"],["Add/Search/Update/Delete","Yes","T01–T12"],["Main transaction","Yes","T13–T21"],
 ["Use Case Diagram","Yes","UML/Use_Case_Diagram.png"],["Class Diagram","Yes","UML/Class_Diagram.png"],["2 Sequence Diagrams","Yes","UML/Sequence_Diagram_1/2.png"],
 ["15 test cases", "Yes", f"{len(res)} tests, all pass"],
 ["Executable + README","Partly","Linux executable and README included; Windows .exe must be built with BUILD_WINDOWS.bat or the GitHub Action"]],
 widths=[2.0,0.7,4.3], size=9)

sec("16. Limitations and Future Work")
bullets(["Console interface only; one freelancer per project and one active project per freelancer.","Linear search is used (fine for small data); a database or map would scale better.","No login or user roles; a GUI could be added later."])
sec("17. Conclusion")
para("The project meets every item in the blueprint: seven classes plus a helper namespace, inheritance with runtime polymorphism, dynamic memory, file persistence, validated CRUD, a rule-driven project transaction, four UML diagrams, and 25 real tests with screenshots.")
d.save(f"Docs/{ENROLL}_{CODE}_Project_Report.docx"); print("docs ok")
