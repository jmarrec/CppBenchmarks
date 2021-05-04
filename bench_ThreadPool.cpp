#include <benchmark/benchmark.h>

#include <string>
#include <sstream>
#include <vector>
#include <fmt/format.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>

std::string getText() {
  std::string result = R"(
  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

  OS:Space,
   {azklnksjdljfksdjkfjds},
   Autosize;

)";
  return result;
}

class IdfObject {
  public:
    IdfObject(const std::string& text) {
      std::string::size_type pos = 0;
      std::string::size_type prev = 0;

      while ((pos = text.find('\n', prev)) != std::string::npos) {
        m_fields.push_back(text.substr(prev, pos - prev));
        prev = pos + 1;
      }

      // To get the last substring (or only, if delimiter is not found)
      m_fields.push_back(text.substr(prev));
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

    }

    std::vector<std::string> m_fields;
};

namespace current {
  class IdfFile {
    public:
      IdfFile(const std::string& text) {

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = text.find(';', prev)) != std::string::npos) {
          auto idfObject = IdfObject(text.substr(prev, pos - prev));
          m_objects.push_back(idfObject);
          prev = pos + 1;
        }
        // To get the last substring (or only, if delimiter is not found)
       auto idfObject = IdfObject(text.substr(prev));
       m_objects.push_back(idfObject);
      }

      std::vector<IdfObject> m_objects;
  };
}

namespace parallel {
  class IdfFile {
    public:
      IdfFile(const std::string& text) {

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;

        // Build a thread pool to dispatch the loading of each IdfObject
        boost::asio::thread_pool pool(boost::thread::hardware_concurrency());
        // A mutex to avoid writing to m_objects at the same time
        boost::mutex mtx_;

        while ((pos = text.find(';', prev)) != std::string::npos) {

          auto thisText = text.substr(prev, pos - prev);

          boost::asio::post(pool, [this,
              &mtx_, thisText]() {

              auto idfObject = IdfObject(thisText);
              boost::lock_guard<boost::mutex> guard(mtx_);
              m_objects.push_back(idfObject);

              }
              );
          prev = pos + 1;
        }

        // To get the last substring (or only, if delimiter is not found)
        auto thisText = text.substr(prev);
        boost::asio::post(pool, [this,
            &mtx_, thisText]() {

            auto idfObject = IdfObject(thisText);
            boost::lock_guard<boost::mutex> guard(mtx_);
            m_objects.push_back(idfObject);

            }
            );

        // join the pool
        pool.join();
      }


      std::vector<IdfObject> m_objects;
  };
}

static void BM_ParseCurrent(benchmark::State& state) {
  for (auto _ : state) {
    current::IdfFile idfFile(getText());
  }
}

static void BM_ParseParallel(benchmark::State& state) {
  for (auto _ : state) {
    parallel::IdfFile idfFile(getText());
  }
}


// Register the function as a benchmark
BENCHMARK(BM_ParseCurrent);
BENCHMARK(BM_ParseParallel);

// Run the benchmark
BENCHMARK_MAIN();
