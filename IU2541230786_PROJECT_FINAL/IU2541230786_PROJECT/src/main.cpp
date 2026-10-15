#include "FreelanceSystem.h"

int main() {
    FreelanceSystem system;   // creates the dynamic arrays
    system.run();             // menu loop; all logic lives in the system class
    return 0;                 // destructor releases the memory
}
