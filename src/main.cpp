#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>

#include "Indexer.h"
#include "File.h"
#include "DVScore.h"
#include "EntropyAnalyser.h"

extern "C" {
    #include "sqlite3.h"
}

using namespace std;
namespace fs = filesystem;

int main(int argc, char *argv[]) {
    fs::path rootPath;
    if (argc > 1) {
        vector<string> args(argv, argv+argc);
        rootPath = args[1];
    } else {
        rootPath = "/";
    }
    
    vector<File> FileList;
    sqlite3 *DB;
    int exit = 0;
    exit = sqlite3_open("fsindex.db", &DB);
    if (exit) {
        cerr << "Error opening the db " << sqlite3_errmsg(DB) << endl;
        return -1;
    }

    Indexer indexer = Indexer(FileList, rootPath, DB);
    cout << "Indexer running!" << endl;
    indexer.indexFiles();
    FileList.shrink_to_fit();

    sqlite3_close(DB);
    return 0;
}
