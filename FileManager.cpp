#include "FileManager.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

static vector<string> split(const string& line) {
    vector<string> parts; string item; stringstream ss(line);
    while (getline(ss, item, '|')) parts.push_back(item);
    if (!line.empty() && line.back() == '|') parts.push_back("");
    return parts;
}
template <class T>
static void writeAll(const string& path, const T* arr, int n) {
    ofstream out(path);
    if (!out) throw runtime_error("Cannot write file: " + path);
    for (int i = 0; i < n; i++) out << arr[i].toRecord() << "\n";
}
// Reads one file; 'make' builds an object from the split fields (it may throw -> line is skipped).
template <class T, class F>
static void loadFile(const string& path, T*& arr, int& n, int& cap, size_t fields, F make, int& skipped) {
    n = 0;
    ifstream in(path); string line;
    while (getline(in, line)) {
        auto p = split(line);
        try {
            if (p.size() != fields) throw invalid_argument("bad field count");
            T obj = make(p);                       // validate first, store only valid objects
            if (n == cap) Utils::grow(arr, n, cap);
            arr[n++] = obj;
        } catch (const exception&) { skipped++; }
    }
}

FileManager::FileManager(const string& d) : dir(d) {}
FileManager::~FileManager() {}

void FileManager::save(const Client* c, int nc, const Freelancer* f, int nf, const Project* p, int np, const Payment* y, int ny) const {
    filesystem::create_directories(dir);
    writeAll(dir + "/clients.dat", c, nc);
    writeAll(dir + "/freelancers.dat", f, nf);
    writeAll(dir + "/projects.dat", p, np);
    writeAll(dir + "/payments.dat", y, ny);
}
int FileManager::load(Client*& c, int& nc, int& cc, Freelancer*& f, int& nf, int& fc,
                      Project*& p, int& np, int& pc, Payment*& y, int& ny, int& yc) const {
    int skipped = 0;
    loadFile(dir + "/clients.dat", c, nc, cc, 3, [](const vector<string>& v) { return Client(stoi(v[0]), v[1], v[2]); }, skipped);
    loadFile(dir + "/freelancers.dat", f, nf, fc, 4, [](const vector<string>& v) { return Freelancer(stoi(v[0]), v[1], v[2], stod(v[3])); }, skipped);
    loadFile(dir + "/projects.dat", p, np, pc, 6, [](const vector<string>& v) {
        return Project(stoi(v[0]), stoi(v[1]), stoi(v[2]), v[3], stod(v[4]), static_cast<Project::Status>(stoi(v[5]))); }, skipped);
    loadFile(dir + "/payments.dat", y, ny, yc, 4, [](const vector<string>& v) { return Payment(stoi(v[0]), stoi(v[1]), stod(v[2]), v[3]); }, skipped);
    return skipped;
}
