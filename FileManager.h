#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <string>
#include "Client.h"
#include "Freelancer.h"
#include "Project.h"
#include "Payment.h"
using namespace std;

// Saves/loads each record type to its own text file (one record per line, fields separated by '|').
class FileManager {
private:
    string dir;
public:
    FileManager(const string& dir = "Data");
    ~FileManager();
    void save(const Client* c, int nc, const Freelancer* f, int nf, const Project* p, int np, const Payment* y, int ny) const;
    // load: fills the arrays (growing them when full). Missing files are fine (first run).
    // Returns how many corrupt lines were skipped.
    int load(Client*& c, int& nc, int& cc, Freelancer*& f, int& nf, int& fc,
             Project*& p, int& np, int& pc, Payment*& y, int& ny, int& yc) const;
};
#endif
