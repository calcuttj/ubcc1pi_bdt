add_library(
  ubcc1pi_Objects
  FileReader.cxx
)

target_include_directories(
  ubcc1pi_Objects PUBLIC
  $ENV{ROOT_INC}
  ${CMAKE_SOURCE_DIR}/ubcc1pi/Interface/
)

target_link_libraries(
  ubcc1pi_Objects
  ubcc1pi_Interface 
)
