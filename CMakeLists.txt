# ======================================================================
#  ubcc1pi main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/mypandoraanalysis/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(ubcc1pi)
set(CMAKE_CXX_STANDARD 20)
# these are minimum required versions, not the actual product versions
#find_ups_product( art v1_09_00 )
#find_ups_product( cetbuildtools v3_10_00 )
#find_ups_product( nusimdata )
#find_ups_product( lardataobj )
#find_ups_product( larevt )
#find_ups_product( larsim )

# macros for dictionary and simple_plugin
#include(ArtDictionary)
#include(ArtMake)
#include(BuildPlugins)

find_package(ROOT REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

# source
add_subdirectory(ubcc1pi)
# configuration
#add_subdirectory(fcl)
#add_subdirectory(scripts)

# unit tests
#add_subdirectory(test)

# ups - table and config files
#add_subdirectory(ups)

# packaging utility
#include(UseCPack)
