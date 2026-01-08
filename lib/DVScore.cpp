#include "DVScore.h"
#include "File.h"
#include "EntropyAnalyser.h"

#include <string>
#include <cmath>
#include <regex>

DVScore::DVScore(File f): file(f), utility(0), sensitivity(0) {}

double DVScore::calculateNameScore() {
    const int KeywordBoost = check_suffix(file.fName);
    const double Entropy = EntropyAnalyzer::process(file.fName);
    return (KeywordBoost+(1-Entropy))/2;
}

double DVScore::calculateMetaScore() {
    const double timeScore = std::exp(-decayConstant*file.fEpoch);
    const double sizeScore = 1 - 1/(1+std::exp(-0.1*(file.fSize/1024 - 150)));
    const double extensionScore = extMap.count(file.fExtension) ? extMap.at(file.fExtension) : 0.0;
    return 1/std::log2(timeScore + sizeScore + extensionScore);
}

double DVScore::calculateContextScore() {
    //ToDo add a location bonus
    return 1/(std::log2(file.fDepth + 1));
}

void DVScore::caluclateUtility() {
    utility = (nameWeight * calculateNameScore()) +
        (metadataWeight * calculateMetaScore()) +
        (contextWeight * calculateContextScore());
}

double DVScore::getUtility() {
    return utility;
}

void DVScore::CalculateSensitivity() {
}

double DVScore::getSensitivity() {
    return sensitivity;
}

int DVScore::check_suffix(const std::string& input) {
    static const std::regex suffix_pattern(R"((?:[0-9]+|v[0-9]{1,3})$)");
    if (std::regex_search(input, suffix_pattern)) {
        return 1;
    } else {
        return 0;
    }
}