#pragma once

#include <vector>
#include <filesystem>
#include <sqlite3.h>

namespace fs = std::filesystem;

class File;

class Indexer {
private:
    static constexpr size_t BATCH_SIZE = 10000;

    std::vector<File>& FileList;
    fs::path rootPath;
    sqlite3* DB;
    size_t filecount = 0;

    int getDepth(fs::path path);
    bool execSimple(const char* sql);
    int insertFilesBatch();
    int storeAndClear();

public:
    Indexer(std::vector<File>& fl, fs::path path, sqlite3* db);

    void indexFiles();
};
