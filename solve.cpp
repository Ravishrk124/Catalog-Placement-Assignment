#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <set>
#include <algorithm>

#include "json.hpp"
#include "BigInt.h"

using json = nlohmann::json;
using Point = std::pair<int, BigInt>;

// --- Helper Functions (No changes needed here) ---
int char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid character in number value");
}

BigInt string_to_bigint(const std::string& value_str, int base) {
    BigInt result = 0;
    BigInt power = 1;
    for (int i = value_str.length() - 1; i >= 0; i--) {
        result += BigInt(char_to_int(value_str[i])) * power;
        power *= base;
    }
    return result;
}

BigInt calculate_secret_for_combination(const std::vector<Point>& combination) {
    BigInt secret_c = 0;
    int k = combination.size();
    if (k == 0) return 0; // Avoid division by zero if combo is empty
    for (int j = 0; j < k; ++j) {
        long double lj_coeff_double = 1.0;
        for (int m = 0; m < k; ++m) {
            if (m == j) continue;
            lj_coeff_double *= static_cast<long double>(combination[m].first) /
                               static_cast<long double>(combination[m].first - combination[j].first);
        }
        secret_c += combination[j].second * static_cast<long long>(round(lj_coeff_double));
    }
    return secret_c;
}

void find_combinations(const std::vector<Point>& all_points, int k, int start_index,
                       std::vector<Point>& current_combo,
                       std::vector<std::vector<Point>>& all_combos) {
    if (current_combo.size() == k) {
        all_combos.push_back(current_combo);
        return;
    }
    for (size_t i = start_index; i < all_points.size(); ++i) {
        current_combo.push_back(all_points[i]);
        find_combinations(all_points, k, i + 1, current_combo, all_combos);
        current_combo.pop_back();
    }
}


// --- Main function with new deviation analysis ---
int main() {
    std::ifstream input_file("input.json");
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open input.json" << std::endl; return 1; }
    json data;
    input_file >> data;

    int k = data["keys"]["k"];
    std::vector<Point> all_points;
    for (auto& el : data.items()) {
        if (el.key() != "keys") {
            all_points.push_back({std::stoi(el.key()), string_to_bigint(el.value()["value"], std::stoi(el.value()["base"].get<std::string>()))});
        }
    }
    
    std::vector<std::vector<Point>> all_combinations;
    std::vector<Point> current_combo;
    find_combinations(all_points, k, 0, current_combo, all_combinations);

    if (all_combinations.empty()) {
        std::cerr << "Error: Not enough points to form a combination." << std::endl; return 1; }
        
    std::map<BigInt, int> results_count;
    for (const auto& combo : all_combinations) {
        results_count[calculate_secret_for_combination(combo)]++;
    }

    auto winning_pair = *std::max_element(results_count.begin(), results_count.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    BigInt final_secret = winning_pair.first;

   

    std::vector<Point> deviated_points;
    std::vector<Point> good_combo;

    // Find the first combination that produced the winning secret to use as a trusted "basis"
    for (const auto& combo : all_combinations) {
        if (calculate_secret_for_combination(combo) == final_secret) {
            good_combo = combo;
            break;
        }
    }
    
    std::cout << "\n--- Analysis Report ---" << std::endl;

    if (good_combo.empty() || all_points.size() <= k) {
        std::cout << "Deviation Check: Not enough redundant data to reliably check for deviation." << std::endl;
    } else {
        // Test each original point against a trusted subset of the good combo
        std::vector<Point> basis_subset(good_combo.begin(), good_combo.begin() + k - 1);
        for (const auto& point_to_test : all_points) {
            std::vector<Point> test_combo = basis_subset;
            test_combo.push_back(point_to_test);

            // Sort by x-value to keep combinations consistent, though not strictly necessary
            std::sort(test_combo.begin(), test_combo.end());

            BigInt test_secret = calculate_secret_for_combination(test_combo);
            if (test_secret != final_secret) {
                deviated_points.push_back(point_to_test);
            }
        }
    }
    
    if (deviated_points.empty()) {
        std::cout << "Deviation Found: No" << std::endl;
        std::cout << "All points appear to be consistent." << std::endl;
    } else {
        std::cout << "Deviation Found: Yes" << std::endl;
        std::cout << "The following point(s) are inconsistent with the consensus curve:" << std::endl;
        for (const auto& p : deviated_points) {
            std::cout << "  - Point with x=" << p.first << " is deviated." << std::endl;
        }
    }
    
    std::cout << "\n----------------------------------" << std::endl;
    std::cout << "The verified secret (c) is:" << std::endl;
    std::cout << final_secret << std::endl;
    std::cout << "----------------------------------" << std::endl;

    return 0;
}
