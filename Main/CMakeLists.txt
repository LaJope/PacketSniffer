project(pasn)

file(GLOB SOURCE_FILES "./Source_Files/*.cpp")

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ./Header_Files/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE pcap Packet++ Pcap++ Common++ Threads::Threads
)
