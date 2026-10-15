#!/usr/bin/env python3
"""Builds + runs the OOP verification programs for real and renders their output to PNG evidence."""
import os, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from run_tests import render
os.chdir(ROOT)
srcs = [f"src/{f}" for f in sorted(os.listdir("src")) if f.endswith(".cpp") and f != "main.cpp"]
# 1. warnings-as-errors production build
w = subprocess.run(["g++","-std=c++17","-Wall","-Wextra","-Wpedantic","-Werror","-o","household"]+["src/"+f for f in sorted(os.listdir("src")) if f.endswith(".cpp")],capture_output=True,text=True)
build = "$ g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -o household src/*.cpp\n" + (w.stderr or "") + ("BUILD OK - 0 warnings, 0 errors" if w.returncode == 0 else "BUILD FAILED")
render("BUILD","Compilation (warnings treated as errors)","PASS" if w.returncode==0 else "FAIL",build,[],"BUILD OK - 0 warnings, 0 errors",os.path.join(HERE,"Screenshots","BUILD_PASS.png"))
# 2. OOP harness under AddressSanitizer + UBSan
c = subprocess.run(["g++","-std=c++17","-Wall","-Wextra","-g","-fsanitize=address,undefined","-I","src","Tests/oop_checks.cpp"]+srcs+["-o","/tmp/oop_checks"],capture_output=True,text=True)
assert c.returncode == 0, c.stderr
r = subprocess.run(["/tmp/oop_checks"],capture_output=True,text=True)
out = "$ ./oop_checks   (built with -fsanitize=address,undefined)\n" + r.stdout + ("(sanitizers: no errors, no leaks)" if not r.stderr else r.stderr)
ok = r.returncode == 0 and "0 failed" in r.stdout and not r.stderr
open(os.path.join(HERE,"oop_checks_output.txt"),"w").write(out)
render("OOP","OOP verification harness (30 checks)","PASS" if ok else "FAIL",out,[],"Summary: 30 passed, 0 failed",os.path.join(HERE,"Screenshots","OOP_checks_PASS.png"))
# 3. compile-fail proofs
cf = subprocess.run(["bash","Tests/compile_fail_checks.sh"],capture_output=True,text=True).stdout
cf_short = "\n".join(l[:100] for l in cf.splitlines())
open(os.path.join(HERE,"compile_fail_output.txt"),"w").write(cf)
render("OOP","Compiler enforces abstraction and encapsulation","PASS" if cf.count("[PASS]")==3 and "[FAIL]" not in cf else "FAIL","$ bash Tests/compile_fail_checks.sh\n"+cf_short,[],"[PASS] Cannot read private",os.path.join(HERE,"Screenshots","OOP_compile_fail_PASS.png"))
print("evidence ok:", ok, cf.count("[PASS]"))
