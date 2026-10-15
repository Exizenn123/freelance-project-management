#!/bin/bash
# Each snippet MUST fail to compile - proves abstraction and encapsulation are enforced by the compiler.
cd "$(dirname "$0")/.."
t() {
  printf '#include "Client.h"\n#include "Freelancer.h"\nint main(){ %s }\n' "$2" > /tmp/cf.cpp
  if g++ -std=c++17 -I src -fsyntax-only /tmp/cf.cpp 2>/tmp/cf.err; then echo "[FAIL] $1 (compiled, should not)"; return 1
  else echo "[PASS] $1 -> compiler error: $(grep -m1 -oE 'error: .*' /tmp/cf.err | cut -c1-90)"; fi
}
t "Cannot instantiate abstract Person" 'Person p;'
t "Cannot read private Client::phone" 'Client c; std::string s = c.phone;'
t "Cannot write private Freelancer::rate" 'Freelancer f; f.rate = -1;'
