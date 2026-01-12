#include "Indexer.h"

#include <iostream>
#include <chrono>
#include <filesystem>

#include "File.h"

using namespace std;

int Indexer::getDepth(fs::path path) {
    int depth = 0;
    for (auto it = path.begin(); it != path.end(); ++it)
        ++depth;
    return depth;
}

bool Indexer::execSimple(const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(DB, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        cerr << "SQLite error: " << err << endl;
        sqlite3_free(err);
        return false;
    }
    return true;
}

int Indexer::insertFilesBatch() {
    if (!execSimple("BEGIN TRANSACTION;")) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt* stmtFile = nullptr;
    sqlite3_stmt* stmtPathSelect = nullptr;
    sqlite3_stmt* stmtPathInsert = nullptr;
    int rc;

    const char* sqlPathSelect = "SELECT id FROM paths WHERE Path = ?";
    rc = sqlite3_prepare_v2(DB, sqlPathSelect, -1, &stmtPathSelect, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Prepare failed (Path SELECT): " << sqlite3_errmsg(DB) << endl;
        execSimple("ROLLBACK;");
        return rc;
    }
    const char* sqlPathInsert = "INSERT OR IGNORE INTO paths (path) VALUES (?)";
    rc = sqlite3_prepare_v2(DB, sqlPathInsert, -1, &stmtPathInsert, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Prepare failed (Path INSERT): " << sqlite3_errmsg(DB) << endl;
        execSimple("ROLLBACK;");
        return rc;
    }
    const char* sqlFile =
        "INSERT OR IGNORE INTO files "
        "(Name, Extension, Path_id, Size, Epoch, Depth) "
        "VALUES (?, ?, ?, ?, ?, ?)";
    rc = sqlite3_prepare_v2(DB, sqlFile, -1, &stmtFile, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Prepare failed (File INSERT): " << sqlite3_errmsg(DB) << endl;
        execSimple("ROLLBACK;");
        return rc;
    }
    for (const auto& file : FileList) {
        int pathId = 0;
        
        sqlite3_bind_text(stmtPathSelect, 1, file.fPath.c_str(), -1, SQLITE_TRANSIENT);
        rc = sqlite3_step(stmtPathSelect);
        if (rc == SQLITE_ROW) {
            pathId = sqlite3_column_int(stmtPathSelect, 0);
        } else {
            
            sqlite3_reset(stmtPathInsert);
            sqlite3_bind_text(stmtPathInsert, 1, file.fPath.c_str(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmtPathInsert);
            if (rc != SQLITE_DONE && rc != SQLITE_CONSTRAINT) { 
                cerr << "Insert path failed: " << sqlite3_errmsg(DB) << endl;
                sqlite3_finalize(stmtFile);
                sqlite3_finalize(stmtPathSelect);
                sqlite3_finalize(stmtPathInsert);
                execSimple("ROLLBACK;");
                return rc;
            }
            
            pathId = (int)sqlite3_last_insert_rowid(DB);
        }
        sqlite3_reset(stmtPathSelect);
        
        sqlite3_bind_text(stmtFile, 1, file.fName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmtFile, 2, file.fExtension.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmtFile, 3, pathId);
        sqlite3_bind_int64(stmtFile, 4, file.fSize);
        sqlite3_bind_double(stmtFile, 5, file.fEpoch);
        sqlite3_bind_int(stmtFile, 6, file.fDepth);

        rc = sqlite3_step(stmtFile);
        if (rc != SQLITE_DONE) {
            cerr << "Insert file failed: " << sqlite3_errmsg(DB) << endl;
            sqlite3_finalize(stmtFile);
            sqlite3_finalize(stmtPathSelect);
            sqlite3_finalize(stmtPathInsert);
            execSimple("ROLLBACK;");
            return rc;
        }

        sqlite3_reset(stmtFile);
        sqlite3_clear_bindings(stmtFile);
    }

    sqlite3_finalize(stmtFile);
    sqlite3_finalize(stmtPathSelect);
    sqlite3_finalize(stmtPathInsert);

    if (!execSimple("COMMIT;")) {
        return SQLITE_ERROR;
    }

    return SQLITE_OK;
}


int Indexer::storeAndClear() {
    if (!FileList.empty()) {
        if (insertFilesBatch() != SQLITE_OK) {
            return -1;
        }
        filecount += FileList.size();
        cout << "Wrote " << filecount << " file entries into the database!" << endl;
        FileList.clear();
    }
    return 0;
}

Indexer::Indexer(std::vector<File>& fl, fs::path path, sqlite3* db)
    : FileList(fl), rootPath(std::move(path)), DB(db) {}

void Indexer::indexFiles() {
    if (fs::exists(rootPath) && fs::is_directory(rootPath)) {
        for (const auto& entry :
            fs::recursive_directory_iterator(
                rootPath,
                fs::directory_options::skip_permission_denied)) {

            try {
                if (entry.is_regular_file() &&
                    fs::file_size(entry.path()) > 0 &&
                    !entry.is_symlink()) {

                    auto ftime = fs::last_write_time(entry.path());
                    auto sys_time =
                        chrono::time_point_cast<chrono::system_clock::duration>(
                            ftime - fs::file_time_type::clock::now()
                            + chrono::system_clock::now());

                    double epoch_time =
                        chrono::duration<double>(
                            sys_time.time_since_epoch()).count();

                    FileList.emplace_back(
                        entry.path().filename().string(),
                        entry.path().extension().string(),
                        fs::absolute(entry.path()).parent_path().string(),
                        fs::file_size(entry.path()),
                        epoch_time,
                        getDepth(entry.path()));
                }
            } catch (const fs::filesystem_error&) {
                // TODO: add logging
                continue;
            }

            if (FileList.size() > BATCH_SIZE) {
                int retries = 3;
                while (retries-- > 0 && storeAndClear() != 0) {
                    cerr << "storing data failed, retrying: "
                        << retries << endl;
                }
            }
        }
    }

    int retries = 3;
    while (retries-- > 0 && storeAndClear() != 0) {
        cerr << "storing data failed, retrying: "<< retries << endl;
    }
}
