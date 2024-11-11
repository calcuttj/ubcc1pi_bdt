add_subdirectory(Interface)
add_subdirectory(Objects)

add_library(
  BDT SHARED
  BDT.cxx
)

target_include_directories(
  BDT PUBLIC
  #ROOT_INCLUDE_DIRS 
)
target_link_libraries(
  BDT PUBLIC
  ubcc1pi_Interface
  ${ROOT_LIBRARIES}
)

add_executable(
  WriteScores
  WriteBDTScoresLight.cxx
)
target_include_directories(
  WriteScores PUBLIC
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(
  WriteScores PUBLIC
  ubcc1pi_Interface
  ubcc1pi_Objects
  BDT
  ${ROOT_LIBRARIES}
  ROOT::TMVA
  ${Boost_LIBRARIES}
  Boost::program_options
)
