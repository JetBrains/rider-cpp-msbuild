#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <optional>
#include <random>
#include <chrono>
#include <cctype>

// Demonstrates usage of several C++17 std library facilities in a small program.

static std::unordered_map<std::string, int> word_count(const std::string& text) {
    std::unordered_map<std::string, int> freq;
    std::istringstream iss(text);
    std::string w;
    while (iss >> w) {
        // normalize to lowercase and strip punctuation at ends
        auto begin = std::find_if(w.begin(), w.end(), [](unsigned char c){ return std::isalnum(c); });
        auto end = std::find_if(w.rbegin(), w.rend(), [](unsigned char c){ return std::isalnum(c); }).base();
        if (begin < end) {
            std::string norm(begin, end);
            std::transform(norm.begin(), norm.end(), norm.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            ++freq[norm];
        }
    }
    return freq;
}

static std::vector<std::pair<std::string,int>> top_k(const std::unordered_map<std::string,int>& freq, std::size_t k) {
    std::vector<std::pair<std::string,int>> items(freq.begin(), freq.end());
    // sort by count desc, then word asc
    std::sort(items.begin(), items.end(), [](const auto& a, const auto& b){
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    if (items.size() > k) items.resize(k);
    return items;
}

static std::pair<double, std::optional<double>> sum_and_median(std::vector<int> values) {
    if (values.empty()) return {0.0, std::nullopt};
    // nth_element to find median in O(n)
    std::size_t n = values.size();
    std::nth_element(values.begin(), values.begin() + n/2, values.end());
    if (n % 2 == 1) {
        double med = values[n/2];
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return {sum, med};
    } else {
        int a = values[n/2];
        std::nth_element(values.begin(), values.begin() + n/2 - 1, values.end());
        int b = values[n/2 - 1];
        double med = (a + b) / 2.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return {sum, med};
    }
}

int main(int argc, char** argv) {
    std::cout << "Hello from HelloNMake sample!\n";

    // 1) Use std to analyze text
    const std::string text = "C++ makes it easy to shoot yourself in the foot; std makes it harder, but when you do, it blows your whole leg off.";
    auto freq = word_count(text);
    auto top3 = top_k(freq, 3);

    std::cout << "Top 3 words:\n";
    for (const auto& item : top3) {
        const auto& w = item.first;
        int c = item.second;
        std::cout << "  '" << w << "' -> " << c << "\n";
    }

    // 2) Use random + algorithms to process numbers
    std::optional<unsigned int> seed;
    if (argc > 1) {
        try {
            unsigned int s = static_cast<unsigned int>(std::stoul(argv[1]));
            seed = std::make_optional(s);
        } catch (...) {}
    }

    std::mt19937 rng;
    if (seed) {
        rng.seed(*seed);
        std::cout << "Using user-provided seed: " << *seed << "\n";
    } else {
        auto now = static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        rng.seed(now);
        std::cout << "Using time-based seed.\n";
    }

    std::uniform_int_distribution<int> dist(1, 100);
    std::vector<int> values;
    values.resize(20);
    std::generate(values.begin(), values.end(), [&]{ return dist(rng); });

    // transform: square odd numbers, keep even as-is
    std::transform(values.begin(), values.end(), values.begin(), [](int x){ return (x % 2) ? x*x : x; });

    auto result = sum_and_median(values);
    double sum = result.first;
    auto median = result.second;

    std::cout << "Values: ";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i) std::cout << ", ";
        std::cout << values[i];
    }
    std::cout << "\n";

    std::cout << "Sum: " << sum << "\n";
    if (median) std::cout << "Median: " << *median << "\n";
    else std::cout << "Median: n/a\n";

    return 0;
}
