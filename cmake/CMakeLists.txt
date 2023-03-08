# CMakeLists.txt for nd280Geant4Sim package. It creates a library with
# dictionary and a main program
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

find_package(nd280SoftwarePolicy 3)
if(NOT nd280SoftwarePolicy_FOUND)
  message(FATAL_ERROR " nd280SoftwarePolicy not found - abort ")
endif()

include(g4NeutronXSPackageVersion.cmake)
ND280_PROJECT(g4NeutronXS ${PACKAGE_VERSION})
include(g4NeutronXSND280_USE.cmake)

nd280_standard_library()
# Build the dictionary for the i/o classes.

#ROOT_GENERATE_DICTIONARY(G_ND280HadPhysicsList ND280HadPhysicsList.hh)

#ROOT_GENERATE_DICTIONARY(G_AddPhysicsList ND280HadPhysicsList.hh)

ROOT_GENERATE_DICTIONARY(G_ProdTuple_t ProdTuple_t.hh)

ROOT_GENERATE_DICTIONARY(G_TrackInfo_t TrackInfo_t.hh)

include_directories(src)
include_directories(inc)

# Build the library.
add_library(ProdTuple_t G_ProdTuple_t.cxx)
add_library(TrackInfo_t G_TrackInfo_t.cxx)
#add_library(ND280HadPhysicsList G_ND280HadPhysicsList.cxx)
#add_library(AddPhysicsList G_AddPhysicsList.cxx)

ND280_EXECUTABLE(g4had g4had.cc)
target_link_libraries(g4had.exe gfortran ProdTuple_t TrackInfo_t)

nd280_end_project()

file(GLOB macFiles ../src/*.mac)
machinedependentoutput(OutputDirectory)

message(" copy .mac files from ../src into ${OutputDirectory}")
file(COPY ${macFiles}
     DESTINATION ../${OutputDirectory}
     FILE_PERMISSIONS OWNER_READ
                      OWNER_WRITE
                      GROUP_READ
                      GROUP_WRITE
                      WORLD_READ
                      WORLD_WRITE)


