#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstring>
#include <execution>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <random>

class Space
{
 public:
  Space(const std::string& name) : m_name(name){};
  std::string nameString() const {
    return m_name;
  }

 private:
  std::string m_name;
  std::string m_other;
  std::string m_other2;
};

auto generate_random_alphanumeric_string(std::size_t len) -> std::string {
  static constexpr auto chars = "0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz";
  std::random_device rd;   //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());  //Standard mersenne_twister_engine seeded with rd()
  auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
  auto result = std::string(len, '\0');
  std::generate_n(begin(result), len, [&]() { return chars[dist(gen)]; });
  return result;
}

auto makeVectorOfStrings(std::size_t len) -> std::vector<std::string> {
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

void makeDuplicates(std::vector<std::string>& vec) {
  for (size_t i = 0; i < vec.size(); i = i + 10) {
    vec[i] = "A";
  }
}

auto makeVectorOfSpaces(std::size_t len) -> std::vector<Space> {
  auto vectorOfStrings = makeVectorOfStrings(len);
  makeDuplicates(vectorOfStrings);
  std::vector<Space> result;
  result.reserve(len);
  std::transform(vectorOfStrings.begin(), vectorOfStrings.end(), std::back_inserter(result), [](const auto& s) { return Space(s); });
  return result;
}

static void BM_RawLoop(benchmark::State& state) {

  std::vector<Space> vectorOfSpaces = makeVectorOfSpaces(state.range(0));

  for (auto _ : state) {
    std::set<std::string> result;
    for (const auto& s : vectorOfSpaces) {
      result.insert(s.nameString());
    }
    benchmark::DoNotOptimize(result);
  }
  state.SetComplexityN(state.range(0));
}

static void BM_Transform(benchmark::State& state) {

  std::vector<Space> vectorOfSpaces = makeVectorOfSpaces(state.range(0));

  for (auto _ : state) {

    std::set<std::string> result;
    std::transform(vectorOfSpaces.cbegin(), vectorOfSpaces.cend(), std::inserter(result, result.begin()),
                   [](const auto& s) { return s.nameString(); });
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_RawLoop)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(BM_Transform)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
