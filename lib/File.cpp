#include "File.h"

#include <string>

using namespace std;

File::File(string name, string extension, string path, size_t size, double epoch, int depth) {
            fName=name;
            fExtension=extension;
            fPath=path;
            fSize=size;
            fEpoch=epoch;
            fDepth=depth;
}