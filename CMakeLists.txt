cmake_minimum_required(VERSION 3.12.0)

project(PacketSniffer)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)

include(ExternalProject)

ExternalProject_Add(
  PcapPlusPlus
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/PcapPlusPlus
  BINARY_DIR ${CMAKE_BINARY_DIR}/PcapPlusPlus-build
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/PcapPlusPlus-install
  BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --parallel ${CMAKE_BUILD_PARALLEL_LEVEL}
  INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
)

add_subdirectory(Main)

add_custom_target(${PROJECT_NAME} ALL)
add_dependencies(${PROJECT_NAME} Main PcapPlusPlus)
add_dependencies(Main PcapPlusPlus)

