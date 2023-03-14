#include "BCLFileReference.hpp"

namespace openstudio {

BCLFileReference::BCLFileReference(const fs::path& measureRootDir, const fs::path& relativePath)
  : m_measureRootDir(fs::system_complete(measureRootDir)), m_path(fs::system_complete(measureRootDir / relativePath)) {}

fs::path BCLFileReference::path() const {
  return m_path;
}

fs::path BCLFileReference::relativePath() const {
  return fs::relative(m_path, m_measureRootDir);
}

}  // namespace openstudio
