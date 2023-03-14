#include "BCLFileReference.hpp"

namespace openstudio {

BCLFileReference::BCLFileReference(const fs::path& measureRootDir, const fs::path& relativePath)
  : m_measureRootDir(fs::system_complete(measureRootDir)), m_path(fs::system_complete(measureRootDir / relativePath)) {}

//bool BCLFileReference::operator<(const BCLFileReference& other) const {
//  return m_path < other.m_path;
//}

// bool operator<(const openstudio::BCLFileReference& lhs, const openstudio::BCLFileReference& rhs) {
//   return lhs.m_path < rhs.m_path;
// }

// std::strong_ordering operator<=>(const BCLFileReference& lhs, const BCLFileReference& rhs) {
//
//   if (lhs.m_path < rhs.m_path) {
//     return std::strong_ordering::less;
//   } else if (lhs.m_path == rhs.m_path) {
//     return std::strong_ordering::equal;
//   }
//   return std::strong_ordering::greater;
// }

fs::path BCLFileReference::path() const {
  return m_path;
}

fs::path BCLFileReference::relativePath() const {
  return fs::relative(m_path, m_measureRootDir);
}

}  // namespace openstudio
