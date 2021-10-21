#include <benchmark/benchmark.h>
#include <pugixml.hpp>

#include <stdexcept>

std::string byVal(pugi::xml_node node) {
  return node.name();
}

std::string byRef(const pugi::xml_node& node) {
  return node.name();
}

static void BM_ByVal(benchmark::State& state) {

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string("<mesh name='sphere'><bounds>0 0 1 1</bounds></mesh><node id='123'>text</node><!-- comment -->",
                                                  pugi::parse_default | pugi::parse_comments);
  if (!result) {
    throw std::runtime_error("Failed to parse");
  }
  // Get a node, modify its name
  pugi::xml_node node = doc.child("node");

  for (auto _ : state) {
    std::string name = byVal(node);
    benchmark::DoNotOptimize(name);
  }
}

static void BM_ByRef(benchmark::State& state) {

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string("<mesh name='sphere'><bounds>0 0 1 1</bounds></mesh><node id='123'>text</node><!-- comment -->",
                                                  pugi::parse_default | pugi::parse_comments);
  if (!result) {
    throw std::runtime_error("Failed to parse");
  }
  // Get a node, modify its name
  pugi::xml_node node = doc.child("node");

  for (auto _ : state) {
    std::string name = byRef(node);
    benchmark::DoNotOptimize(name);
  }
}

BENCHMARK(BM_ByVal);
BENCHMARK(BM_ByRef);
