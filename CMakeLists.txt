cmake_minimum_required(VERSION 3.15.0)
# cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific extensions, for eg on GCC use -std=c++1z (=c++17) and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

# Use ccache is available, has to be before "project()"
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  # Support Unix Makefiles and Ninja
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(CppBench VERSION 0.0.1)

# The PROJECT_SOURCE_DIR here is root of this repo, which we will use instead of using CMAKE_SOURCE_DIR
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# We will use BINARY_DIR instead of CMAKE_BINARY_DIR
set(PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}")

#MESSAGE(FATAL_ERROR "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}, BINARY_DIR=${BINARY_DIR}, PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}, CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}, CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

#if(POLICY CMP0020)
  #cmake_policy(SET CMP0020 NEW)
#endif()
#if(POLICY CMP0022)
  #cmake_policy(SET CMP0022 NEW)
#endif()
#if(POLICY CMP0042)
  #cmake_policy(SET CMP0042 NEW)
#endif()
#if(POLICY CMP0054)
  #cmake_policy(SET CMP0054 NEW)
#endif()
#if(POLICY CMP0077)
  #cmake_policy(SET CMP0077 NEW)
#endif()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

###############################################################################
#                                  N I N J A                                  #
###############################################################################
# Ninja support: has to be atop for it to take effect before anything else is done
# Add Color Output if Using Ninja
# Enhance error reporting and compiler messages
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not clang
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-fcolor-diagnostics> $<$<COMPILE_LANGUAGE:CXX>:-fcolor-diagnostics>)
  else()
    add_compile_options(-fcolor-diagnostics)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not gcc
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-fdiagnostics-color=always>
                        $<$<COMPILE_LANGUAGE:CXX>:-fdiagnostics-color=always>)
  else()
    add_compile_options(-fdiagnostics-color=always)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION GREATER 1900)
  add_compile_options(/diagnostics:column)
else()
  message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

# Xcode/Ninja generators undefined MAKE
#if(CMAKE_GENERATOR MATCHES "Make")
#  set(MAKE "$(MAKE)")
#else()
#  set(MAKE make)
#endif()

###############################################################################
#                         C M A K E    C O N T R O L                          #
###############################################################################

# High level project configuration
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Products")

set(LIBRARY_SEARCH_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug")

###############################################################################
#          C O M P I L E R    A N D    S Y S T E M    O P T I O N S           #
###############################################################################

# Compiler and system specific options
if(UNIX)

  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC -fno-strict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")

  # all warnings
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -fno-strict-aliasing -Winvalid-pch -Wnon-virtual-dtor")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fPIC -fno-strict-aliasing -Winvalid-pch")
  # Treat all warnings as errors, extra errors, and be pedantic
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -pedantic -Wno-unused-function")
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -ftemplate-depth=1024")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")
    # DLM: had to add this due to issues with boost optional and gcc, may be resolved if we move to std::optional
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
  endif()

  # Note: CMAKE_CXX_STANDARD set to 20 should already take care of adding -std=c++20 or equivalent
  if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif()

#######################################################################
#                              C O N A N                              #
#######################################################################

set(CMAKE_CONAN_EXPECTED_HASH
    5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd)
set(CMAKE_CONAN_VERSION "0.18.1")

if(EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  file(SHA256 "${CMAKE_BINARY_DIR}/conan.cmake" CMAKE_CONAN_HASH)
elseif(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake"
       OR NOT "${CMAKE_CONAN_HASH}" MATCHES "${CMAKE_CONAN_EXPECTED_HASH}")
  # Put it in CMAKE_BINARY_DIR so we don't end up with two when building OpenStudioApplication
  message(
    STATUS
      "Downloading conan.cmake ${CMAKE_CONAN_VERSION} from https://github.com/conan-io/cmake-conan"
  )
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/${CMAKE_CONAN_VERSION}/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=${CMAKE_CONAN_EXPECTED_HASH}
    TLS_VERIFY ON)
else()
  message(STATUS "Using existing conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.48.0 REQUIRED)

message(STATUS "RUNNING CONAN")

conan_add_remote(NAME bincrafters
  URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan)

list(APPEND CONAN_BUILD "missing")

if(CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CONAN_CMAKE_MULTI ON)
  if (NOT CONAN_CONFIGURATION_TYPES)
    # Cache it, and let user change it (if they want to pull RelWithDebInfo for eg)
    set(CONAN_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "List of configurations for which you want to fetch conan packages")
    mark_as_advanced(CONAN_CONFIGURATION_TYPES)
  endif()
  message(STATUS "Conan: Using cmake_multi generator")
  set(CONAN_GENERATOR "cmake_multi")
else()
  message(STATUS "Conan: Using cmake generator")
  set(CONAN_CMAKE_MULTI OFF)
  set(CONAN_GENERATOR "cmake")
  set(CONAN_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
endif()

message(STATUS "Conan: conan_cmake_configure")
# This will create the conanfile.txt
conan_cmake_configure(
  REQUIRES
  boost/1.79.0
  fmt/9.1.0
  benchmark/1.8.0
  jsoncpp/1.9.5
  gtest/1.13.0
  pugixml/1.12.1
  xerces-c/3.2.3
  tbb/2020.3  # onetbb/2021.9.0 is incompatible with STL algorithms, the tbb::task is removed
  frozen/1.1.1
  magic_enum/0.9.2
  GENERATORS ${CONAN_GENERATOR}
)

foreach(build_type ${CONAN_CONFIGURATION_TYPES})
  conan_cmake_autodetect(settings BUILD_TYPE ${build_type})
  message(STATUS "Conan: Autodetected settings for build type ${build_type}: ${settings}")

  # Avoid polluting with cppstd which prevents downloading some existing binary packages (like boost)
  # Former deprecated conan_cmake_run was NOT adding compiler.cppstd
  foreach(ARG ${settings})
    if(${ARG} MATCHES "compiler.cppstd=(.*)")
      message("Removing ${ARG}")
      list(REMOVE_ITEM settings ${ARG})
    endif()
  endforeach()

  conan_cmake_install(PATH_OR_REFERENCE .
    BUILD ${CONAN_BUILD}
    OPTIONS ${CONAN_OPTIONS}
    SETTINGS ${settings}
    ${CONAN_UPDATE}
  )
endforeach()

# Loads the conanbuildinfo.cmake / conanbuildinfo_multi.cmake
conan_load_buildinfo()
# conan_basic_setup in the conanbuildinfo.cmake. TARGETS => set cmake targets, NO_OUTPUT_DIRS => Don't modify the BIN / LIB folders etc
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

message("CONAN_LIBS=${CONAN_LIBS}")

# Search first in the binary dir, where conan will install finders, then
# search for modules in the root dir to override cmake ones
# Start with ROOT, then PROJECT_BINARY_DIR
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/CMake")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

###############################################################################
#                              E X E C U T A B L E                            #
###############################################################################

include_directories(${PROJECT_BINARY_DIR})

# Executables: one per cpp file
# file( GLOB APP_SOURCES RELATIVE app/*.cxx )
file(GLOB BENCH_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/bench*.cpp)


include(CheckCXXSourceCompiles)

foreach( bench_file ${BENCH_FILES} )
  string(REPLACE ".cpp" "" bench_name ${bench_file})
  message("bench_name=${bench_name}")

  if (bench_name MATCHES "json")
    add_executable( ${bench_name} ${bench_file} )
    message(STATUS "${bench_name} matches json")
    target_link_libraries(${bench_name} PRIVATE
      CONAN_PKG::benchmark
      CONAN_PKG::jsoncpp
    )

  elseif (bench_name MATCHES "pugixml")
    add_executable( ${bench_name} ${bench_file} )
    message(STATUS "${bench_name} matches pugixml")
    target_link_libraries(${bench_name} PRIVATE
      CONAN_PKG::benchmark
      CONAN_PKG::pugixml
    )

  elseif(bench_name MATCHES "OpenMP")
    find_package(Threads REQUIRED)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
      message("Found OpenMP!")
      add_executable( ${bench_name} ${bench_file} )
      target_link_libraries(${bench_name} PRIVATE
        CONAN_PKG::benchmark
      )
      target_link_libraries(${bench_name} INTERFACE Threads::Threads)
      target_link_libraries(${bench_name} PRIVATE OpenMP::OpenMP_CXX)
    else()
      message("OpenMP not Found, skipping ${bench_name}")
    endif()
  elseif(bench_name MATCHES "bench_stl_parallel")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      message("Skipping ${bench_name}")
    else()
      add_executable( ${bench_name} ${bench_file} )
      target_link_libraries(${bench_name} PRIVATE
        CONAN_PKG::benchmark
        CONAN_PKG::tbb
      )
    endif()
  elseif(bench_name MATCHES "bench_string_trim")
    check_cxx_source_compiles(
    "
#include <cmath>
#include <ranges>

#if __cplusplus < 201709L && !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#  error \"insufficient support for C++20\"
#endif

#if !(defined __cpp_lib_ranges) || (__cpp_lib_ranges < 201911)
#  error \"insufficient support for C++20\"
#endif
int main() { }"
    HAVE_CXX20_RANGES)
    if (HAVE_CXX20_RANGES)
      message("Ranges are supported")
      add_executable( ${bench_name} ${bench_file} )
      target_link_libraries(${bench_name} PRIVATE
        CONAN_PKG::benchmark
        CONAN_PKG::boost
      )
    else()
      message(AUTHOR_WARNING "Ranges are NOT supported, skipping ${bench_file}")
    endif()

  elseif (bench_name MATCHES "EnumMap")
    add_executable( ${bench_name} ${bench_file} )
    message(STATUS "${bench_name} matches EnumMap")
    target_link_libraries(${bench_name} PRIVATE
      CONAN_PKG::benchmark
      CONAN_PKG::boost
      CONAN_PKG::fmt
      CONAN_PKG::frozen
    )

  else()
    add_executable( ${bench_name} ${bench_file} )
    target_link_libraries(${bench_name} PRIVATE
      CONAN_PKG::benchmark
      CONAN_PKG::boost
      CONAN_PKG::fmt
    )
  endif()

endforeach()

add_executable( test_regex test_regex_replace.cpp )
target_link_libraries(test_regex PRIVATE
  CONAN_PKG::boost
  CONAN_PKG::fmt
)

add_executable(IddEnums_constexpr_test IddEnums_constexpr_test.cpp)
target_link_libraries(IddEnums_constexpr_test PRIVATE
  CONAN_PKG::boost
  CONAN_PKG::fmt
  CONAN_PKG::frozen
)
target_compile_options(IddEnums_constexpr_test PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=90000000>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-fconstexpr-steps=90000000>
)

add_library(testlib
  TestClass.hpp
  TestClass.cpp
  BCLFileReference.hpp
  BCLFileReference.cpp
)

target_link_libraries(testlib
  PRIVATE
  project_options
  CONAN_PKG::boost
  CONAN_PKG::fmt
)

add_executable(test_exe
  test/TestFixture.hpp
  test/TestFixture.cpp

  test/TestClass_GTest.cpp
  test/Other_GTest.cpp
)
target_link_libraries(test_exe
  PRIVATE
  CONAN_PKG::gtest
  project_options
  testlib
  CONAN_PKG::boost
  CONAN_PKG::fmt
)

# add_executable(xerces_test
#   test/TestFixture.hpp
#   test/TestFixture.cpp
#
#   test/TestXerces_GTest.cpp
#   test/DOMTreeErrorReporter.hpp
#   test/DOMTreeErrorReporter.cpp
#   )
# target_link_libraries(xerces_test
#   PUBLIC
#   CONAN_PKG::gtest
#   CONAN_PKG::xerces-c
#   PRIVATE
#   project_options
# )
enable_testing()

include(GoogleTest)
gtest_discover_tests(test_exe
  # WORKING_DIRECTORY Testing
)


