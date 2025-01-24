project(Main)

set(SOURCE_FILES ./Source_Files/main.cpp ./Source_Files/netInterfaceListener.cpp ./Source_Files/main.cpp.bak ./Source_Files/PcapFileSniffer.cpp ./Source_Files/csvPcapWriter.cpp ./Source_Files/Application.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ./Header_Files/
  ${CMAKE_BINARY_DIR}/PcapPlusPlus-install/include
)

target_link_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_BINARY_DIR}/PcapPlusPlus-install/lib
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE pcap Packet++ Pcap++ Common++
)
