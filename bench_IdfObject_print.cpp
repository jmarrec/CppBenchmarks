// Benchmark for https://github.com/NREL/OpenStudio/issues/4187

#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>
// #include <fmt/format.h>
#include <vector>
#include <filesystem>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace stdofstream {
  namespace withendl {
    class IdfObject {
      public:
        IdfObject(const std::string& t_name, const std::string& t_text)
        : m_name(t_name), m_text(t_text) {}

        /** Serialize this object to os as Idf text. */
        std::ostream& print(std::ostream& os) const {
          os << "IdfObject named " << m_name << ", text=" << m_text << std::endl;
          return os;
        }
      private:
        std::string m_name;
        std::string m_text;
    };

    class IdfFile {
      public:
        IdfFile() { };
        bool setHeader(const std::string& header) { m_header = header; return true; }
        bool addObject(const IdfObject& idfObject) { m_objects.push_back(idfObject); return true; }

        std::ostream& print(std::ostream& os) const {
          if (!m_header.empty()) {
            os << m_header << std::endl;
          }
          os << std::endl;
          for (const IdfObject& object : m_objects) {
            object.print(os);
          }
          return os;
        }

        bool save(std::filesystem::path wp) const {
          std::ofstream outFile(wp, std::ofstream::out);

          if (outFile) {
            try {
              print(outFile);
              outFile.close();
              return true;
            } catch (...) {
              std::cout << "Unable to write file to path '" << wp.string() << "'.";
              outFile.close();
              return false;
            }
          }
          return false;
        }


      private:
        std::string m_header;
        std::vector<IdfObject> m_objects;

    };

    std::ostream& operator<<(std::ostream& os, const IdfObject& idfObject) {
      return idfObject.print(os);
    }
  }

  namespace withn {
    class IdfObject {
      public:
        IdfObject(const std::string& t_name, const std::string& t_text)
        : m_name(t_name), m_text(t_text) {}

        /** Serialize this object to os as Idf text. */
        std::ostream& print(std::ostream& os) const {
          os << "IdfObject named " << m_name << ", text=" << m_text << '\n';
          return os;
        }
      private:
        std::string m_name;
        std::string m_text;
    };

    class IdfFile {
      public:
        IdfFile() { };
        bool setHeader(const std::string& header) { m_header = header; return true; }
        bool addObject(const IdfObject& idfObject) { m_objects.push_back(idfObject); return true; }

        std::ostream& print(std::ostream& os) const {
          if (!m_header.empty()) {
            os << m_header << '\n';
          }
          os << std::endl;
          for (const IdfObject& object : m_objects) {
            object.print(os);
          }
          return os;
        }

        bool save(std::filesystem::path wp) const {
          std::ofstream outFile(wp, std::ofstream::out);

          if (outFile) {
            try {
              print(outFile);
              outFile.close();
              return true;
            } catch (...) {
              std::cout << "Unable to write file to path '" << wp.string() << "'.";
              outFile.close();
              return false;
            }
          }
          return false;
        }


      private:
        std::string m_header;
        std::vector<IdfObject> m_objects;

    };

    std::ostream& operator<<(std::ostream& os, const IdfObject& idfObject) {
      return idfObject.print(os);
    }
  }


  static void BM_OutputN(benchmark::State& state) {
    withn::IdfFile idf_file;
    idf_file.setHeader("This is the header\n");
    for (int i = 0; i < 1000; ++i) {
       withn::IdfObject o1("Object1", "blablablablablalbla");
       idf_file.addObject(o1);
    }
    // Code inside this loop is measured repeatedly
    for (auto _ : state) {
      idf_file.save("file.txt");
    }
  }
  // Register the function as a benchmark
  BENCHMARK(BM_OutputN);

  static void BM_OutputEndl(benchmark::State& state) {
    withendl::IdfFile idf_file;
    idf_file.setHeader("This is the header\n");
    for (int i = 0; i < 1000; ++i) {
       withendl::IdfObject o1("Object1", "blablablablablalbla");
       idf_file.addObject(o1);
    }
    // Code inside this loop is measured repeatedly
    for (auto _ : state) {
      idf_file.save("file.txt");
    }
  }
  // Register the function as a benchmark
  BENCHMARK(BM_OutputEndl);

}


namespace boostofstream {
  namespace withendl {
    class IdfObject {
      public:
        IdfObject(const std::string& t_name, const std::string& t_text)
        : m_name(t_name), m_text(t_text) {}

        /** Serialize this object to os as Idf text. */
        std::ostream& print(std::ostream& os) const {
          os << "IdfObject named " << m_name << ", text=" << m_text << std::endl;
          return os;
        }
      private:
        std::string m_name;
        std::string m_text;
    };

    class IdfFile {
      public:
        IdfFile() { };
        bool setHeader(const std::string& header) { m_header = header; return true; }
        bool addObject(const IdfObject& idfObject) { m_objects.push_back(idfObject); return true; }

        std::ostream& print(std::ostream& os) const {
          if (!m_header.empty()) {
            os << m_header << std::endl;
          }
          os << std::endl;
          for (const IdfObject& object : m_objects) {
            object.print(os);
          }
          return os;
        }

        bool save(boost::filesystem::path wp) const {
          boost::filesystem::ofstream outFile(wp);

          if (outFile) {
            try {
              print(outFile);
              outFile.close();
              return true;
            } catch (...) {
              std::cout << "Unable to write file to path '" << wp.string() << "'.";
              outFile.close();
              return false;
            }
          }
          return false;
        }


      private:
        std::string m_header;
        std::vector<IdfObject> m_objects;

    };

    std::ostream& operator<<(std::ostream& os, const IdfObject& idfObject) {
      return idfObject.print(os);
    }
  }

  namespace withn {
    class IdfObject {
      public:
        IdfObject(const std::string& t_name, const std::string& t_text)
        : m_name(t_name), m_text(t_text) {}

        /** Serialize this object to os as Idf text. */
        std::ostream& print(std::ostream& os) const {
          os << "IdfObject named " << m_name << ", text=" << m_text << '\n';
          return os;
        }
      private:
        std::string m_name;
        std::string m_text;
    };

    class IdfFile {
      public:
        IdfFile() { };
        bool setHeader(const std::string& header) { m_header = header; return true; }
        bool addObject(const IdfObject& idfObject) { m_objects.push_back(idfObject); return true; }

        std::ostream& print(std::ostream& os) const {
          if (!m_header.empty()) {
            os << m_header << '\n';
          }
          os << std::endl;
          for (const IdfObject& object : m_objects) {
            object.print(os);
          }
          return os;
        }

        bool save(boost::filesystem::path wp) const {
          boost::filesystem::ofstream outFile(wp);

          if (outFile) {
            try {
              print(outFile);
              outFile.close();
              return true;
            } catch (...) {
              std::cout << "Unable to write file to path '" << wp.string() << "'.";
              outFile.close();
              return false;
            }
          }
          return false;
        }


      private:
        std::string m_header;
        std::vector<IdfObject> m_objects;

    };

    std::ostream& operator<<(std::ostream& os, const IdfObject& idfObject) {
      return idfObject.print(os);
    }
  }


  static void BM_BoostOutputN(benchmark::State& state) {
    withn::IdfFile idf_file;
    idf_file.setHeader("This is the header\n");
    for (int i = 0; i < 1000; ++i) {
       withn::IdfObject o1("Object1", "blablablablablalbla");
       idf_file.addObject(o1);
    }
    // Code inside this loop is measured repeatedly
    for (auto _ : state) {
      idf_file.save("file.txt");
    }
  }
  // Register the function as a benchmark
  BENCHMARK(BM_BoostOutputN);

  static void BM_BoostOutputEndl(benchmark::State& state) {
    withendl::IdfFile idf_file;
    idf_file.setHeader("This is the header\n");
    for (int i = 0; i < 1000; ++i) {
       withendl::IdfObject o1("Object1", "blablablablablalbla");
       idf_file.addObject(o1);
    }
    // Code inside this loop is measured repeatedly
    for (auto _ : state) {
      idf_file.save("file.txt");
    }
  }
  // Register the function as a benchmark
  BENCHMARK(BM_BoostOutputEndl);

}

// Run the benchmark
BENCHMARK_MAIN();
