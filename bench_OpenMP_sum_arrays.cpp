#include <benchmark/benchmark.h>

#ifdef _OPENMP
#  include <omp.h>
#endif

#include <memory>
#include <string>
#include <vector>

using Real64 = double;

struct HeatBalFanSysData
{
  std::vector<double> QRadSurfAFNDuct;
  std::vector<double> PoolHeatTransCoefs;
};

struct HeatBalSurfData
{
  std::vector<double> SurfCTFInside0;
  std::vector<double> SurfIsAdiabatic;
  std::vector<double> SurfCTFCross0;
  std::vector<double> SurfIsOperatingPool;
  std::vector<double> SurfTempTerm;
  std::vector<double> SurfTempDiv;
  std::vector<double> SurfCTFConstInPart;
  std::vector<double> SurfOpaqQRadSWInAbs;
  std::vector<double> SurfQAdditionalHeatSourceInside;
  std::vector<double> SurfHConvInt;
  std::vector<double> SurfQdotRadHVACInPerArea;
};

struct HeatBalSurfMgr
{
  std::vector<double> RefAirTemp;
};

struct SurfaceData
{
  std::string Name;
  int RepresentativeCalcSurfNum;  // Index of the surface that is used to calculate the heat balance for this surface. Equal to this surfaces
                                  // index when not using representative surface calculations
  int spaceNum;                   // Space the surface is part of
  int Zone;                       // Zone the surface is a part of
};

struct SurfacesData
{
  bool UseRepresentativeSurfaceCalculations = false;
  int TotSurfaces = 0;
  std::vector<SurfaceData> Surface;
};

struct ZoneData
{
  std::vector<int> spaceIndexes;
};

struct SpaceData
{
  int OpaqOrIntMassSurfaceFirst;
  int OpaqOrIntMassSurfaceLast;
};

struct HeatBalanceData
{
  std::vector<double> SurfQdotRadIntGainsInPerArea;
  std::vector<ZoneData> Zone;
  std::vector<SpaceData> space;
};

struct DataGlobal
{
  int NumOfZones = 0;
  double TimeStepZoneSec = 15 * 60;  // 4 Timesteps per hour, 15 minutes each
};

struct EnergyPlusData
{
  std::unique_ptr<DataGlobal> dataGlobal;
  std::unique_ptr<HeatBalanceData> dataHeatBal;
  std::unique_ptr<HeatBalFanSysData> dataHeatBalFanSys;
  std::unique_ptr<HeatBalSurfData> dataHeatBalSurf;
  std::unique_ptr<HeatBalSurfMgr> dataHeatBalSurfMgr;
  std::unique_ptr<SurfacesData> dataSurface;

  EnergyPlusData() {
    this->dataGlobal = std::make_unique<DataGlobal>();
    this->dataHeatBal = std::make_unique<HeatBalanceData>();
    this->dataHeatBalFanSys = std::make_unique<HeatBalFanSysData>();
    this->dataHeatBalSurf = std::make_unique<HeatBalSurfData>();
    this->dataHeatBalSurfMgr = std::make_unique<HeatBalSurfMgr>();
    this->dataSurface = std::make_unique<SurfacesData>();
  }
};

namespace DataHeatBalSurface {
Real64 constexpr IterDampConst(5.0);
}

EnergyPlusData create_test_state(int totSurfaces) {
  EnergyPlusData state;
  // 10 zones with 2 spaces each; surfaces per space scales with totSurfaces
  int NumZones = 10;
  int NumSpaces = NumZones * 2;
  int SurfacesPerSpace = totSurfaces / NumSpaces;
  state.dataGlobal->NumOfZones = NumZones;

  int FirstZone = 0;
  int LastZone = NumZones - 1;
  state.dataHeatBal->Zone.resize(NumZones);
  state.dataHeatBal->space.resize(NumSpaces);
  state.dataSurface->TotSurfaces = NumSpaces * SurfacesPerSpace;
  state.dataSurface->Surface.resize(state.dataSurface->TotSurfaces);

  const size_t NumSurfaces = state.dataSurface->TotSurfaces;
  state.dataHeatBalFanSys->QRadSurfAFNDuct.resize(NumSurfaces);
  state.dataHeatBalFanSys->PoolHeatTransCoefs.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfCTFInside0.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfIsAdiabatic.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfCTFCross0.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfIsOperatingPool.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfTempTerm.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfTempDiv.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfCTFConstInPart.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfOpaqQRadSWInAbs.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfQAdditionalHeatSourceInside.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfHConvInt.resize(NumSurfaces);
  state.dataHeatBalSurf->SurfQdotRadHVACInPerArea.resize(NumSurfaces);
  state.dataHeatBalSurfMgr->RefAirTemp.resize(NumSurfaces);
  state.dataHeatBal->SurfQdotRadIntGainsInPerArea.resize(NumSurfaces);

  for (int zoneNum = FirstZone; zoneNum <= LastZone; ++zoneNum) {
    state.dataHeatBal->Zone[zoneNum].spaceIndexes = {zoneNum * 2, zoneNum * 2 + 1};
    for (int spaceNum : state.dataHeatBal->Zone[zoneNum].spaceIndexes) {
      state.dataHeatBal->space[spaceNum].OpaqOrIntMassSurfaceFirst = spaceNum * SurfacesPerSpace;
      state.dataHeatBal->space[spaceNum].OpaqOrIntMassSurfaceLast = (spaceNum + 1) * SurfacesPerSpace - 1;
      for (int surfNum = state.dataHeatBal->space[spaceNum].OpaqOrIntMassSurfaceFirst;
           surfNum <= state.dataHeatBal->space[spaceNum].OpaqOrIntMassSurfaceLast; ++surfNum) {
        state.dataSurface->Surface[surfNum].spaceNum = spaceNum;
        state.dataSurface->Surface[surfNum].Zone = zoneNum;
        state.dataSurface->Surface[surfNum].RepresentativeCalcSurfNum = surfNum;  // No representative surface calculations
      }
    }
  }

  return state;
}

void CalcHeatBalanceInsideSurf2CTFOnly_extract(EnergyPlusData& state) {
  int FirstZone = 0;
  int LastZone = state.dataGlobal->NumOfZones - 1;

  for (int zoneNum = FirstZone; zoneNum <= LastZone; ++zoneNum) {
    for (int spaceNum : state.dataHeatBal->Zone[zoneNum].spaceIndexes) {
      auto const& thisSpace = state.dataHeatBal->space[spaceNum];
      int const firstSurf = thisSpace.OpaqOrIntMassSurfaceFirst;
      int const lastSurf = thisSpace.OpaqOrIntMassSurfaceLast;

      Real64 const timeStepZoneSeconds = state.dataGlobal->TimeStepZoneSec;  // local for vectorization
      Real64 const iterDampConstant = DataHeatBalSurface::IterDampConst;     // local for vectorization
      // this loop auto-vectorizes
      for (int surfNum = firstSurf; surfNum <= lastSurf; ++surfNum) {
        auto const& surface = state.dataSurface->Surface[surfNum];
        if (state.dataSurface->UseRepresentativeSurfaceCalculations) {
          int repSurfNum = surface.RepresentativeCalcSurfNum;
          if (surfNum != repSurfNum) {
            continue;
          }
        }

        state.dataHeatBalSurf->SurfTempTerm[surfNum] =
          state.dataHeatBalSurf->SurfCTFConstInPart[surfNum] + state.dataHeatBal->SurfQdotRadIntGainsInPerArea[surfNum]
          + state.dataHeatBalSurf->SurfOpaqQRadSWInAbs[surfNum] + state.dataHeatBalSurf->SurfQAdditionalHeatSourceInside[surfNum]
          + state.dataHeatBalSurf->SurfHConvInt[surfNum] * state.dataHeatBalSurfMgr->RefAirTemp[surfNum]
          + state.dataHeatBalSurf->SurfQdotRadHVACInPerArea[surfNum] + (state.dataHeatBalFanSys->QRadSurfAFNDuct[surfNum] / timeStepZoneSeconds);
        state.dataHeatBalSurf->SurfTempDiv[surfNum] =
          1.0
          / (state.dataHeatBalSurf->SurfCTFInside0[surfNum]
             - state.dataHeatBalSurf->SurfIsAdiabatic[surfNum] * state.dataHeatBalSurf->SurfCTFCross0[surfNum]
             + state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] * state.dataHeatBalFanSys->PoolHeatTransCoefs[surfNum]
             + static_cast<double>(state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] == 0) * state.dataHeatBalSurf->SurfHConvInt[surfNum]
             + iterDampConstant);
      }
    }
  }
}

void CalcHeatBalanceInsideSurf2CTFOnly_simd(EnergyPlusData& state) {
  int FirstZone = 0;
  int LastZone = state.dataGlobal->NumOfZones - 1;

  for (int zoneNum = FirstZone; zoneNum <= LastZone; ++zoneNum) {
    for (int spaceNum : state.dataHeatBal->Zone[zoneNum].spaceIndexes) {
      auto const& thisSpace = state.dataHeatBal->space[spaceNum];
      int const firstSurf = thisSpace.OpaqOrIntMassSurfaceFirst;
      int const lastSurf = thisSpace.OpaqOrIntMassSurfaceLast;

      Real64 const timeStepZoneSeconds = state.dataGlobal->TimeStepZoneSec;  // local for vectorization
      Real64 const iterDampConstant = DataHeatBalSurface::IterDampConst;     // local for vectorization
#pragma omp simd
      for (int surfNum = firstSurf; surfNum <= lastSurf; ++surfNum) {
        auto const& surface = state.dataSurface->Surface[surfNum];
        if (state.dataSurface->UseRepresentativeSurfaceCalculations) {
          int repSurfNum = surface.RepresentativeCalcSurfNum;
          if (surfNum != repSurfNum) {
            continue;
          }
        }

        state.dataHeatBalSurf->SurfTempTerm[surfNum] =
          state.dataHeatBalSurf->SurfCTFConstInPart[surfNum] + state.dataHeatBal->SurfQdotRadIntGainsInPerArea[surfNum]
          + state.dataHeatBalSurf->SurfOpaqQRadSWInAbs[surfNum] + state.dataHeatBalSurf->SurfQAdditionalHeatSourceInside[surfNum]
          + state.dataHeatBalSurf->SurfHConvInt[surfNum] * state.dataHeatBalSurfMgr->RefAirTemp[surfNum]
          + state.dataHeatBalSurf->SurfQdotRadHVACInPerArea[surfNum] + (state.dataHeatBalFanSys->QRadSurfAFNDuct[surfNum] / timeStepZoneSeconds);
        state.dataHeatBalSurf->SurfTempDiv[surfNum] =
          1.0
          / (state.dataHeatBalSurf->SurfCTFInside0[surfNum]
             - state.dataHeatBalSurf->SurfIsAdiabatic[surfNum] * state.dataHeatBalSurf->SurfCTFCross0[surfNum]
             + state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] * state.dataHeatBalFanSys->PoolHeatTransCoefs[surfNum]
             + static_cast<double>(state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] == 0) * state.dataHeatBalSurf->SurfHConvInt[surfNum]
             + iterDampConstant);
      }
    }
  }
}

void CalcHeatBalanceInsideSurf2CTFOnly_simd_rewrite(EnergyPlusData& state) {
  //int FirstZone = 0;
  //int LastZone = state.dataGlobal->NumOfZones - 1;

#  pragma omp simd
  for (int surfNum = 0; surfNum < state.dataSurface->TotSurfaces; ++surfNum) {
    //auto const& surface = state.dataSurface->Surface[surfNum];
    //if (surface.Zone < FirstZone || surface.Zone > LastZone) {
      //continue;
    //}

    //if (state.dataSurface->UseRepresentativeSurfaceCalculations) {
      //int repSurfNum = surface.RepresentativeCalcSurfNum;
      //if (surfNum != repSurfNum) {
        //continue;
      //}
    //}

    Real64 const timeStepZoneSeconds = state.dataGlobal->TimeStepZoneSec;  // local for vectorization
    Real64 const iterDampConstant = DataHeatBalSurface::IterDampConst;     // local for vectorization
    state.dataHeatBalSurf->SurfTempTerm[surfNum] =
      state.dataHeatBalSurf->SurfCTFConstInPart[surfNum] + state.dataHeatBal->SurfQdotRadIntGainsInPerArea[surfNum]
      + state.dataHeatBalSurf->SurfOpaqQRadSWInAbs[surfNum] + state.dataHeatBalSurf->SurfQAdditionalHeatSourceInside[surfNum]
      + state.dataHeatBalSurf->SurfHConvInt[surfNum] * state.dataHeatBalSurfMgr->RefAirTemp[surfNum]
      + state.dataHeatBalSurf->SurfQdotRadHVACInPerArea[surfNum] + (state.dataHeatBalFanSys->QRadSurfAFNDuct[surfNum] / timeStepZoneSeconds);
    state.dataHeatBalSurf->SurfTempDiv[surfNum] =
      1.0
      / (state.dataHeatBalSurf->SurfCTFInside0[surfNum]
         - state.dataHeatBalSurf->SurfIsAdiabatic[surfNum] * state.dataHeatBalSurf->SurfCTFCross0[surfNum]
         + state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] * state.dataHeatBalFanSys->PoolHeatTransCoefs[surfNum]
         + static_cast<double>(state.dataHeatBalSurf->SurfIsOperatingPool[surfNum] == 0) * state.dataHeatBalSurf->SurfHConvInt[surfNum]
         + iterDampConstant);
  }
}

static void BM_Regular(benchmark::State& bm_state) {
  EnergyPlusData state = create_test_state(static_cast<int>(bm_state.range(0)));
  bm_state.SetComplexityN(bm_state.range(0));

  // Code inside this loop is measured repeatedly
  for (auto _ : bm_state) {
    CalcHeatBalanceInsideSurf2CTFOnly_extract(state);
    benchmark::DoNotOptimize(state);
  }
}

BENCHMARK(BM_Regular)->RangeMultiplier(10)->Range(100, 100000)->Complexity();

static void BM_OmpSimd(benchmark::State& bm_state) {
  EnergyPlusData state = create_test_state(static_cast<int>(bm_state.range(0)));
  bm_state.SetComplexityN(bm_state.range(0));

  // Code inside this loop is measured repeatedly
  for (auto _ : bm_state) {
    CalcHeatBalanceInsideSurf2CTFOnly_simd(state);
    benchmark::DoNotOptimize(state);
  }
}

BENCHMARK(BM_OmpSimd)->RangeMultiplier(10)->Range(100, 100000)->Complexity();

static void BM_OmpSimdRewrite(benchmark::State& bm_state) {
  EnergyPlusData state = create_test_state(static_cast<int>(bm_state.range(0)));
  bm_state.SetComplexityN(bm_state.range(0));

  // Code inside this loop is measured repeatedly
  for (auto _ : bm_state) {
    CalcHeatBalanceInsideSurf2CTFOnly_simd_rewrite(state);
    benchmark::DoNotOptimize(state);
  }
}
BENCHMARK(BM_OmpSimdRewrite)->RangeMultiplier(10)->Range(100, 100000)->Complexity();
