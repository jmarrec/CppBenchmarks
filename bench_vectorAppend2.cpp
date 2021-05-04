#include <benchmark/benchmark.h>

#include <vector>
#include <memory>
#include <string>

#include <fmt/format.h>

class X_Impl {
    private:
    std::string i;
    std::string j;
    std::string k;
    std::string l;
};


class X_NoRuleOf0_MemberInit
{
public:
   X_NoRuleOf0_MemberInit() : impl{std::make_shared<X_Impl>()} {}

   // break performance for the fun of it!!
   virtual ~X_NoRuleOf0_MemberInit() {}
  private:
    std::shared_ptr<X_Impl> impl;
};

// this example shows what happens if you break the rule of 0
// by providing an empty destructor
static void WithNoRuleOf0MemberInit(benchmark::State& state) {

  struct MyEntry {
    MyEntry(X_NoRuleOf0_MemberInit t_x) : x(std::move(t_x)) {  };
    X_NoRuleOf0_MemberInit x;

    // break performance for the fun of it!!
    ~MyEntry() {}
  };

  std::vector<MyEntry> newSurfaces1;
  for (int64_t i = 0; i < state.range(0); ++i) {
    newSurfaces1.emplace_back(X_NoRuleOf0_MemberInit{});
  }

  // size_t vecSize = 1 + state.range(0);

  MyEntry surface(X_NoRuleOf0_MemberInit{});

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.push_back(surface);
    vec.insert(vec.end(), newSurfaces1.begin(), newSurfaces1.end());
  }

  state.SetComplexityN(state.range(0));

}
// Register the function as a benchmark

static void WithNoRuleOf0MemberInit_reserve(benchmark::State& state) {

  struct MyEntry {
    // 0 copies in this version, but one move per object's construction
    MyEntry(X_NoRuleOf0_MemberInit t_x) : x(std::move(t_x)) {  };
    X_NoRuleOf0_MemberInit x;
  };


  std::vector<MyEntry> newSurfaces1;
  for (int64_t i = 0; i < state.range(0); ++i) {
    newSurfaces1.emplace_back(X_NoRuleOf0_MemberInit{});
  }

/*
 *  std::vector<MyEntry> newSurfaces2;
 *  for (int64_t i = 0; i < state.range(0); ++i) {
 *    newSurfaces1.emplace_back(X_NoRuleOf0_MemberInit{});
 *  }
 *
 */
  size_t vecSize = 1 + state.range(0);

  MyEntry surface(X_NoRuleOf0_MemberInit{});

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::vector<MyEntry> vec;
    // by reserving ahead of time we know that there will be no data moves or copies
    vec.reserve(vecSize);
    vec.push_back(surface);
    vec.insert(vec.end(), newSurfaces1.begin(), newSurfaces1.end());
  }

  state.SetComplexityN(state.range(0));
}
// Register the function as a benchmark

BENCHMARK(WithNoRuleOf0MemberInit)
  ->Range(8, 8<<10)
  ->Complexity();

BENCHMARK(WithNoRuleOf0MemberInit_reserve)
  ->Range(8, 8<<10)
  ->Complexity();
