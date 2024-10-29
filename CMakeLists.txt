cmake_minimum_required(VERSION 3.15.0)
# cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific extensions, for eg on GCC use -std=c++1z (=c++17) and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

# Use ccache if available, has to be before "project()"
find_program(CCACHE_PROGRAM NAMES ccache sccache)
if(CCACHE_PROGRAM)
  # Support: Unix Makefiles and Ninja only
  # RULE_LAUNCH_COMPILE is an internal variable and makes RC compilation fail on windows, hence why we use the C/CXX COMPILER_LAUNCHER instead
  message(STATUS "${CCACHE_PROGRAM} found and enabled")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM} CACHE FILEPATH "CXX compiler cache used")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM} CACHE FILEPATH "C compiler cache used")
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

# Search first in the binary dir, where conan will install finders, then
# search for modules in the root dir to override cmake ones
# Start with ROOT, then PROJECT_BINARY_DIR
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/CMake")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package(Boost)
find_package(fmt)
find_package(jsoncpp)
find_package(pugixml)
find_package(XercesC)
find_package(frozen)
find_package(magic_enum)
find_package(benchmark)
find_package(GTest)

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
      benchmark::benchmark_main
      JsonCpp::JsonCpp
    )

  elseif (bench_name MATCHES "pugixml")
    add_executable( ${bench_name} ${bench_file} )
    message(STATUS "${bench_name} matches pugixml")
    target_link_libraries(${bench_name} PRIVATE
      benchmark::benchmark_main
      pugixml::pugixml
    )

  elseif(bench_name MATCHES "OpenMP")
    find_package(Threads REQUIRED)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
      message("Found OpenMP!")
      add_executable( ${bench_name} ${bench_file} )
      target_link_libraries(${bench_name} PRIVATE
        benchmark::benchmark_main
      )
      target_link_libraries(${bench_name} INTERFACE Threads::Threads)
      target_link_libraries(${bench_name} PRIVATE OpenMP::OpenMP_CXX)
    else()
      message("OpenMP not Found, skipping ${bench_name}")
    endif()
  elseif(bench_name MATCHES "bench_stl_parallel")
    message("Skipping ${bench_name}")
    # if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    #   message("Skipping ${bench_name}")
    # else()
    #   add_executable( ${bench_name} ${bench_file} )
    #   target_link_libraries(${bench_name} PRIVATE
    #     benchmark::benchmark_main
    #     tbb::tbb
    #   )
    # endif()
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
        benchmark::benchmark_main
        boost::boost
      )
    else()
      message(AUTHOR_WARNING "Ranges are NOT supported, skipping ${bench_file}")
    endif()

  elseif (bench_name MATCHES "EnumMap")
    add_executable( ${bench_name} ${bench_file} )
    message(STATUS "${bench_name} matches EnumMap")
    target_link_libraries(${bench_name} PRIVATE
      benchmark::benchmark_main
      boost::boost
      fmt::fmt
      frozen::frozen
    )

  else()
    add_executable( ${bench_name} ${bench_file} )
    target_link_libraries(${bench_name} PRIVATE
      benchmark::benchmark_main
      boost::boost
      fmt::fmt
    )
  endif()

endforeach()

add_executable( test_regex test_regex_replace.cpp )
target_link_libraries(test_regex PRIVATE
  boost::boost
  fmt::fmt
)

# add_executable(IddEnums_constexpr_test IddEnums_constexpr_test.cpp)
# target_link_libraries(IddEnums_constexpr_test PRIVATE
#   boost::boost
#   fmt::fmt
#   frozen::frozen
# )
# target_compile_options(IddEnums_constexpr_test PRIVATE
#   $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=90000000>
#   $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-fconstexpr-steps=90000000>
# )

add_library(testlib
  TestClass.hpp
  TestClass.cpp
  BCLFileReference.hpp
  BCLFileReference.cpp
)

target_link_libraries(testlib
  PRIVATE
  project_options
  boost::boost
  fmt::fmt
)

add_executable(test_exe
  test/TestFixture.hpp
  test/TestFixture.cpp

  test/TestClass_GTest.cpp
  test/Other_GTest.cpp
)
target_link_libraries(test_exe
  PRIVATE
  gtest::gtest
  project_options
  testlib
  boost::boost
  fmt::fmt
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
#   gtest::gtest
#   XercesC::XercesC
#   PRIVATE
#   project_options
# )
enable_testing()

include(GoogleTest)
gtest_discover_tests(test_exe
  # WORKING_DIRECTORY Testing
)


