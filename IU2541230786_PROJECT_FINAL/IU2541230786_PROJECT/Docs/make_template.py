# Builds the Submission Template from the REAL test results. Run from the project root.
import json
from docx import Document
from docx.shared import Pt

NAME, ENR, CODE = "Divyansh Jogai", "IU2541230786", "CPP73"
res = json.load(open("Tests/results.json"))
d = Document(); d.styles["Normal"].font.name = "Calibri"; d.styles["Normal"].font.size = Pt(11)

def table(head, rows):
    t = d.add_table(rows=1, cols=len(head)); t.style = "Table Grid"
    for i, h in enumerate(head):
        c = t.rows[0].cells[i]; c.text = ""; c.paragraphs[0].add_run(h).bold = True
    for r in rows:
        cells = t.add_row().cells
        for i, v in enumerate(r): cells[i].text = str(v)
    for row in t.rows:
        for c in row.cells:
            for p in c.paragraphs:
                for r in p.runs: r.font.size = Pt(9)
    d.add_paragraph()

d.add_heading(f"{ENR} – {CODE} – SUBMISSION TEMPLATE", 1)
for l in ["Project Name: Freelance Project Management", f"Enrollment No.: {ENR}", f"Student Name: {NAME}", "Date of Submission: 14-10-2026"]: d.add_paragraph(l)

d.add_heading("1. Project Summary", 2)
d.add_paragraph("Problem Statement: Freelance work is often tracked in chats and notes, which leads to double-assigned freelancers, payments above the agreed budget, projects closed while money is still due, and lost records. This project is a console application that manages clients, freelancers, projects and payments with validation, and stores everything in files.")
d.add_paragraph("Objectives: (1) Apply OOP concepts in C++; (2) manage records with add/display/search/update/delete; (3) implement the project transaction (create, assign, pay, close, cancel) with business rules; (4) keep data after the program closes using file handling.")
d.add_paragraph("Key Features:")
for f in ["Add, display, search, update and delete clients, freelancers and projects",
          "Create a project, assign a freelancer, record payments, close or cancel the project",
          "A freelancer can work on only one Assigned project at a time",
          "Payments can never exceed the budget; a project closes only when fully paid",
          "Freelancer fee estimate (rate × hours) and a summary report with earnings per freelancer",
          "Input validation (name, 10-digit phone, positive rate/budget/amount, real calendar date)",
          "Automatic load at start and save at exit using text files"]: d.add_paragraph(f, style="List Bullet")

d.add_heading("2. Class Summary", 2)
table(["Class", "Purpose / Responsibility", "Key Data Members", "Key Member Functions"], [
 ("Person (abstract)", "Base class for people; defines the polymorphic interface.", "id, name", "Person(), Person(id,name), ~Person(), getId(), getName(), setName(), display()*, toRecord()*, role()*"),
 ("Client", "Stores a client who posts projects; validates phone.", "phone (+ inherited id, name)", "Client(), Client(id,name,phone), setPhone(), display(), toRecord()"),
 ("Freelancer", "Stores a freelancer, skill and hourly rate; computes fees.", "skill, rate (+ inherited id, name)", "Freelancer(), Freelancer(id,name,skill,rate), setSkill(), setRate(), calculateFee(), display(), toRecord()"),
 ("Project", "Holds a project and controls the Open/Assigned/Closed/Cancelled life cycle.", "projectId, clientId, freelancerId, title, budget, status", "Project(), Project(...), assign(), updateStatus(), setTitle(), setBudget(), display(), toRecord()"),
 ("Payment", "Validates and stores one payment for a project.", "paymentId, projectId, amount, date", "Payment(), Payment(...), makePayment(), display(), toRecord()"),
 ("FreelanceSystem", "Controller: owns the dynamic arrays, implements every menu option and business rule.", "clients*, freelancers*, projects*, payments* (dynamic arrays), counts, capacities, FileManager fm", "FreelanceSystem(), ~FreelanceSystem(), createProject(), assignFreelancer(), makePayment(), closeProject(), cancelProject(), report(), saveData(), loadData(), run()"),
 ("FileManager", "Reads and writes all records to text files in Data/.", "dir (data folder)", "FileManager(), save(), load()"),
 ("Utils (namespace)", "Input reading, validation rules and the template that doubles a dynamic array.", "none (free functions)", "readLine(), readInt(), readDouble(), isValidName(), isValidPhone(), isValidText(), isValidDate(), toLower(), grow<T>()")])

d.add_heading("3. Project Flow", 2)
d.add_paragraph("USER → MAIN MENU → FreelanceSystem → Entity classes (Client, Freelancer, Project, Payment) → FileManager → FILE (Data/*.dat)")

d.add_heading("4. UML Deliverables", 2)
table(["Diagram", "File Name", "Completed"], [("Use Case Diagram (3 actors, 10 use cases)", "UML/Use_Case_Diagram.png", "☑"), ("Class Diagram", "UML/Class_Diagram.png", "☑"),
 ("Sequence Diagram 1 (Record payment and close project)", "UML/Sequence_Diagram_1.png", "☑"), ("Sequence Diagram 2 (Assign a freelancer)", "UML/Sequence_Diagram_2.png", "☑")])

d.add_heading("5. C++ Concepts Demonstrated", 2)
table(["Concept", "Where Used in My Project", "Explanation"], [
 ("Classes & Objects", "Client, Freelancer, Project, Payment, FreelanceSystem, FileManager", "Each real-world thing is a class; the system creates objects of them."),
 ("Encapsulation", "Private data members with validated setters", "Bad values (phone, rate, budget, date) throw invalid_argument and leave the object unchanged."),
 ("Default + Parameterized Constructors", "All entity classes", "Default builds an empty object (used by arrays); parameterized validates and stores values."),
 ("Destructors", "~Person() (virtual), ~FreelanceSystem()", "The system destructor releases all four dynamic arrays with delete[]."),
 ("Inheritance", "Client and Freelancer derive from Person", "Shared id and name live once in the base class."),
 ("Runtime Polymorphism", "FreelanceSystem::displayEveryone()", "A Person* array holds both kinds; the correct display() and role() run at run time."),
 ("Dynamic Memory", "new Client[4] etc.; Utils::grow()", "Arrays start small and double when full, so any number of records fits; delete[] frees them."),
 ("File Handling", "FileManager::save() / load()", "Records are written as '|' separated lines and reloaded at start-up; corrupt lines are skipped."),
 ("Validation + Exception Handling", "Utils::isValid*; throw/catch in FreelanceSystem::run()", "invalid_argument and logic_error are caught and shown as 'Error: ...' without crashing.")])

d.add_heading("6. Testing", 2)
table(["Test ID", "Operation", "Input", "Expected Output", "Actual Output", "Pass/Fail"], [(r["id"], r["op"], r["input"], r["expected"], r["actual"], r["status"]) for r in res])
d.add_paragraph(f"{sum(r['status']=='PASS' for r in res)} of {len(res)} tests passed. Screenshots of each run are in Tests/Screenshots/.")

d.add_heading("7. Deployment", 2)
for l in ["Compiler / IDE Used: g++ (GCC) with C++17; any editor / Code::Blocks / VS Code",
          "Compilation Command: g++ -std=c++17 -Wall -Wextra -o household src/*.cpp",
          "Executable Name: household (household.exe on Windows; build with BUILD_WINDOWS.bat)",
          "Data File(s): Data/clients.dat, Data/freelancers.dat, Data/projects.dat, Data/payments.dat"]: d.add_paragraph(l)

d.add_heading("8. Student Declaration", 2)
d.add_paragraph("I understand the submitted project and can explain its classes, data members, member functions, file handling and UML diagrams.")
d.add_paragraph("Student Signature: ____________________")
d.save(f"Docs/{ENR}_{CODE}_Submission_Template.docx"); print("template ok")
