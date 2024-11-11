add_library(
  ubcc1pi_Interface
  Event.cxx
  EventPeLEE.cxx
  #EventXSec.cxx
  Subrun.cxx
  SubrunPeLEE.cxx
)
target_include_directories(
  ubcc1pi_Interface PUBLIC
  $ENV{ROOT_INC}
)

target_link_libraries(
  ubcc1pi_Interface
  ${ROOT_BASIC_LIB_LIST}
)
