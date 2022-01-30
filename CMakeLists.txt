cmake_minimum_required(VERSION 3.2)
PROJECT(indi_lunatico CXX C)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")
include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif()

SET(CMAKE_CXX_STANDARD 11)
SET(RULES_INSTALL_DIR "/lib/udev/rules.d/")

find_package(INDI REQUIRED)

set (BEAVER_VERSION_MAJOR 1)
set (BEAVER_VERSION_MINOR 1)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/VERSION.cmake ${CMAKE_CURRENT_BINARY_DIR}/VERSION )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/indi_beaver.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/indi_beaver.xml )

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${INDI_INCLUDE_DIR})

include(CMakeCommon)

########### Beaver Dome ###########
set(beaver_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/beaver_dome.cpp
   )

add_executable(indi_beaver_dome ${beaver_SRCS})
target_link_libraries(indi_beaver_dome ${INDI_LIBRARIES} )
install(TARGETS indi_beaver_dome RUNTIME DESTINATION bin )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/indi_beaver.xml DESTINATION ${INDI_DATA_DIR})

