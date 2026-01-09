#include "DVScore.h"
#include "File.h"
#include "EntropyAnalyzer.h"
#include <string>
#include <cmath>
#include <regex>
#include <vector>
#include <algorithm>

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
    double locationBonus = 0.0;
    if (file.fPath.find("/home") != std::string::npos) {
        locationBonus = 0.5;
    }
    return 1/(std::log2(file.fDepth + 1)) + locationBonus;
}

void DVScore::caluclateUtility() {
    utility = (nameWeight * calculateNameScore()) +
        (metadataWeight * calculateMetaScore()) +
        (contextWeight * calculateContextScore());
}

double DVScore::getUtility() {
    return utility;
}

void DVScore::calculateSensitivity(const std::string& fileContent) {
    const double extensionScore = extMap.count(file.fExtension) ? extMap.at(file.fExtension) : 0.0;
    const double sensitivityValue = (std::log(countMarkers(fileContent) + 1) /
                                (file.fSize/1024)) * extensionScore;
    sensitivity = std::clamp(sensitivityValue, 0.0, 1.0);
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

int DVScore::countMarkers(const std::string& fileContent) {
    int totalOccurrences = 0;

    std::vector<std::string> patterns = {
        R"(AKIA[A-Z0-9]{16})",
        R"(AIza[0-9A-Za-z\\-_]{35})",
        R"(sk_live_[0-9a-zA-Z]{24})",
        R"(ghp_[a-zA-Z0-9]{36})",
        R"(glpat-[a-zA-Z0-9\-]{20})",
        R"(eyJhbGciOi[A-Za-z0-9-_=]+\.[A-Za-z0-9-_=]+\.?[A-Za-z0-9-_.+/=]*)",
        R"(Authorization:\s*(Bearer|Basic)\s+[A-Za-z0-9._~+/-]+=*)",
        R"(-----BEGIN\s+(RSA|OPENSSH|PGP)\s+PRIVATE\s+KEY)",
        R"(ssh-rsa\s+AAAA[A-Za-z0-9+/]+)",
        R"((mongodb(\+srv)?|postgres|redis|amqp):\/\/[^\s]+)",
        R"(https:\/\/hooks\.slack\.com\/services\/T[A-Z0-9]{8}\/[^\s]+)",
        R"(10\.\d{1,3}\.\d{1,3}\.\d{1,3})",
        R"(172\.(1[6-9]|2[0-9]|3[0-1])\.\d{1,3}\.\d{1,3})",
        R"(192\.168\.\d{1,3}\.\d{1,3})",
        R"(xoxb-[0-9]{11}-[0-9]{11}-[a-zA-Z0-9]{24})",
        R"(SG\.[a-zA-Z0-9_-]{22}\.[a-zA-Z0-9_-]{43})",
        R"(AccountName=[a-z0-9]{3,24};AccountKey=[A-Za-z0-9+/=]{88})",
        R"((SECRET_KEY_BASE|DJANGO_SECRET_KEY)\s*=\s*['"][^'"]+['"])"
    };

    for (const auto& pattern : patterns) {
        try {
            std::regex re(pattern, std::regex_constants::optimize);
            auto words_begin = std::sregex_iterator(fileContent.begin(), fileContent.end(), re);
            auto words_end = std::sregex_iterator();

            totalOccurrences += std::distance(words_begin, words_end);
        } catch (const std::regex_error& e) {
            continue;
        }
    }

    return totalOccurrences;
}