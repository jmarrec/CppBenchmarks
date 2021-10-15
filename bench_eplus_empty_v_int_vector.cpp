#include <benchmark/benchmark.h>

#include <numeric>
#include <string>
#include <vector>

typedef double Real64;  // Platform-specific: C++ has no defined precision floating point types

template <typename T>
struct EPVector : private std::vector<T>
{
  using std::vector<T>::size;
  using std::vector<T>::operator[];
  using std::vector<T>::empty;
  using std::vector<T>::begin;
  using std::vector<T>::end;
  using std::vector<T>::cbegin;
  using std::vector<T>::cend;
  using std::vector<T>::clear;
  using std::vector<T>::emplace_back;
  using std::vector<T>::push_back;
  using const_iterator = typename std::vector<T>::const_iterator;

  using value_type = T;
  using size_type = typename std::vector<T>::size_type;

  [[nodiscard]] T& operator()(std::size_t n) {
    return (*this)[n - 1];
  }

  [[nodiscard]] const T& operator()(std::size_t n) const {
    return (*this)[n - 1];
  }

  void allocate(std::size_t size) {
    m_allocated = true;
    this->resize(size);
    std::fill(begin(), end(), T{});
  }

  void deallocate() noexcept {
    m_allocated = false;
    this->clear();
  }

  [[nodiscard]] bool allocated() const noexcept {
    return m_allocated || !this->empty();
  }

  // operator= used for initialization of the vector
  void operator=(const T& v) {
    std::fill(this->begin(), this->end(), v);
  }

  // dimension is often used to initalize the vector instead of allocate + operator=
  void dimension(std::size_t size, const T& v) {
    this->clear();
    this->resize(size, v);
  }

  // isize needed for current FindItemInList
  [[nodiscard]] int isize() const noexcept {
    return static_cast<int>(this->size());
  }

 private:
  bool m_allocated{false};
};

namespace DataSizing {

struct OARequirementsData
{
  // Holds complete data for a single DesignSpecification:OutdoorAir object or
  // a list of indexes from a DesignSpecification:OutdoorAir:SpaceList object
  std::string Name;                      // Name of DesignSpecification:OutdoorAir or DesignSpecification:OutdoorAir:SpaceList object
  int numDSOA = 0;                       // Number of DesignSpecification:OutdoorAir objects for this instance (zero if not a list)
  EPVector<int> dsoaIndexes;             // Indexes to DesignSpecification:OutdoorAir objects (if this is a DSOA:SpaceList object)
  EPVector<std::string> dsoaSpaceNames;  // Names of spaces if this is a (if this is a DSOA:SpaceList object)
  EPVector<int> dsoaSpaceIndexes;        // Indexes to Spaces (if this is a DSOA:SpaceList object)
  int OAFlowMethod = 0;                  // - Method for OA flow calculation (Flow/Person, Flow/Zone, Flow/Area, FlowACH, Sum, Maximum)
  Real64 OAFlowPerPerson = 0.0;          // - OA requirement per person
  Real64 OAFlowPerArea = 0.0;            // - OA requirement per zone area
  Real64 OAFlowPerZone = 0.0;            // - OA requirement per zone
  Real64 OAFlowACH = 0.0;                // - OA requirement per zone per hour
  int OAFlowFracSchPtr = -1;             // - Fraction schedule applied to total OA requirement
  int OAPropCtlMinRateSchPtr = -1;       // - Fraction schedule applied to Proportional Control Minimum Outdoor Air Flow Rate
  int CO2MaxMinLimitErrorCount = 0;      // Counter when max CO2 concentration < min CO2 concentration for SOAM_ProportionalControlSchOcc
  int CO2MaxMinLimitErrorIndex = 0;      // Index for max CO2 concentration < min CO2 concentration recurring error message for
                                         // SOAM_ProportionalControlSchOcc
  int CO2GainErrorCount = 0;             // Counter when CO2 generation from people is zero for SOAM_ProportionalControlSchOcc
  int CO2GainErrorIndex = 0;  // Index for recurring error message when CO2 generation from people is zero for SOAM_ProportionalControlSchOcc
  bool myEnvrnFlag = true;
};
}  // namespace DataSizing

static void BM_withInt(benchmark::State& state) {

  EPVector<DataSizing::OARequirementsData> OARequirements;
  int numOARequirements = state.range(0);
  int numDSOAIndexes = state.range(1);
  OARequirements.allocate(numOARequirements);

  // Code inside this loop is measured repeteadly
  for (auto _ : state) {

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      auto& thisOAReq = OARequirements(OAIndex);
      thisOAReq.Name = "OARequirements " + std::to_string(OAIndex);
      for (auto i = 0; i < numDSOAIndexes; ++i) {
        ++thisOAReq.numDSOA;
        thisOAReq.dsoaSpaceIndexes.emplace_back(1);
      }
    }

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      bool isSpaceDSOAList = (OARequirements(OAIndex).numDSOA != 0);
      benchmark::DoNotOptimize(isSpaceDSOAList);
    }
  }  // end state

  state.SetComplexityN(state.range(0));
}

static void BM_withEmpty(benchmark::State& state) {

  EPVector<DataSizing::OARequirementsData> OARequirements;
  int numOARequirements = state.range(0);
  int numDSOAIndexes = state.range(1);
  OARequirements.allocate(numOARequirements);

  // Code inside this loop is measured repeteadly
  for (auto _ : state) {

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      auto& thisOAReq = OARequirements(OAIndex);
      thisOAReq.Name = "OARequirements " + std::to_string(OAIndex);
      for (auto i = 0; i < numDSOAIndexes; ++i) {
        thisOAReq.dsoaSpaceIndexes.emplace_back(1);
      }
    }

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      bool isSpaceDSOAList = (!OARequirements(OAIndex).dsoaIndexes.empty());
      benchmark::DoNotOptimize(isSpaceDSOAList);
    }
  }  // end state

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_withInt)->Ranges({{1, 512}, {1, 512}})->Complexity();
BENCHMARK(BM_withEmpty)->Ranges({{1, 512}, {1, 512}})->Complexity();

static void BM_withInt_CheckOnly(benchmark::State& state) {

  EPVector<DataSizing::OARequirementsData> OARequirements;
  int numOARequirements = state.range(0);
  int numDSOAIndexes = state.range(1);
  OARequirements.allocate(numOARequirements);

  for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
    auto& thisOAReq = OARequirements(OAIndex);
    thisOAReq.Name = "OARequirements " + std::to_string(OAIndex);
    for (auto i = 0; i < numDSOAIndexes; ++i) {
      ++thisOAReq.numDSOA;
      thisOAReq.dsoaSpaceIndexes.emplace_back(1);
    }
  }

  // Code inside this loop is measured repeteadly
  for (auto _ : state) {

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      bool isSpaceDSOAList = (OARequirements(OAIndex).numDSOA != 0);
      benchmark::DoNotOptimize(isSpaceDSOAList);
    }
  }  // end state

  state.SetComplexityN(state.range(0));
}

static void BM_withEmpty_CheckOnly(benchmark::State& state) {

  EPVector<DataSizing::OARequirementsData> OARequirements;
  int numOARequirements = state.range(0);
  int numDSOAIndexes = state.range(1);
  OARequirements.allocate(numOARequirements);

  for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
    auto& thisOAReq = OARequirements(OAIndex);
    thisOAReq.Name = "OARequirements " + std::to_string(OAIndex);
    for (auto i = 0; i < numDSOAIndexes; ++i) {
      thisOAReq.dsoaSpaceIndexes.emplace_back(1);
    }
  }

  // Code inside this loop is measured repeteadly
  for (auto _ : state) {

    for (int OAIndex = 1; OAIndex <= numOARequirements; ++OAIndex) {
      bool isSpaceDSOAList = (!OARequirements(OAIndex).dsoaIndexes.empty());
      benchmark::DoNotOptimize(isSpaceDSOAList);
    }
  }  // end state

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_withInt_CheckOnly)->Ranges({{1, 512}, {1, 512}})->Complexity();
BENCHMARK(BM_withEmpty_CheckOnly)->Ranges({{1, 512}, {1, 512}})->Complexity();
