#include <benchmark/benchmark.h>

#include <functional>
#include <string>
#include <string_view>

constexpr auto sampleMessage = "sdlkjfnldsjlfkjdslkfkdsnf  sfjdisljfldjslfjlkdsjflksdjijflsd  shfiouhzajhaajuihaeiuhhnb sd fhfhsnkjfnskdjhfs";

struct DataGlobal
{

  std::function<int(const std::string&)> messageCallback = nullptr;
};

void registerStdOutCallbackCopyAssign(DataGlobal& dataGlobal, std::function<int(const std::string&)> f) {
  dataGlobal.messageCallback = f;
}

void registerStdOutCallbackCopyMove(DataGlobal& dataGlobal, std::function<int(const std::string&)> f) {
  dataGlobal.messageCallback = std::move(f);
}

void registerStdOutCallbackByRefCopyAssign(DataGlobal& dataGlobal, const std::function<int(const std::string&)>& f) {
  dataGlobal.messageCallback = f;
}

static void BM_ByValCopyAssign(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    DataGlobal d1;
    registerStdOutCallbackCopyAssign(d1, [](const std::string& s) {
      int i = 0;
      for (auto c : s) {
        i += static_cast<int>(c);
      }
      return i;
    });
    int i = d1.messageCallback(sampleMessage);
    benchmark::DoNotOptimize(i);
  }
}

static void BM_ByValCopyMove(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    DataGlobal d1;
    registerStdOutCallbackCopyMove(d1, [](const std::string& s) {
      int i = 0;
      for (auto c : s) {
        i += static_cast<int>(c);
      }
      return i;
    });
    int i = d1.messageCallback(sampleMessage);
    benchmark::DoNotOptimize(i);
  }
}

static void BM_ByRefCopyAssign(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    DataGlobal d1;
    registerStdOutCallbackByRefCopyAssign(d1, [](const std::string& s) {
      int i = 0;
      for (auto c : s) {
        i += static_cast<int>(c);
      }
      return i;
    });
    int i = d1.messageCallback(sampleMessage);
    benchmark::DoNotOptimize(i);
  }
}

BENCHMARK(BM_ByValCopyAssign);
BENCHMARK(BM_ByValCopyMove);
BENCHMARK(BM_ByRefCopyAssign);
