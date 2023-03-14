#include <gtest/gtest.h>
#include "TestFixture.hpp"

#include "../BCLFileReference.hpp"

#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <set>
#include <vector>

namespace fs = boost::filesystem;

TEST_F(TestFixture, TestOrdering) {

  openstudio::BCLFileReference fileRef1(fs::path("measure1"), fs::path("docs/test.rb"));
  openstudio::BCLFileReference fileRef2(fs::path("measure1"), fs::path("measure.rb"));

  EXPECT_TRUE(fileRef1 < fileRef2);
  EXPECT_TRUE(fileRef1 <= fileRef2);
  EXPECT_TRUE(fileRef1 != fileRef2);
  EXPECT_FALSE(fileRef1 == fileRef2);
  EXPECT_FALSE(fileRef1 > fileRef2);
  EXPECT_FALSE(fileRef1 >= fileRef2);
}

TEST_F(TestFixture, TestOther) {

  fs::path path1("docs/test.rb");
  fs::path path2("measure.xml");
  fmt::print("{}\n", (path1 < path2));
  //fmt::print("{}\n", (path1 == path2));
  //fmt::print("{}\n", (path1 > path2));

  std::vector<openstudio::BCLFileReference> m_files;
  m_files.emplace_back(fs::path("measure1"), fs::path("docs/test.rb"));
  m_files.emplace_back(fs::path("measure1"), fs::path("measure.xml"));
  m_files.emplace_back(fs::path("measure1"), fs::path("measure.rb"));
  m_files.emplace_back(fs::path("measure1"), fs::path("README.md.erb"));
  m_files.emplace_back(fs::path("measure1"), fs::path("README.md"));
  m_files.emplace_back(fs::path("measure1"), fs::path("resources/file.csv"));

  // std::sort(m_files.begin(), m_files.end(),
  //          [](auto& lhs, auto& rhs) { return lhs.path() < rhs.path(); });

  std::sort(m_files.begin(), m_files.end());

  for (auto& fileRef : m_files) {
    fmt::print("{}\n", fileRef.path().generic_string());
  }
}

TEST_F(TestFixture, TestSet) {

  std::set<openstudio::BCLFileReference> m_files;
  m_files.emplace(fs::path("measure1"), fs::path("docs/test.rb"));
  m_files.emplace(fs::path("measure1"), fs::path("measure.xml"));
  m_files.emplace(fs::path("measure1"), fs::path("measure.rb"));
  m_files.emplace(fs::path("measure1"), fs::path("README.md.erb"));
  m_files.emplace(fs::path("measure1"), fs::path("README.md"));
  m_files.emplace(fs::path("measure1"), fs::path("resources/file.csv"));

  // std::sort(m_files.begin(), m_files.end(),
  //          [](auto& lhs, auto& rhs) { return lhs.path() < rhs.path(); });

  for (auto& fileRef : m_files) {
    fmt::print("{}\n", fileRef.path().generic_string());
  }
}
