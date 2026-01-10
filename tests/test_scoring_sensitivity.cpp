#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "File.h"
#include "DVScore.h"
#include <iostream>
#include <string>

void printSens(const std::string& type, const std::string& name, double score) {
    std::cout << "[" << type << "] File: " << name << " | Sensitivity Score: " << score << std::endl;
}
TEST_CASE("Data Sensitivity Heuristic - 10 Point Stress Test", "[DVScore][Sensitivity]") {
    const double now = 1736445440.0;
    
    SECTION("High Sensitivity - Credentials and Secrets") {
        
        std::string rsa = "-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEA75X+xyz123...[REDACTED]...==\n-----END RSA PRIVATE KEY-----";
        File f1("id_rsa", "pem", "/home/user/.ssh/", rsa.length(), now, 1);
        DVScore d1(f1); d1.calculateSensitivity(rsa);
        printSens("HIGH", f1.fName, d1.getSensitivity());
        CHECK(d1.getSensitivity() >= 0.85);
        
        std::string api = "sk_live_51Mz9X2LpY9z8v1a2b3c4d5e6f7g8h9i0j";
        File f2("api_keys", "json", "/home/user/deploy/", api.length(), now, 2);
        DVScore d2(f2); d2.calculateSensitivity(api);
        printSens("HIGH", f2.fName, d2.getSensitivity());
        CHECK(d2.getSensitivity() >= 0.75);
        
        std::string db = "mongodb+srv://admin:StrongP@ssw0rd2026@cluster0.mongodb.net/prod_db";
        File f3("env_prod", "local", "/var/www/app/", db.length(), now, 2);
        DVScore d3(f3); d3.calculateSensitivity(db);
        printSens("HIGH", f3.fName, d3.getSensitivity());
        CHECK(d3.getSensitivity() >= 0.70);
        
        std::string auth = "admin:$apr1$7j2kl9$P18Xp0Xm4E8G9zQW7rT8u/";
        File f4(".htpasswd", "", "/etc/apache2/", auth.length(), now, 1);
        DVScore d4(f4); d4.calculateSensitivity(auth);
        printSens("HIGH", f4.fName, d4.getSensitivity());
        CHECK(d4.getSensitivity() >= 0.65);
        
        std::string aws = "[default]\naws_access_key_id=AKIAXEXAMPLE\naws_secret_access_key=wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";
        File f5("credentials", "ini", "~/.aws/", aws.length(), now, 1);
        DVScore d5(f5); d5.calculateSensitivity(aws);
        printSens("HIGH", f5.fName, d5.getSensitivity());
        CHECK(d5.getSensitivity() >= 0.80);
    }

    SECTION("Low Sensitivity - Public or Metadata") {
        
        std::string log = "127.0.0.1 - - [10/Jan/2026:15:00:00] \"GET /static/style.css HTTP/1.1\" 200 1024";
        File f6("access", "log", "/var/log/nginx/", log.length(), now, 5);
        DVScore d6(f6); d6.calculateSensitivity(log);
        printSens("LOW ", f6.fName, d6.getSensitivity());
        CHECK(d6.getSensitivity() < 0.40);
        
        std::string css = "body { margin: 0; padding: 0; background-color: #f0f0f0; font-family: sans-serif; }";
        File f7("main", "css", "/var/www/html/", css.length(), now, 5);
        DVScore d7(f7); d7.calculateSensitivity(css);
        printSens("LOW ", f7.fName, d7.getSensitivity());
        CHECK(d7.getSensitivity() < 0.20);
        
        std::string md = "# Project Overview\nThis project is licensed under MIT. Contributions are welcome!";
        File f8("README", "md", "/home/user/project/", md.length(), now, 5);
        DVScore d8(f8); d8.calculateSensitivity(md);
        printSens("LOW ", f8.fName, d8.getSensitivity());
        CHECK(d8.getSensitivity() < 0.15);
        
        std::string build = "Scanning dependencies... [100%] Built target my_cpp_app";
        File f9("build", "out", "/tmp/", build.length(), now, 5);
        DVScore d9(f9); d9.calculateSensitivity(build);
        printSens("LOW ", f9.fName, d9.getSensitivity());
        CHECK(d9.getSensitivity() < 0.30);
        
        std::string words = "apple\nbanana\ncherry\ndate\neggplant\nfig\ngrape";
        File f10("words", "txt", "/usr/share/dict/", words.length(), now, 5);
        DVScore d10(f10); d10.calculateSensitivity(words);
        printSens("LOW ", f10.fName, d10.getSensitivity());
        CHECK(d10.getSensitivity() < 0.25);
    }
}