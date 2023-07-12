#include <benchmark/benchmark.h>

#include <string>
#include <string_view>

#include <fmt/format.h>
#include <fmt/color.h>

#include <boost/regex.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

constexpr auto getIddExample() {
  return R"(!IDD_Version 3.2.0
! *****************************************************************************
! HEADER. BLABLABLA
! **************************************************************************

\group OpenStudio Core

OS:Version,
       \unique-object
       \required-object
       \format singleLine
       \min-fields 1
  A1, \field Handle
       \type handle
       \required-field
  A2, \field Version Identifier
       \type alpha
       \default 3.2.0
  A3; \field Prerelease Identifier
       \type alpha

! A comment << this comment is ignored

\group Another Group

OS:ComponentData,
       \memo Defines the meta-data and contents of a Component, that is, a
       \memo subset of an OpenStudio Model.
       \extensible:1
       \min-fields 7
       ! Comment
  A1, \field Handle
       \type handle
       \required-field
! A1, \field Handle  << this comment is ignored
  A2; \field Name
       \note This should be the name of the Component as listed in the
       \note Component Library, or something else.
       \type alpha
       \required-field
       \reference ComponentNames

OS:ConvergenceLimits,
       \unique-object
       \min-fields 1
  A1, \field Handle
       \type handle
       \required-field
  N1; \field Minimum System Timestep
       \note 0 sets the minimum to the zone timestep (ref: Timestep)
       \note 1 is normal (ratchet down to 1 minute)
       \note setting greater than zone timestep (in minutes) will effectively set to zone timestep
       \type integer
       \units minutes
       \minimum 0
       \maximum 60

)";
}

namespace openstudio {

inline std::string ascii_to_lower_copy(std::string_view input) {
  std::string result{input};
  constexpr auto to_lower_diff = 'a' - 'A';
  for (auto& c : result) {
    if (c >= 'A' && c <= 'Z') {
      c += to_lower_diff;
    }
  }
  return result;
}

inline std::string_view ascii_trim_left(std::string_view s) {
  return s.substr(std::min(s.find_first_not_of(" \f\n\r\t\v"), s.size()));
}

inline std::string_view ascii_trim_right(std::string_view s) {
  return s.substr(0, std::min(s.find_last_not_of(" \f\n\r\t\v") + 1, s.size()));
}

inline std::string_view ascii_trim(std::string_view s) {
  return ascii_trim_left(ascii_trim_right(s));
}

inline void ascii_trim(std::string& str) {
  str = std::string(ascii_trim(std::string_view(str)));
}

}  // namespace openstudio

namespace iddRegex {
const boost::regex& commentOnlyLine() {
  static const boost::regex result("^[\\s\\t]*[!](.*)");
  return result;
}

/// Match group identifier
/// matches[1], group name
const boost::regex& group() {
  static const boost::regex result("^[\\\\][gG]roup[\\s\\t]*(.*)");
  return result;
}

const boost::regex& line() {
  static const boost::regex result("^([^!]*?)[,;](.*)");
  return result;
}

/// Match the closing field in an idd object
/// matches[1], all previous text
/// matches[2], the last field
const boost::regex& closingField() {
  static const boost::regex result("(.*)([AN][0-9]+[\\s\\t]*[;].*?)$");
  return result;
}

/// Match a field or object level comment
/// matches[1], after '\' until next '\'
/// matches[2], after second '\' (may be empty)
const boost::regex& metaDataComment() {
  static const boost::regex result("^[\\s\\t]*?[\\\\]([^\\\\]*)(.*)");
  return result;
}

}  // namespace iddRegex

static void BM_Trim_Boost(benchmark::State& state) {
  for (auto _ : state) {
    std::stringstream is(getIddExample());
    std::string line;
    while (getline(is, line)) {
      boost::trim(line);
    }
  }
}

static void BM_Trim_ASCII(benchmark::State& state) {
  for (auto _ : state) {
    std::stringstream is(getIddExample());
    std::string line;
    while (getline(is, line)) {
      openstudio::ascii_trim(line);
    }
  }
}

// Register the function as a benchmark
BENCHMARK(BM_Trim_Boost);
BENCHMARK(BM_Trim_ASCII);

void parseOld(std::istream& is) {

  std::string line;

  // this will contain matches to regular expressions
  boost::smatch matches;

  [[maybe_unused]] int nCommentOnlys = 0;
  [[maybe_unused]] int nGroups = 0;
  [[maybe_unused]] int nObjects = 0;

  [[maybe_unused]] bool headerClosed = false;
  std::string objectName;

  while (getline(is, line)) {
    openstudio::ascii_trim(line);
    if (line.empty()) {
      headerClosed = true;
      continue;
    }
    if (boost::regex_match(line, iddRegex::commentOnlyLine())) {
      ++nCommentOnlys;
      continue;
    }
    if (boost::regex_search(line, matches, iddRegex::group())) {
      headerClosed = true;
      std::string groupName(matches[1].first, matches[1].second);
      ++nGroups;
      continue;
    }

    headerClosed = true;

    // fmt::print("none of the above, , headerClosed={}\n", headerClosed);

    //int beginLineNum(lineNum);
    bool foundClosingLine(false);

    if (boost::regex_search(line, matches, iddRegex::line())) {
      objectName = std::string(matches[1].first, matches[1].second);
      openstudio::ascii_trim(objectName);
      ++nObjects;
    }

    // continue reading until we have seen the entire object
    // last line will be thrown away, requires empty line between objects in idd
    while (getline(is, line)) {
      // fmt::print(fmt::fg(fmt::color::dark_gray), ">>> {}, {}\n", lineNum, line);

      // remove whitespace
      openstudio::ascii_trim(line);

      // found lat field and this is not a field comment
      if (foundClosingLine && (!boost::regex_match(line, iddRegex::metaDataComment()))) {
        // fmt::print(fmt::fg(fmt::color::red), "!metaDataComment : BREAK");
        break;
      }

      if (!line.empty()) {
        // if the line is not empty add it to the text
        // note, text does not include newlines

        // check if we have found the last field
        if (boost::regex_match(line, iddRegex::closingField())) {
          foundClosingLine = true;
          // fmt::print(fmt::fg(fmt::color::lime_green), "closingField!\n");
        }
      }
    }
  }
  BOOST_ASSERT(nCommentOnlys == 5);
  BOOST_ASSERT(nGroups == 2);
  BOOST_ASSERT(nObjects == 3);
}

void parseNew(std::istream& is) {
  std::string line;

  // this will contain matches to regular expressions
  boost::smatch matches;

  [[maybe_unused]] int nCommentOnlys = 0;
  [[maybe_unused]] int nGroups = 0;
  [[maybe_unused]] int nObjects = 0;

  [[maybe_unused]] bool headerClosed = false;
  std::string objectName;

  while (getline(is, line)) {
    openstudio::ascii_trim(line);
    if (line.empty()) {
      continue;
    }
    if (line[0] == '!') {
      ++nCommentOnlys;
      continue;
    }
    if (line.rfind("\\group ") == 0) {
      std::string groupName(line.substr(7));
      openstudio::ascii_trim(groupName);
      ++nGroups;
      continue;
    }

    headerClosed = true;

    // fmt::print("none of the above, , headerClosed={}\n", headerClosed);

    //int beginLineNum(lineNum);
    bool foundClosingLine(false);

    // Throw away everything after the first '!' if any
    if (auto pos = line.find('!'); pos != std::string::npos) {
      line = line.substr(0, pos);
    }

    // if (boost::regex_search(line, matches, iddRegex::line())) {
    if (auto pos = line.find_first_of(",;"); pos != std::string::npos) {
      objectName = line.substr(0, pos);
      openstudio::ascii_trim(objectName);
      ++nObjects;
    } else {
      fmt::print(fmt::fg(fmt::color::red), "ERROR: CANNOT PARSE objectName in line '{}'", line);
    }

    // continue reading until we have seen the entire object
    // last line will be thrown away, requires empty line between objects in idd
    while (getline(is, line)) {
      // fmt::print(fmt::fg(fmt::color::dark_gray), ">>> {}, {}\n", lineNum, line);

      // remove whitespace
      openstudio::ascii_trim(line);

      // found last field and this is not a field comment
      if (foundClosingLine && (line[0] != '\\')) {
        // fmt::print(fmt::fg(fmt::color::red), "!metaDataComment : BREAK");
        break;
      }

      if (!line.empty()) {
        // if the line is not empty add it to the text
        // note, text does not include newlines

        // check if we have found the last field
        int pos = 0;
        for (auto& c : line) {
          if (pos == 0) {
            if (!((c == 'A') || (c == 'N'))) {
              break;
            }
          } else {
            if (c == ';') {
              foundClosingLine = true;
              break;
            } else if (c < '0' || c > '9') {
              break;
            }
          }
        }
      }
    }
  }
  BOOST_ASSERT(nCommentOnlys == 5);
  BOOST_ASSERT(nGroups == 2);
  BOOST_ASSERT(nObjects == 3);
}

static void BM_CommentOnly_Current(benchmark::State& state) {
  for (auto _ : state) {
    std::stringstream is(getIddExample());
    parseOld(is);
  }
}

static void BM_CommentOnly_New(benchmark::State& state) {
  for (auto _ : state) {
    std::stringstream is(getIddExample());
    parseNew(is);
  }
}

BENCHMARK(BM_CommentOnly_Current);
BENCHMARK(BM_CommentOnly_New);

// Run the benchmark
BENCHMARK_MAIN();

/*
int main() {

  std::stringstream is(getIddExample());
  std::string line;
  while (getline(is, line)) {
    openstudio::ascii_trim(line);
  }
}
*/
