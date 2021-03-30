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

    vectorOfStrings[0] = "a";
    vectorOfStrings[1] = "b";
    vectorOfStrings[2] = "c";
    vectorOfStrings[3] = "d";
    vectorOfStrings[4] = "e";
    vectorOfStrings[5] = "f";
    vectorOfStrings[6] = "g";
    vectorOfStrings[7] = "h";
    vectorOfStrings[8] = "i";
    vectorOfStrings[9] = "j";
    vectorOfStrings[10] = "A";
    vectorOfStrings[11] = "X";

    vectorOfStrings[55] = "u";
    vectorOfStrings[56] = "l";

    return vectorOfStrings;
}


static void NoCache(benchmark::State& state) {

    std::vector<std::string> vectorOfStrings = makeVector(100);

    for (auto _ : state) {

        constexpr size_t manualFixSize = 52;
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
                {"A"sv, "A"sv},
                {"B"sv, "B"sv},
                {"C"sv, "C"sv},
                {"D"sv, "C"sv},
                {"E"sv, "C"sv},
                {"F"sv, "C"sv},
                {"G"sv, "C"sv},
                {"H"sv, "C"sv},
                {"I"sv, "C"sv},
                {"J"sv, "C"sv},
                {"K"sv, "C"sv},
                {"L"sv, "C"sv},
                {"M"sv, "C"sv},
                {"N"sv, "C"sv},
                {"O"sv, "C"sv},
                {"P"sv, "C"sv},
                {"Q"sv, "C"sv},
                {"R"sv, "C"sv},
                {"S"sv, "C"sv},
                {"T"sv, "C"sv},
                {"U"sv, "C"sv},
                {"V"sv, "C"sv},
                {"W"sv, "C"sv},
                {"X"sv, "C"sv},
                {"Y"sv, "C"sv},
                {"Z"sv, "C"sv}
            }
        };

        std::map<std::string, std::string> fieldNames;

        for (const auto& fieldNameInput: vectorOfStrings) {
            std::string myval;
            if (const auto& cached_value = fieldNames.find(fieldNameInput); cached_value != fieldNames.end()) {
              myval = cached_value->second;
              continue;
            }

            const auto cache_value = [&](const auto& input, const auto& value) -> const auto& {
              return fieldNames.emplace(input, value).first->second;
            };

            auto it = std::find_if(manualFixPairs.cbegin(), manualFixPairs.cend(), [&fieldNameInput](const auto& p) { return p.first == fieldNameInput; });
            if (it != manualFixPairs.cend()) {
                myval = cache_value(fieldNameInput, it->second);
                continue;
            }

            myval = cache_value(fieldNameInput, fieldNameInput);
            benchmark::DoNotOptimize(myval);
        }
    }
}


// Register the function as a benchmark
BENCHMARK(NoCache);


static void WithCache(benchmark::State& state) {

    std::vector<std::string> vectorOfStrings = makeVector(100);


    for (auto _ : state) {
        constexpr size_t manualFixSize = 52;
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
                {"t"sv, "C"sv},
                {"u"sv, "C"sv},
                {"v"sv, "C"sv},
                {"w"sv, "C"sv},
                {"x"sv, "C"sv},
                {"y"sv, "C"sv},
                {"z"sv, "C"sv},
                {"A"sv, "A"sv},
                {"B"sv, "B"sv},
                {"C"sv, "C"sv},
                {"D"sv, "C"sv},
                {"E"sv, "C"sv},
                {"F"sv, "C"sv},
                {"G"sv, "C"sv},
                {"H"sv, "C"sv},
                {"I"sv, "C"sv},
                {"J"sv, "C"sv},
                {"K"sv, "C"sv},
                {"L"sv, "C"sv},
                {"M"sv, "C"sv},
                {"N"sv, "C"sv},
                {"O"sv, "C"sv},
                {"P"sv, "C"sv},
                {"Q"sv, "C"sv},
                {"R"sv, "C"sv},
                {"S"sv, "C"sv},
                {"T"sv, "C"sv},
                {"U"sv, "C"sv},
                {"V"sv, "C"sv},
                {"W"sv, "C"sv},
                {"X"sv, "C"sv},
                {"Y"sv, "C"sv},
                {"Z"sv, "C"sv}
            }
        };
        std::array<bool, manualFixSize> alreadyDones;
        alreadyDones.fill(false);


        std::map<std::string, std::string> fieldNames;

        for (const auto& fieldNameInput: vectorOfStrings) {
            std::string myval;
            if (const auto& cached_value = fieldNames.find(fieldNameInput); cached_value != fieldNames.end()) {
              myval = cached_value->second;
              continue;
            }

            const auto cache_value = [&](const auto& input, const auto& value) -> const auto& {
              return fieldNames.emplace(input, value).first->second;
            };

            auto it = std::find_if(manualFixPairs.cbegin(), manualFixPairs.cend(),
                    [&fieldNameInput, &alreadyDones, idx = 0](const auto& p) mutable {
                        int i = idx++;
                        if (!alreadyDones[i] && (p.first == fieldNameInput)) {
                            alreadyDones[i] = true;
                            return true;
                        }
                        return false;
                    });
            if (it != manualFixPairs.cend()) {
                myval = cache_value(fieldNameInput, it->second);
                continue;
            }

            myval = cache_value(fieldNameInput, fieldNameInput);
            benchmark::DoNotOptimize(myval);
        }
    }
}


// Register the function as a benchmark
BENCHMARK(WithCache);


static void NotConstexpr(benchmark::State& state) {


    std::vector<std::string> vectorOfStrings = makeVector(100);

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
                {"A"sv, "A"sv},
                {"B"sv, "B"sv},
                {"C"sv, "C"sv},
                {"D"sv, "C"sv},
                {"E"sv, "C"sv},
                {"F"sv, "C"sv},
                {"G"sv, "C"sv},
                {"H"sv, "C"sv},
                {"I"sv, "C"sv},
                {"J"sv, "C"sv},
                {"K"sv, "C"sv},
                {"L"sv, "C"sv},
                {"M"sv, "C"sv},
                {"N"sv, "C"sv},
                {"O"sv, "C"sv},
                {"P"sv, "C"sv},
                {"Q"sv, "C"sv},
                {"R"sv, "C"sv},
                {"S"sv, "C"sv},
                {"T"sv, "C"sv},
                {"U"sv, "C"sv},
                {"V"sv, "C"sv},
                {"W"sv, "C"sv},
                {"X"sv, "C"sv},
                {"Y"sv, "C"sv},
                {"Z"sv, "C"sv}
            };


        std::map<std::string, std::string> fieldNames;

        for (const auto& fieldNameInput: vectorOfStrings) {
            std::string myval;
            if (const auto& cached_value = fieldNames.find(fieldNameInput); cached_value != fieldNames.end()) {
              myval = cached_value->second;
              continue;
            }

            const auto cache_value = [&](const auto& input, const auto& value) -> const auto& {
              return fieldNames.emplace(input, value).first->second;
            };

            auto it = manualFixPairs.find(fieldNameInput);
            if (it != manualFixPairs.end()) {
                myval = it->second;
                manualFixPairs.erase(it);
                continue;
            }

            myval = cache_value(fieldNameInput, fieldNameInput);
            benchmark::DoNotOptimize(myval);
        }

    }
}


// Register the function as a benchmark
BENCHMARK(NotConstexpr);
