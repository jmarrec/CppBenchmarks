#include <benchmark/benchmark.h>

#include <iostream>
#include <array>
#include <string_view>
#include <algorithm>
#include <cstring>
#include <functional>
#include <random>
#include <string>
#include <map>


using namespace std::literals::string_view_literals;

auto generate_random_alphanumeric_string(std::size_t len) -> std::string {
    static constexpr auto chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
    auto result = std::string(len, '\0');
    std::generate_n(begin(result), len, [&]() { return chars[dist(gen)]; });
    return result;
}

auto makeVector(std::size_t len) -> std::vector<std::string> {
    std::vector<std::string> vectorOfStrings(len);

    auto genString = []() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 20);
        return generate_random_alphanumeric_string(dist(gen));
    };

    std::generate(vectorOfStrings.begin(), vectorOfStrings.end(), genString);
    return vectorOfStrings;
}


static void NoCache(benchmark::State& state) {

    std::vector<std::string> vectorOfStrings = makeVector(100);
    vectorOfStrings[0] = "a";
    vectorOfStrings[1] = "b";

    for (auto _ : state) {

        constexpr size_t manualFixSize = 26;
        static constexpr std::array<std::pair<std::string_view, std::string_view>, manualFixSize > manualFixPairs {
            {
                {"a"sv, "A"sv},
                {"b"sv, "B"sv},
                {"c"sv, "C"sv},
                {"d"sv, "C"sv},
                {"e"sv, "C"sv},
                {"f"sv, "C"sv},
                {"g"sv, "C"sv},
                {"h"sv, "C"sv},
                {"i"sv, "C"sv},
                {"j"sv, "C"sv},
                {"k"sv, "C"sv},
                {"l"sv, "C"sv},
                {"m"sv, "C"sv},
                {"n"sv, "C"sv},
                {"o"sv, "C"sv},
                {"p"sv, "C"sv},
                {"q"sv, "C"sv},
                {"r"sv, "C"sv},
                {"s"sv, "C"sv},
                {"t"sv, "C"sv},
                {"u"sv, "C"sv},
                {"v"sv, "C"sv},
                {"w"sv, "C"sv},
                {"x"sv, "C"sv},
                {"y"sv, "C"sv},
                {"z"sv, "C"sv},
            }
        };

        for (const auto& val: vectorOfStrings) {
            auto it = std::find_if(manualFixPairs.cbegin(), manualFixPairs.cend(), [&val](const auto& p) { return p.first == val; });
            if (it != manualFixPairs.cend()) {
                auto myval = it->second;
                benchmark::DoNotOptimize(myval);
            }
        }
    }
}


// Register the function as a benchmark
BENCHMARK(NoCache);


static void WithCache(benchmark::State& state) {

    std::vector<std::string> vectorOfStrings = makeVector(100);
    vectorOfStrings[0] = "a";
    vectorOfStrings[1] = "b";


    for (auto _ : state) {
        constexpr size_t manualFixSize = 26;
        static constexpr std::array<std::pair<std::string_view, std::string_view>, manualFixSize > manualFixPairs {
            {
                {"a"sv, "A"sv},
                {"b"sv, "B"sv},
                {"c"sv, "C"sv},
                {"d"sv, "C"sv},
                {"e"sv, "C"sv},
                {"f"sv, "C"sv},
                {"g"sv, "C"sv},
                {"h"sv, "C"sv},
                {"i"sv, "C"sv},
                {"j"sv, "C"sv},
                {"k"sv, "C"sv},
                {"l"sv, "C"sv},
                {"m"sv, "C"sv},
                {"n"sv, "C"sv},
                {"o"sv, "C"sv},
                {"p"sv, "C"sv},
                {"q"sv, "C"sv},
                {"r"sv, "C"sv},
                {"s"sv, "C"sv},
                {"t"sv, "C"sv},
                {"u"sv, "C"sv},
                {"v"sv, "C"sv},
                {"w"sv, "C"sv},
                {"x"sv, "C"sv},
                {"y"sv, "C"sv},
                {"z"sv, "C"sv},
            }
        };
        std::array<bool, manualFixSize> alreadyDones;
        alreadyDones.fill(false);

        for (const auto& val: vectorOfStrings) {
            auto it = std::find_if(manualFixPairs.cbegin(), manualFixPairs.cend(),
                    [&val, &alreadyDones, idx = 0](const auto& p) mutable {
                        int i = idx++;
                        if (!alreadyDones[i] && (p.first == val)) {
                            alreadyDones[i] = true;
                            return true;
                        }
                        return false;
                    });
            if (it != manualFixPairs.cend()) {
                auto myval = it->second;
                benchmark::DoNotOptimize(myval);
            }
        }
    }
}


// Register the function as a benchmark
BENCHMARK(WithCache);


static void NotConstexpr(benchmark::State& state) {


    std::vector<std::string> vectorOfStrings = makeVector(100);
    vectorOfStrings[0] = "a";
    vectorOfStrings[1] = "b";


    for (auto _ : state) {
        std::map<std::string_view, std::string_view> manualFixPairs
            {
                {"a"sv, "A"sv},
                {"b"sv, "B"sv},
                {"c"sv, "C"sv},
                {"d"sv, "C"sv},
                {"e"sv, "C"sv},
                {"f"sv, "C"sv},
                {"g"sv, "C"sv},
                {"h"sv, "C"sv},
                {"i"sv, "C"sv},
                {"j"sv, "C"sv},
                {"k"sv, "C"sv},
                {"l"sv, "C"sv},
                {"m"sv, "C"sv},
                {"n"sv, "C"sv},
                {"o"sv, "C"sv},
                {"p"sv, "C"sv},
                {"q"sv, "C"sv},
                {"r"sv, "C"sv},
                {"s"sv, "C"sv},
                {"t"sv, "C"sv},
                {"u"sv, "C"sv},
                {"v"sv, "C"sv},
                {"w"sv, "C"sv},
                {"x"sv, "C"sv},
                {"y"sv, "C"sv},
                {"z"sv, "C"sv},
            };


        for (const auto& val: vectorOfStrings) {
            auto it = manualFixPairs.find(val);
            if (it != manualFixPairs.end()) {
                auto myval = it->second;
                manualFixPairs.erase(it);
                benchmark::DoNotOptimize(myval);
            }
        }
    }
}


// Register the function as a benchmark
BENCHMARK(NotConstexpr);

