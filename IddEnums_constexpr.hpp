#ifndef UTILITIES_IDD_IDDENUMS_CONSTEXPR_HXX
#define UTILITIES_IDD_IDDENUMS_CONSTEXPR_HXX

#include <compare>
#include <iostream>
#include <numeric>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

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

inline std::string ascii_to_upper_copy(std::string_view input) {
  std::string result{input};
  constexpr auto to_upper_diff = 'A' - 'a';
  for (auto& c : result) {
    if (c >= 'a' && c <= 'z') {
      c += to_upper_diff;
    }
  }
  return result;
}

/** Base class for OPENSTUDIO_ENUMs. Comparison operators use the underlying
 * (integer) value. */
template <typename Enum>
class EnumBase
{
 public:
  /** Default constructor. EnumBase default constructor never actually called;
     * OPENSTUDIO_ENUM- generated classes have default constructors that set the
     * value to the first item in the enumerated list. */
  EnumBase() = delete;

  /** Construct from t_value. Throws std::runtime_error if t_value is not in
     * the domain. */
  constexpr EnumBase(int t_value) : m_value(lookupValue(t_value)) {}

  /** Construct from t_value. Throws std::runtime_error if t_value does not
     * match a name or a description (case insensitive). */
  EnumBase(std::string_view t_value) : m_value(lookupValue(t_value)) {}

  /** Returns the name associated with t_value, if it exists. Otherwise,
     * throws std::runtime_error. */
  static constexpr std::string_view valueName(int t_value) {
    t_value = lookupValue(t_value);
    return Enum::names[static_cast<size_t>(t_value)];
  }

  /** Returns the description associated with t_value, if it exists.
     * Otherwise, throws std::runtime_error. */
  static constexpr std::string_view valueDescription(int t_value) {
    t_value = lookupValue(t_value);
    auto& desc = Enum::descriptions[static_cast<size_t>(t_value)];
    return desc.empty() ? valueName(t_value) : desc;
  }

  /** Returns the set of all values in this enum's domain. */
  static constexpr auto getValues() {
    std::array<int, Enum::Size> values;
    std::iota(values.begin(), values.end(), Enum::SizeType(0));
    return values;
  }

  /** Returns this instance's current value (as an integer). */
  constexpr int value() const {
    return m_value;
  }

  /** Returns the name associated with this instance's current value. */
  constexpr std::string_view valueName() const {
    return valueName(m_value);
  }

  /** Returns the description associated with this instance's current value.
     */
  constexpr std::string_view valueDescription() const {
    return valueDescription(m_value);
  }

  /** Set this instance's value to t_value. Throws std::runtime_error if
     *  t_value is not in the domain. */
  constexpr void setValue(int t_value) {
    m_value = lookupValue(t_value);
  }

#if 1
  constexpr auto operator<=>(const EnumBase<Enum>&) const = default;
#else
  constexpr bool operator==(const EnumBase<Enum>& other) const {
    return (m_value == other.m_value);
  }
  constexpr bool operator!=(const EnumBase<Enum>& other) const {
    return (m_value != other.m_value);
  }
  constexpr bool operator>(const EnumBase<Enum>& other) const {
    return (m_value > other.m_value);
  }
  constexpr bool operator>=(const EnumBase<Enum>& other) const {
    return (m_value >= other.m_value);
  }
  constexpr bool operator<(const EnumBase<Enum>& other) const {
    return (m_value < other.m_value);
  }
  constexpr bool operator<=(const EnumBase<Enum>& other) const {
    return (m_value <= other.m_value);
  }
#endif

  /** Returns the (integer) value associated with t_name, as determined by
     * case-insensitive comparison to the enumerated names and descriptions. */
  static int lookupValue(std::string_view t_name) {
    auto nameUC = ascii_to_upper_copy(t_name);  // TODO: make this constexpr
    auto itNames = std::find(Enum::namesUC.cbegin(), Enum::namesUC.cend(), nameUC);
    if (itNames != Enum::namesUC.cend()) {
      return std::distance(Enum::namesUC.cbegin(), itNames);
    }
    auto itDescs = std::find(Enum::descriptionsUC.cbegin(), Enum::descriptionsUC.cend(), nameUC);
    if (itDescs != Enum::descriptionsUC.cend()) {
      return std::distance(Enum::descriptionsUC.cbegin(), itDescs);
    }

    throw std::runtime_error(fmt::format("Unknown OpenStudio Enum Value '{}' for Enum {}", t_name, Enum::enumName()));
  }

  /** Returns t_value if it is in the domain. Otherwise throws
     * std::runtime_error. */
  static constexpr int lookupValue(int t_value) {
    if (t_value < Enum::Size) {
      return t_value;
    } else {
      throw std::runtime_error(fmt::format("Unknown OpenStudio Enum Value = {} for Enum {}", t_value, Enum::enumName()));
    }
  }

 protected:
  int m_value;
};

struct IddObjectType : public ::EnumBase<IddObjectType>
{
 public:
  using SizeType = int;  // Or uint16_t? This provides 65535 possible keys which is more than enough
  static constexpr SizeType Size = 8;

  enum domain : SizeType;

  constexpr IddObjectType() : EnumBase<IddObjectType>(0) {}
  IddObjectType(const std::string& t_name) : EnumBase<IddObjectType>(t_name) {}
  constexpr IddObjectType(int t_value) : EnumBase<IddObjectType>(t_value) {}

  static std::string_view enumName() {
    return "IddObjectType";
  }

  constexpr domain value() const {
    return static_cast<domain>(EnumBase<IddObjectType>::value());
  }

 protected:
  friend class EnumBase<IddObjectType>;

  static constexpr std::array<std::string_view, Size> names{"Catchall",       "UserCustom",        "LeadInput", "EndLeadInput",
                                                            "SimulationData", "EndSimulationData", "Version",   "CommentOnly"};
  static constexpr std::array<std::string_view, Size> namesUC{"CATCHALL",       "USERCUSTOM",        "LEADINPUT", "ENDLEADINPUT",
                                                              "SIMULATIONDATA", "ENDSIMULATIONDATA", "VERSION",   "COMMENTONLY"};
  static constexpr std::array<std::string_view, Size> descriptions{
    "", "", "Lead Input", "End Lead Input", "Simulation Data", "End Simulation Data", "Version", ""};
  static constexpr std::array<std::string_view, Size> descriptionsUC{
    "", "", "LEAD INPUT", "END LEAD INPUT", "SIMULATION DATA", "END SIMULATION DATA", "VERSION", ""};
};

inline std::ostream& operator<<(std::ostream& os, const IddObjectType& e) {
  return os << e.valueName() << "(" << e.value() << ")";
}

/** \class IddObjectType */
enum IddObjectType::domain : IddObjectType::SizeType
{
  Catchall,
  UserCustom,
  LeadInput,
  EndLeadInput,
  SimulationData,
  EndSimulationData,
  Version,
  CommentOnly,
};

#endif  // UTILITIES_IDD_IDDENUMS_CONSTEXPR_HXX
