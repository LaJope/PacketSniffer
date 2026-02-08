project(ps-io)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(Threads REQUIRED)
find_package(PCAPPLUSPLUS REQUIRED)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE pcap Packet++ Pcap++ Common++ Threads::Threads
)
