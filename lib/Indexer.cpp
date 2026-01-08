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

    const char* sql =
        "INSERT OR IGNORE INTO files "
        "(Name, Extension, Path, Size, Epoch, Depth) "
        "VALUES (?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(DB) << endl;
        execSimple("ROLLBACK;");
        return rc;
    }

    for (const auto& file : FileList) {
        sqlite3_bind_text(stmt, 1, file.fName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, file.fExtension.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, file.fPath.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 4, file.fSize);
        sqlite3_bind_double(stmt, 5, file.fEpoch);
        sqlite3_bind_int(stmt, 6, file.fDepth);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "Insert failed: " << sqlite3_errmsg(DB) << endl;
            sqlite3_finalize(stmt);
            execSimple("ROLLBACK;");
            return rc;
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }

    sqlite3_finalize(stmt);

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
