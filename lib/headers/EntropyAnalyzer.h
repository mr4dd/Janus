#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>

class EntropyAnalyzer {
public:
    
    static double process(const std::string& input);

private:    
    static std::vector<std::string_view> tokenize(std::string_view str);
    
    static double calculate_shannon_entropy(std::string_view token);
};