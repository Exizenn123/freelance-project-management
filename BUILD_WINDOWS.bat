@echo off
g++ -std=c++17 -Wall -Wextra -Wpedantic -o household.exe src/*.cpp
if %errorlevel%==0 (echo Build OK. Run household.exe) else (echo Build failed)
pause
