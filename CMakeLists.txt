cmake_minimum_required(VERSION 3.12.0)

project(PacketSniffer)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)

set (CMAKE_CXX_STANDARD 17)

find_program(FLEX_EXECUTABLE flex)
if (NOT FLEX_EXECUTABLE)
    message(FATAL_ERROR "flex not found. Please install flex using:\n"
                        "ubuntu: sudo apt install flex\n"
                        "arch: sudo pacman -S flex\n"
                        "or your prefered package manager")
endif()

find_program(BISON_EXECUTABLE bison)
if (NOT BISON_EXECUTABLE)
    message(FATAL_ERROR "bison not found. Please install bison using:\n"
                        "ubuntu: sudo apt install bison\n"
                        "arch: sudo pacman -S bison\n"
                        "or your prefered package manager")
endif()

find_library(PCAP_LIBRARY pcap)
find_path(PCAP_INCLUDE_DIR pcap.h)

if (NOT PCAP_LIBRARY OR NOT PCAP_INCLUDE_DIR)
    message(FATAL_ERROR "pcap not found. Please install pcap using:\n"
                        "ubuntu: sudo apt install libpcap-dev\n"
                        "arch: sudo pacman -S libpcap\n"
                        "or your prefered package manager")
endif()

add_subdirectory(src/ps-database)
add_subdirectory(src/ps-io)
add_subdirectory(src/ps-utils)
add_subdirectory(src/ps-app)

add_custom_target(${PROJECT_NAME} ALL)
add_dependencies(${PROJECT_NAME} pasn)

