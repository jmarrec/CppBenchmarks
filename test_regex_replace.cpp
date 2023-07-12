#include <boost/regex/v5/match_flags.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>
#include <ranges>

std::string m_convertName(const std::string& originalName) {
  std::string result(originalName);
  boost::trim(result);
  result = boost::regex_replace(result, boost::regex("^100 ?%"), "All");
  result = boost::regex_replace(result, boost::regex("[ \\-]+"), "\\u");
  result = boost::regex_replace(result, boost::regex("[:/,\\(\\)%]+"), "_");
  result = boost::regex_replace(result, boost::regex("[\\.\\?\\]\\[]"), "");
  result = boost::regex_replace(result, boost::regex("="), "_EQUAL_");
  result = boost::regex_replace(result, boost::regex("\\*\\*"), "_POW_");
  result = boost::regex_replace(result, boost::regex("\\+"), "_PLUS_");
  result = boost::regex_replace(result, boost::regex("\\*"), "_TIMES_");
  return result;
}

int main() {

  boost::regex find("\\s?[0-9]+");
  std::string replace;

  std::string extensibleFieldName("Speed 1 Rated Evaporator Fan Power Per Volume Flow Rate 2017");
  fmt::print("{}\n", extensibleFieldName);

  fmt::print("m_convertName={}\n", m_convertName(extensibleFieldName));

  extensibleFieldName = boost::regex_replace(extensibleFieldName, find, replace, boost::format_first_only);
  fmt::print("{}\n", extensibleFieldName);

  std::string fieldName = "2023 Speed 1 Rated Evaporator Fan Power Per Volume Flow Rate";
  std::string fieldName2 = boost::regex_replace(fieldName, boost::regex("\\s?[0-9]+"), "", boost::format_first_only);

  fmt::print("{} => {}\n", fieldName, fieldName2);

  std::string fieldName3 = boost::regex_replace(fieldName, boost::regex("\\s?(?<![0-9])[0-9]{1,2}(?![0-9])"), "", boost::format_first_only);

  fmt::print("{} => {}\n", fieldName, fieldName3);

  boost::match_results<std::string::const_iterator> what;
  boost::match_flag_type flags = boost::match_default;
  auto start = fieldName.cbegin();
  auto end = fieldName.cend();
  std::vector<std::pair<size_t, size_t>> erasePos;
  while (boost::regex_search(start, end, what, boost::regex("\\s?[0-9]+"), flags)) {
    fmt::print("searching {}\n", std::string(start, end));
    fmt::print("matches.size()={}\n", what.size());
    std::string s(what[0].first, what[0].second);
    fmt::print("std::string(what[0].first, what[0].second)={}\n", s);
    // if (s.size() <= 2) {
    fmt::print("what[0].second - what[0].first = {}\n, ", what[0].second - what[0].first);
    fmt::print("what[0].first - fieldName.cbegin() = {}\n, ", what[0].first - fieldName.cbegin());
    erasePos.emplace_back(what[0].first - fieldName.cbegin(), what[0].second - what[0].first);
    // }
    // update search position
    start = what[0].second;
    // update flags:
    flags |= boost::match_prev_avail;
    flags |= boost::match_not_bob;
  }

  for (auto& [pos, len] : std::ranges::views::reverse(erasePos)) {
    fmt::print("{}, len {}\n", pos, len);
    fieldName.erase(pos, len);
  }

  fmt::print("'{}'\n", fieldName);
}
