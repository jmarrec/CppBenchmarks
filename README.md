# CppBenchmarks

A collection of some benchmark scripts I used for C++.

The `CMakeLists.txt` will add some [conan](https://github.com/conan-io/conan) dependencies (such as [Google Benchmark library](https://github.com/google/benchmark)), and do some other magic (enable ccache, etc).

Tested with:

* GCC 10.1.0
* cmake 3.19.2
* conan 1.32.1

One executable will be made for each cpp file that starts with `bench`: `bench_<name>.cpp` will create a `build/Products/<name>` executable.

As usual, you can pass options to the executable:

```shell
mkdir build/
cd build/
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON ../
cd ../ && ln -s build/compile_commands.json . && cd build/
ninja
./Products/bench_basics --benchmark_out=results.csv --benchmark_out_format=csv

./Products/bench_basics --help
benchmark [--benchmark_list_tests={true|false}]
          [--benchmark_filter=<regex>]
          [--benchmark_min_time=<min_time>]
          [--benchmark_repetitions=<num_repetitions>]
          [--benchmark_report_aggregates_only={true|false}]
          [--benchmark_display_aggregates_only={true|false}]
          [--benchmark_format=<console|json|csv>]
          [--benchmark_out=<filename>]
          [--benchmark_out_format=<json|console|csv>]
          [--benchmark_color={auto|true|false}]
          [--benchmark_counters_tabular={true|false}]
          [--v=<verbosity>]
```
