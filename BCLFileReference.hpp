#ifndef UTILITIES_BCL_BCLFILEREFERENCE_HPP
#define UTILITIES_BCL_BCLFILEREFERENCE_HPP

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <compare>
#include <string>

namespace fs = boost::filesystem;

namespace openstudio {

/** BCLFileReference is a class for tracking files that come with BCL components and measures.
  **/
class BCLFileReference
{

 public:
  /** @name Constructors */
  //@{

  /// Constructor from file path.
  explicit BCLFileReference(const fs::path& measureRootDir, const fs::path& relativePath);

  friend bool operator==(const BCLFileReference& lhs, const BCLFileReference& rhs);

  friend std::strong_ordering operator<=>(const BCLFileReference& lhs, const BCLFileReference& rhs);

  // friend std::strong_ordering operator<=>(const BCLFileReference& lhs, const BCLFileReference& rhs);
  /// Returns absolute path to file.
  fs::path path() const;

  // Returns path to file, relative to measure root directory (including subdirectory, eg 'docs/subfolder/docs.rb')
  fs::path relativePath() const;

 private:
  fs::path m_measureRootDir;
  fs::path m_path;
  std::string m_checksum;
  std::string m_softwareProgram;
  std::string m_softwareProgramVersion;
  boost::optional<std::string> m_minCompatibleVersion;
  boost::optional<std::string> m_maxCompatibleVersion;
  std::string m_fileName;
  std::string m_fileType;
  std::string m_usageType;
};

}  // namespace openstudio

#endif  // UTILITIES_BCL_BCLFILEREFERENCE_HPP
