#include <benchmark/benchmark.h>

#include <vector>
#include <string>

#define DAYOFMONTH "Day Of Month"
#define MONTH "Month"
#define DAYTYPE "Day Type"

namespace current {
  class OSGridController {
    public:
      OSGridController() {};

      void addCategoryAndFields(const std::pair<std::string, std::vector<std::string>>& categoryAndFields) {
        m_categoriesAndFields.push_back(categoryAndFields);
      }

    private:
      std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
  };
}

namespace proposed {
  class OSGridController {
    public:
      OSGridController() {};

      void addCategoryAndFields(std::string header, std::vector<std::string> categoryAndFields) {
        m_categoriesAndFields.emplace_back(std::move(header), std::move(categoryAndFields));
      }

    private:
      std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
  };
}

static void Current(benchmark::State& state) {

  current::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields;
      fields.push_back(DAYOFMONTH);
      fields.push_back(MONTH);
      fields.push_back(DAYTYPE);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Date", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }
  }
}

static void ProposedInitial(benchmark::State& state) {

  current::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      grid.addCategoryAndFields(std::make_pair("Date", std::move(fields)));
    }
  }
}

/*
static void ProposedInitialMoveCurrent(benchmark::State& state) {

  current::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      grid.addCategoryAndFields(std::make_pair("Date", std::move(fields)));
    }
  }
}

static void ProposedNoMove(benchmark::State& state) {

  proposed::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      grid.addCategoryAndFields(std::make_pair("Date", fields));
    }
  }
}
*/

static void Proposed(benchmark::State& state) {

  proposed::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      //std::vector<std::string> fields;
      grid.addCategoryAndFields("Date", {DAYOFMONTH, MONTH, DAYTYPE});
    }
  }
}


BENCHMARK(Current)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(ProposedInitial)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
//BENCHMARK(ProposedInitialMoveCurrent)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
//BENCHMARK(ProposedNoMove)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(Proposed)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
