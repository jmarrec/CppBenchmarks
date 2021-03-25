// Benchmark for https://github.com/NREL/OpenStudio/issues/4187
#include <benchmark/benchmark.h>
#include <json/json.h>

#include <iostream>
#include <fstream>
// #include <fmt/format.h>
#include <vector>
#include <filesystem>



static void BM_JsonStreamOp(benchmark::State& state) {

  std::filesystem::path p("/home/julien/Software/CppBenchmarks/resources/Sandia_Modules.json");

  if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)){
    std::cout << "Path '" << p.string() << "' is not a valid file";
    exit(1);
  }

  std::ifstream ifs(p);

  Json::Value m_sandiaRoot;
  ifs >> m_sandiaRoot;

switch (m_sandiaRoot.type()) {
        case Json::nullValue: std::cout << "nullValue\n"; break;
        case Json::intValue: std::cout << "intValue\n"; break;
        case Json::uintValue: std::cout << "uintValue\n"; break;
        case Json::realValue: std::cout << "realValue\n"; break;
        case Json::stringValue: std::cout << "stringValue\n"; break;
        case Json::booleanValue: std::cout << "booleanValue\n"; break;
        case Json::arrayValue: std::cout << "arrayValue\n"; break;
        case Json::objectValue: std::cout << "objectValue\n"; break;
        default: std::cout << "wrong type\n"; break;
    }


  //for (auto& m: m_sandiaRoot.getMemberNames()) {
    //std::cout << m << '\n';
  //}

  //std::cout << "\n\n";
  //for (auto& m: m_sandiaRoot["data"].getMemberNames()) {
    //std::cout << m << '\n';
  //}

  Json::Value this_data = m_sandiaRoot["data"]["Solarex MST-43LV [1998]"];
  if (!this_data) {
    throw std::runtime_error("error");
  }
switch (this_data.type()) {
        case Json::nullValue: std::cout << "nullValue\n"; break;
        case Json::intValue: std::cout << "intValue\n"; break;
        case Json::uintValue: std::cout << "uintValue\n"; break;
        case Json::realValue: std::cout << "realValue\n"; break;
        case Json::stringValue: std::cout << "stringValue\n"; break;
        case Json::booleanValue: std::cout << "booleanValue\n"; break;
        case Json::arrayValue: std::cout << "arrayValue\n"; break;
        case Json::objectValue: std::cout << "objectValue\n"; break;
        default: std::cout << "wrong type\n"; break;
    }
  std::cout << this_data << '\n';
  std::map<std::string, double> stdmap;
  for (auto &k: this_data) {
    std::cout << k;
    // stdmap.insert(std::make_pair(kv.first, kv.second));
  }
  for (Json::ValueConstIterator it = this_data.begin(); it != this_data.end(); ++it) {
    std::cout << it.name() << ", " << it->asDouble() << "\n";
    stdmap.insert(std::make_pair(it.name(), it->asDouble()));
  }

  // std::cout << m_sandiaRoot << '\n';

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
      // open file
      std::ifstream ifs(p);

      Json::Value m_sandiaRoot;
      ifs >> m_sandiaRoot;
  }
}



static void BM_JsonCharBuilder(benchmark::State& state) {

  std::filesystem::path p("/home/julien/Software/CppBenchmarks/resources/Sandia_Modules.json");

  if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)){
    std::cout << "Path '" << p.string() << "' is not a valid file";
    exit(1);
  }

  // open file
  std::ifstream ifs(p);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string formattedErrors;

    // open file
    std::ifstream ifs(p);

    Json::Value m_sandiaRoot;

    bool parsingSuccessful = Json::parseFromStream(rbuilder, ifs, &m_sandiaRoot, &formattedErrors);
    if (!parsingSuccessful){
      std::cout << "JSON file at '" << p.string() << "' cannot be processed: " << formattedErrors << '\n';
    }
  }

}

BENCHMARK(BM_JsonStreamOp);
BENCHMARK(BM_JsonCharBuilder);

