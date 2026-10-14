# Freelance Project Management (IU2541230786 - CPP73)
Author: Divyansh Jogai | Enrollment No.: IU2541230786

Console-based C++17 OOP project: manage clients and freelancers, create projects, assign a freelancer, record payments, and close or cancel projects. Records persist in text files in `Data/`.

## Build
    g++ -std=c++17 -Wall -Wextra -Wpedantic -o household src/*.cpp      (Linux/Mac/MinGW)
Windows: double-click `BUILD_WINDOWS.bat` (needs MinGW g++). The build has 0 warnings.

## Run
Run `./household` (or `household.exe`) from this folder so it finds `Data/`. Records load at start and are saved on Exit (9) or via menu 8.

## Menu
1 Add, 2 Display (incl. polymorphic "Everyone"), 3 Search, 4 Update, 5 Delete, 6 Transaction (Create project / Assign / Pay / Close / Cancel / Estimate fee), 7 Report, 8 Save/Load, 9 Exit.

## Rules enforced
Project path Open -> Assigned -> Closed (or Cancelled); one active project per freelancer; payments only for Assigned projects and never above the budget; close only when fully paid; cancel only with no payments; linked records cannot be deleted.

## Classes
Person (abstract) -> Client, Freelancer; Project; Payment; FileManager; FreelanceSystem (controller); Utils (validation + dynamic array growth).

## Verification (all real runs)
- `python3 Tests/run_tests.py` : 25 functional tests through a pseudo-terminal; writes `Tests/results.json` and `Tests/Screenshots/*.png`.
- `python3 Tests/make_evidence.py` : -Werror build, 30 OOP checks under AddressSanitizer/UBSan, compile-fail proofs.
- `Tests/oop_checks.cpp`, `Tests/compile_fail_checks.sh` : the OOP harness sources.

## Folders
`src/` code | `Data/` records | `Tests/` tests + screenshots | `UML/` diagrams (+ generator) | `Docs/` report (+ generator)
