cmake_minimum_required (VERSION 2.8)
project (vcf-validator CXX C)

set (vcf-validator_VERSION_MAJOR 0)
set (vcf-validator_VERSION_MINOR 2)

set (CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -g")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories (inc)
include_directories (lib)

macro (declare_mod MODNAME)
  file (GLOB MOD_${MODNAME}_INC "inc/${MODNAME}/*")
  file (GLOB MOD_${MODNAME}_SRC "src/${MODNAME}/*")
  set (MOD_${MODNAME}_ALL ${MOD_${MODNAME}_INC} ${MOD_${MODNAME}_SRC})
  add_library (mod_${MODNAME} ${MOD_${MODNAME}_ALL})
endmacro (declare_mod)

# Application modules
declare_mod (vcf)

# Dependency libraries
if (BUILD_STATIC)
  set(Boost_USE_STATIC_LIBS ON) # only find static libs
endif (BUILD_STATIC)
find_package (Boost COMPONENTS filesystem program_options regex system REQUIRED )
include_directories (${Boost_INCLUDE_DIR} )


# reference: http://stackoverflow.com/questions/7876753/reusing-custom-makefile-for-static-library-with-cmake
# set the output destination
set(ROCKSDB_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/rocksdb_test/rocksdb/librocksdb.a)
# create a custom target called build_rocksdb that is part of ALL
# and will run each time you type make
add_custom_target(build_rocksdb ALL
        COMMAND ./build_rocksdb.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Original rocksdb makefile target")
# now create an imported static target
add_library(rocksdb STATIC IMPORTED)
# Import target "rocksdb" for configuration ""
set_property(TARGET rocksdb APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(rocksdb PROPERTIES
        IMPORTED_LOCATION_NOCONFIG "${ROCKSDB_LIBRARY}")
# now you can use rocksdb as if it were a regular cmake built target in your project
add_dependencies(rocksdb build_rocksdb)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/rocksdb_test/rocksdb/include)

# note, this will only work on linux/unix platforms, also it does building
# in the source tree which is also sort of bad style and keeps out of source
# builds from working.

find_package (Threads REQUIRED)

# Static build extra flags
if (BUILD_STATIC)
  set (BUILD_SHARED_LIBRARIES OFF)
  set (CMAKE_EXE_LINKER_FLAGS "-static")
  set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
  set (CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)       # remove -Wl,-Bdynamic
  set (CMAKE_EXE_LINK_DYNAMIC_CXX_FLAGS)
  set (CMAKE_SHARED_LIBRARY_C_FLAGS)         # remove -fPIC
  set (CMAKE_SHARED_LIBRARY_CXX_FLAGS)
  set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)    # remove -rdynamic
  set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
endif (BUILD_STATIC)

# Application tests
file (GLOB ALL_TESTS "test/*/*")
add_executable (test_validator test/main_test.cpp ${ALL_TESTS})
target_link_libraries (test_validator mod_vcf ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
enable_testing ()
add_test (NAME ValidatorTests COMMAND test_validator)

# Build binary
add_executable (vcf_validator src/main.cpp)
target_link_libraries (vcf_validator mod_vcf ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


add_executable (test_rocksdb_simple_example rocksdb_test/simple_example.cc)
target_link_libraries (test_rocksdb_simple_example mod_vcf ${Boost_LIBRARIES} rocksdb z snappy ${CMAKE_THREAD_LIBS_INIT})



add_executable (test_rocksdb_benchmark rocksdb_test/benchmark.cpp)
target_link_libraries (test_rocksdb_benchmark mod_vcf ${Boost_LIBRARIES} rocksdb z snappy ${CMAKE_THREAD_LIBS_INIT})


