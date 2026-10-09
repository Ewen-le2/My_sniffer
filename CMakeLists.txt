cmake_minimum_required(VERSION 3.20)

project(My_sniffer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(My_sniffer
    src/main.cpp
    src/fcn_helper.cpp
    src/nic_factory.cpp
    src/nic/get_adapters.cpp
    src/keyboard.cpp

    src/protocol_parsing/ethernet.cpp
    src/protocol_parsing/ipv4.cpp
    src/protocol_parsing/tcp.cpp
    src/protocol_parsing/udp.cpp

    src/filter/filter.cpp
    src/filter/filter_common.cpp
    src/filter/filter_proto.cpp
    src/filter/filter_port.cpp
    src/filter/filter_ip.cpp
    src/filter/filter_mac.cpp

    src/analysis/analysis.cpp
    src/analysis/traffic_analyzer.cpp
    src/analysis/beacon_detector.cpp
    src/analysis/alert.cpp
)

if(WIN32)
    target_sources(My_sniffer PRIVATE
        src/nic/nic_windows.cpp
    )
    target_link_libraries(My_sniffer PRIVATE
        ws2_32
        wpcap
        Packet
    )
    target_link_directories(My_sniffer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk/Lib/x64
    )
elseif(UNIX)
    target_sources(My_sniffer PRIVATE
        src/nic/nic_linux.cpp
    )
endif()

target_include_directories(My_sniffer
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk/Include
)

if(MSVC)
    target_compile_options(My_sniffer PRIVATE /W4)
else()
    target_compile_options(My_sniffer PRIVATE -Wall -Wextra -Wpedantic)
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/include/version.hpp.in
    ${PROJECT_BINARY_DIR}/generated/version.hpp
    @ONLY
)

target_include_directories(My_sniffer
    PRIVATE
        ${PROJECT_BINARY_DIR}/generated
)

include(GNUInstallDirs)

install(
    TARGETS My_sniffer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES
        README.md
        CHANGELOG.md
        LICENSE
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

set(CPACK_PACKAGE_NAME "my-sniffer")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "My_sniffer")
set(CPACK_PACKAGE_CONTACT "you@example.com")
set(
    CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Cross-platform packet sniffer"
)

set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}"
)

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ;DEB")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "you@example.com")
    set(CPACK_DEBIAN_PACKAGE_SECTION "net")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
        "Cross-platform packet sniffer using libpcap."
    )

endif()

include(CPack)