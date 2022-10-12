#include <benchmark/benchmark.h>

#include <string>
#include <string_view>

#include <boost/algorithm/string.hpp>
#include <ranges>
#include <utility>
#include <concepts>
#include <optional>

namespace ascii {

constexpr std::string_view ascii_trim_left(std::string_view s) {
  return s.substr(std::min(s.find_first_not_of(" \f\n\r\t\v"), s.size()));
}

constexpr std::string_view ascii_trim_right(std::string_view s) {
  return s.substr(0, std::min(s.find_last_not_of(" \f\n\r\t\v") + 1, s.size()));
}

constexpr std::string_view ascii_trim(std::string_view s) {
  return ascii_trim_left(ascii_trim_right(s));
}

inline void ascii_trim(std::string& str) {
  str = std::string(ascii_trim(std::string_view(str)));
}
}  // namespace ascii

std::string getTestString() {
  return "\tthis is a string   \n";
}

static void BM_Trim_Boost(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = getTestString();
    boost::trim(str);
    benchmark::DoNotOptimize(str);
  }
}

static void BM_Trim_ASCII(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = getTestString();
    ascii::ascii_trim(str);
    benchmark::DoNotOptimize(str);
  }
}

namespace range {

inline constexpr auto trim_front = std::views::drop_while(::isspace);

inline constexpr auto trim_back = std::views::reverse | trim_front | std::views::reverse;

std::string trim(std::string_view const in) {
  auto view = in | trim_front | trim_back;
  return {view.begin(), view.end()};
}

std::string trim2(std::string_view const in) {
  auto view = in | std::views::drop_while(isspace) | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;
  return {view.begin(), view.end()};
}

template <std::ranges::view V, typename Pred>
requires std::ranges::bidirectional_range<V>&& std::indirect_unary_predicate<Pred, std::ranges::iterator_t<V>> class drop_last_while_view
  : public std::ranges::view_interface<drop_last_while_view<V, Pred>>
{
 public:
  drop_last_while_view() = default;

  drop_last_while_view(V base, Pred pred) : base_(std::move(base)), pred_(std::move(pred)) {}

  auto begin() {
    return std::ranges::begin(base_);
  }

  auto end() {
    if (!cached_end_) {
      cached_end_ = std::ranges::find_if_not(std::ranges::rbegin(base_), std::ranges::rend(base_), pred_).base();
    }

    return *cached_end_;
  }

 private:
  [[no_unique_address]] V base_;
  [[no_unique_address]] Pred pred_;
  [[no_unique_address]] std::optional<std::ranges::iterator_t<V>> cached_end_;
};

namespace myviews {

#ifdef __GNUC__
using std::views::__adaptor::_RangeAdaptor;

struct drop_last_while_fn : _RangeAdaptor<drop_last_while_fn>
{
  using _RangeAdaptor<drop_last_while_fn>::operator();
  static constexpr int _S_arity = 2;
#else
struct drop_last_while_fn
{
#endif
  template <std::ranges::viewable_range R, typename Pred>
  constexpr auto operator()(R&& r, Pred pred) const -> range::drop_last_while_view<std::views::all_t<R>, Pred> {
    return range::drop_last_while_view<std::views::all_t<R>, Pred>(std::views::all(std::forward<R>(r)), std::move(pred));
  }
};

inline constexpr auto drop_last_while = drop_last_while_fn{};

}  // namespace myviews

std::string trim3(std::string_view const in) {
  auto view = in | std::views::drop_while(isspace) | myviews::drop_last_while(isspace);
  return {view.begin(), view.end()};
}

}  // namespace range

static void BM_Trim_Ranges(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = getTestString();
    str = range::trim(str);
    benchmark::DoNotOptimize(str);
  }
}

static void BM_Trim_Ranges2(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = getTestString();
    str = range::trim2(str);
    benchmark::DoNotOptimize(str);
  }
}

static void BM_Trim_Ranges3(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = getTestString();
    str = range::trim3(str);
    benchmark::DoNotOptimize(str);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_Trim_Boost);
BENCHMARK(BM_Trim_ASCII);
BENCHMARK(BM_Trim_Ranges);
BENCHMARK(BM_Trim_Ranges2);
BENCHMARK(BM_Trim_Ranges3);
