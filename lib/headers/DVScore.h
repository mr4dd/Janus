#pragma once

#include <string>
#include <unordered_map>

#include "File.h"


class DVScore {
    private:
        const std::unordered_map<std::string, double> extMap = {
            {"txt",   0.98}, {"md",    0.95}, {"tex",   0.95}, {"me",    0.95},
            {"cpp",   0.90}, {"hpp",   0.90}, {"c",     0.90}, {"h",     0.90},
            {"py",    0.88}, {"java",  0.88}, {"cs",    0.88}, {"go",    0.85},
            {"rs",    0.88}, {"swift", 0.88}, {"kt",    0.88}, {"php",   0.85},
            {"js",    0.85}, {"ts",    0.85}, {"rb",    0.85}, {"lua",   0.85},
            {"css",   0.3}, {"env",  1.0}, {"html",  0.85}, {"sh",    0.85},

            {"docx",  0.82}, {"pdf",   0.75}, {"rtf",   0.80}, {"odt",   0.80},
            {"xlsx",  0.78}, {"pptx",  0.78}, {"csv",   0.70}, {"json",  0.72},
            {"yaml",  0.75}, {"xml",   0.70}, {"ini",   0.75}, {"conf",  0.75},

            {"jpg",   0.68}, {"jpeg",  0.68}, {"png",   0.65}, {"gif",   0.60},
            {"svg",   0.75}, {"mp3",   0.55}, {"wav",   0.50}, {"mp4",   0.55},
            {"mov",   0.55}, {"psd",   0.65}, {"ai",    0.65}, {"blend", 0.65},

            {"sql",   0.48}, {"db",    0.45}, {"sqlite", 0.45}, {"db3",   0.45},
            {"mdb",   0.40}, {"accdb", 0.40}, {"log",   0.35}, {"bak",   0.30},

            {"zip",   0.25}, {"tar",   0.25}, {"gz",    0.20}, {"7z",    0.20},
            {"rar",   0.20}, {"iso",   0.15}, {"dmg",   0.15}, {"jar",   0.20},

            {"exe",   0.05}, {"dll",   0.05}, {"so",    0.05}, {"bin",   0.02},
            {"dat",   0.08}, {"sys",   0.02}, {"obj",   0.05}, {"class", 0.05},
            {"o",     0.05}, {"pyc",   0.02}, {"tmp",   0.01}, {"com",   0.05}
        };
        const double decayConstant = 0.005;
        File file;
        double utility;
        double sensitivity;
        const double nameWeight = 0.5;
        const double metadataWeight = 0.4;
        const double contextWeight = 0.1;
        double calculateNameScore();
        double calculateMetaScore();
        double calculateContextScore();
        int check_suffix(const std::string&);
        int countMarkers(const std::string&);
    public:
        DVScore(File);
        void caluclateUtility();
        double getUtility();
        void calculateSensitivity(const std::string&);
        double getSensitivity();


};