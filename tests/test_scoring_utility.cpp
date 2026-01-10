#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "File.h"
#include "DVScore.h"
#include <iostream>

TEST_CASE("User Utility Heuristic Stress Test", "[DVScore]") {
        const double now = 1736445440.0;

    SECTION("High Utility") {
        File f1("final_thesis", "docx", "/home/user/Uni/", 45000, now, 5);
        DVScore d1(f1); d1.caluclateUtility();
        CHECK(d1.getUtility() >= 0.7);

        File f2("todo_list", "txt", "/home/user/", 1024, now - 45000, 4);
        DVScore d2(f2); d2.caluclateUtility();
        CHECK(d2.getUtility() >= 0.55);

        File f3("budget_2026", "xlsx", "/home/user/Finance/", 55000, now - 145000, 5);
        DVScore d3(f3); d3.caluclateUtility();
        CHECK(d3.getUtility() >= 0.68);
    }

    SECTION("Medium Utility") {
        File f4("family_photo", "jpg", "/home/user/old/", 3500000, 1600000000.0, 5);
        DVScore d4(f4); d4.caluclateUtility();
        CHECK(d4.getUtility() >= 0.32);
        CHECK(d4.getUtility() <= 0.65);

        File f5("draft_notes", "tmp", "/home/user/Work/", 12000, now - 86400, 5);
        DVScore d5(f5); d5.caluclateUtility();
        CHECK(d5.getUtility() >= 0.32);
    }

    SECTION("Low Utility") {
        File f6("vmlinuz-gen", "bin", "/boot/", 9000000, now - 1000000, 3);
        DVScore d6(f6); d6.caluclateUtility();
        CHECK(d6.getUtility() < 0.32);

        File f7("libcrypto", "so", "/usr/lib/", 2500000, 1700000000.0, 4);
        DVScore d7(f7); d7.caluclateUtility();
        CHECK(d7.getUtility() < 0.32);
    }

    SECTION("Total Junk") {
        File f8("cache_db", "db", "/home/user/.cache/", 50000000, now, 5);
        DVScore d8(f8); d8.caluclateUtility();
        CHECK(d8.getUtility() <= 0.46);

        File f9("old_log_2022", "log", "/var/log/", 500000, 1400000000.0, 4);
        DVScore d9(f9); d9.caluclateUtility();
        CHECK(d9.getUtility() < 0.45);

        File f10("93f2a1", "dat", "/tmp/system/backup/01/", 1000, 1200000000.0, 6);
        DVScore d10(f10); d10.caluclateUtility();
        CHECK(d10.getUtility() < 0.25);
    }
}
