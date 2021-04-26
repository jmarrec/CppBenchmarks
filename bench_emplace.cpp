#include <benchmark/benchmark.h>

#include <vector>
#include <string>

#define DAYOFMONTH "Day Of Month"
#define MONTH "Month"
#define DAYTYPE "Day Type"


class OSGridController {
  public:
    OSGridController() {};

    void addCategoryAndFields(const std::pair<std::string, std::vector<std::string>>& categoryAndFields) {
      m_categoriesAndFields.push_back(categoryAndFields);
    }

  private:
    std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
};

static void Current(benchmark::State& state) {

  std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields;
      fields.push_back(DAYOFMONTH);
      fields.push_back(MONTH);
      fields.push_back(DAYTYPE);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Date", fields);
      m_categoriesAndFields.push_back(categoryAndFields);
    }
  }

}

static void Proposed(benchmark::State& state) {

  std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      m_categoriesAndFields.push_back(std::make_pair("Date", fields));
    }
  }
}

static void Proposed2(benchmark::State& state) {

  std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      m_categoriesAndFields.emplace_back(std::make_pair("Date", fields));
    }
  }
}

BENCHMARK(Current)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(Proposed)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(Proposed2)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
