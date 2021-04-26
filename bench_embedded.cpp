#include <benchmark/benchmark.h>

#include <iostream>
#include <array>

static void CurrentCLike(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    static constexpr size_t embedded_file_count = 2;

    static const char *embedded_file_name_0 = ":/Resources/sandia/Sandia_Modules.json";
    static const char *embedded_file_name_1 = ":/Resources/sandia/Sandia_Modules2.json";

    static const uint8_t embedded_file_0[] = {0x78,0x9c,0xed,0xbd,0x5d,0xaf,0xe3,0xd8};
    static const uint8_t embedded_file_1[] = {0xed,0xaf,0xe3, 0xbd,0x5d,0xaf,0xe3,0xd8,0xe3};

    static const size_t embedded_file_len_0 = 8;
    static const size_t embedded_file_len_1 = 9;

    static const char *embedded_file_names[] = {
      embedded_file_name_0,
      embedded_file_name_1,

    };
    static const size_t embedded_file_lens[] = {
      embedded_file_len_0,
      embedded_file_len_1,

    };
    static const uint8_t *embedded_files[] = {
      embedded_file_0,
      embedded_file_1,
    };

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(embedded_file_names);
    benchmark::DoNotOptimize(embedded_file_lens);
    benchmark::DoNotOptimize(embedded_files);

    for (size_t i = 0; i < embedded_file_count; ++i) {
      auto embedded_file_name = embedded_file_names[i];
      benchmark::DoNotOptimize(embedded_file_name);
      for (size_t j = 0; j < embedded_file_lens[i]; ++j) {
        auto content = embedded_files[i][j];
        benchmark::DoNotOptimize(content);
      }
    }
  }
}
// Register the function as a benchmark
BENCHMARK(CurrentCLike);

static void ConstexprCLike(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    static constexpr size_t embedded_file_count = 2;

    static constexpr auto embedded_file_name_0 = ":/Resources/sandia/Sandia_Modules.json";
    static constexpr auto embedded_file_name_1 = ":/Resources/sandia/Sandia_Modules2.json";

    static constexpr uint8_t embedded_file_0[] = {0x78,0x9c,0xed,0xbd,0x5d,0xaf,0xe3,0xd8};
    static constexpr uint8_t embedded_file_1[] = {0xed,0xaf,0xe3, 0xbd,0x5d,0xaf,0xe3,0xd8,0xe3};

    static constexpr size_t embedded_file_len_0 = 8;
    static constexpr size_t embedded_file_len_1 = 9;

    static const char *embedded_file_names[] = {
      embedded_file_name_0,
      embedded_file_name_1,

    };
    static constexpr size_t embedded_file_lens[] = {
      embedded_file_len_0,
      embedded_file_len_1,

    };
    static const uint8_t *embedded_files[] = {
      embedded_file_0,
      embedded_file_1,
    };

    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(embedded_file_names);
    benchmark::DoNotOptimize(embedded_file_lens);
    benchmark::DoNotOptimize(embedded_files);

    for (size_t i = 0; i < embedded_file_count; ++i) {
      auto embedded_file_name = embedded_file_names[i];
      benchmark::DoNotOptimize(embedded_file_name);
      for (size_t j = 0; j < embedded_file_lens[i]; ++j) {
        auto content = embedded_files[i][j];
        benchmark::DoNotOptimize(content);
      }
    }
  }
}



// Register the function as a benchmark
BENCHMARK(ConstexprCLike);

/**
static void NewConstexprArray(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    static constexpr size_t embedded_file_count = 2;

    static constexpr auto embedded_file_name_0 = ":/Resources/sandia/Sandia_Modules.json";
    static constexpr auto embedded_file_name_1 = ":/Resources/sandia/Sandia_Modules2.json";

    static constexpr std::array<std::string_view, embedded_file_count> embedded_file_names {{embedded_file_name_0, embedded_file_name_1}};

    static constexpr size_t embedded_file_len_0 = 8;
    static constexpr std::array<uint8_t, embedded_file_len_0> embedded_file_0{0x78,0x9c,0xed,0xbd,0x5d,0xaf,0xe3,0xd8};
    static constexpr size_t embedded_file_len_1 = 9;
    static constexpr std::array<uint8_t, embedded_file_len_1> embedded_file_1{0xed,0xaf,0xe3, 0xbd,0x5d,0xaf,0xe3,0xd8,0xe3};


    static constexpr std::array<int, embedded_file_count> embedded_file_lens {
      embedded_file_len_0,
      embedded_file_len_1,
    };


    static constexpr std::array<std::array<uint8_t>, embedded_file_count> embedded_files = {
       embedded_file_0,
       embedded_file_1,
    };

    //static constexpr auto embedded_files{{
      //embedded_file_0,
      //embedded_file_1,
    //}};

    static const uint8_t *embedded_files[] = {
      embedded_file_0,
      embedded_file_1,
    };



    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(embedded_file_names);
    benchmark::DoNotOptimize(embedded_file_lens);
    benchmark::DoNotOptimize(embedded_files);

    for (size_t i = 0; i < embedded_file_count; ++i) {
      std::cout << embedded_file_lens[i] << "," << embedded_file_names[i] << '\n';
      auto embedded_file_name = embedded_file_names[i];
      benchmark::DoNotOptimize(embedded_file_name);
      for (auto j = 0; j < embedded_file_lens[i]; ++j) {
        auto content = embedded_files[i][j];
        benchmark::DoNotOptimize(content);
      }
    }
  }
}
// Register the function as a benchmark
BENCHMARK(NewConstexprArray);

*/
