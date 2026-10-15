Windows build (IU2541230786 - Divyansh Jogai)
No Windows .exe could be produced in the Linux environment used to prepare this project (no MinGW cross-compiler).
Get household.exe in either way:
 1. On Windows with MinGW g++: double-click ..\BUILD_WINDOWS.bat, then copy household.exe to this folder.
 2. After pushing to GitHub: open Actions > "Build Windows exe" > download the artifact "household-windows-exe".
Run household.exe from the project folder (next to Data\) so it finds the records.
