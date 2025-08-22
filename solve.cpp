#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "json.hpp"
#include "BigInt.h"

// Use the nlohmann namespace for JSON parsing
using json = nlohmann::json;

// Function to convert a character to its integer value (for bases > 10)
int char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid character in number value");
}

// Function to convert a string representation of a number in a given base to a BigInt
BigInt string_to_bigint(const std::string& value_str, int base) {
    BigInt result = 0;
    BigInt power = 1;

    for (int i = value_str.length() - 1; i >= 0; i--) {
        result += BigInt(char_to_int(value_str[i])) * power;
        power *= base;
    }
    return result;
}

// Function to calculate the Lagrange basis polynomial value L_j(0)
long long lagrange_basis(int j, int k) {
    long double product = 1.0;
    for (int m = 0; m < k; ++m) {
        if (m == j) continue;
        // Using x_i = i + 1
        product *= static_cast<long double>(m + 1) / (static_cast<long double>(m + 1) - static_cast<long double>(j + 1));
    }
    return static_cast<long long>(round(product));
}

int main() {
    // 1. Read the JSON file
    std::ifstream input_file("input.json");
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open input.json" << std::endl;
        return 1;
    }
    json data;
    input_file >> data;

    // 2. Parse keys and decode data points
    int k = data["keys"]["k"];
    std::vector<std::pair<int, BigInt>> points;

    std::cout << "Decoding " << k << " points..." << std::endl;
    for (int i = 1; i <= k; ++i) {
        std::string key = std::to_string(i);
        int base = std::stoi(data[key]["base"].get<std::string>());
        std::string value_str = data[key]["value"];
        
        BigInt y = string_to_bigint(value_str, base);
        points.push_back({i, y});
    }
    std::cout << "Successfully decoded points." << std::endl;

    // 3. Calculate Lagrange coefficients L_j(0) and the secret 'c'
    BigInt secret_c = 0;
    for (int j = 0; j < k; ++j) {
        long long lj_coeff = lagrange_basis(j, k);
        secret_c += points[j].second * lj_coeff;
    }

    // 4. Print the final result
    std::cout << "\n----------------------------------" << std::endl;
    std::cout << "The calculated secret (c) is:" << std::endl;
    std::cout << secret_c << std::endl;
    std::cout << "----------------------------------" << std::endl;

    return 0;
}