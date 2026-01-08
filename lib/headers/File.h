#pragma once

#include <string>

class File {
    public:
        std::string fName;
        std::string fExtension;
        std::string fPath;
        size_t fSize;
        double fEpoch;
        int fDepth;

        File(std::string, std::string, std::string, size_t, double, int);
};