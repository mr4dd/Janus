#include "EntropyAnalyzer.h"
#include <iostream>
#include <cmath>
#include <iomanip>

std::vector<std::string_view> EntropyAnalyzer::tokenize(std::string_view str) {
    std::vector<std::string_view> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find_first_of("-_. ", start)) != std::string_view::npos) {
        if (end > start) tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    if (start < str.length()) tokens.push_back(str.substr(start));
    return tokens;
}

double EntropyAnalyzer::calculate_shannon_entropy(std::string_view token) {
    if (token.empty()) return 0.0;

    std::unordered_map<char, int> frequencies;
    for (char c : token) {
        frequencies[c]++;
    }

    double entropy = 0.0;
    double len = static_cast<double>(token.length());

    for (auto const& [ch, count] : frequencies) {
        double p = count / len;
        entropy -= p * std::log2(p);
    }

    return entropy;
}

double EntropyAnalyzer::process(const std::string& input) {

    auto tokens = tokenize(input);
    
    double totalEntropy = 0.0;

    for (auto token : tokens) {
        totalEntropy -= calculate_shannon_entropy(token);
    }
    return totalEntropy;
}