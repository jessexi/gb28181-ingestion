# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 2.8)

set(project_name "gb28181")
set(gb_version 1.0.0)

set(CMAKE_CXX_STANDARD 11)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
#cmake_policy(SET CMP0081 NEW)

# set the project name
project(${project_name} VERSION ${gb_version})


#include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(/usr/local/include/)
#link_directories(${PROJECT_SOURCE_DIR}/pjlibs/)
link_directories(/usr/local/lib)


find_package(JRTPLIB REQUIRED)

include_directories(${JRTPLIB_INCLUDE_DIRS})
link_directories(${JRTPLIB_LIBRARY_DIRS})


include_directories(${PROJECT_SOURCE_DIR}/)


add_subdirectory(src/sip)


# add the executable
add_executable(${project_name} application.cpp )

set(pjsip_list "")

foreach(dep ${PJSIP_LIBRARIES})
if("${dep}" MATCHES "no-exclude-audiodev")
    message(STATUS "exclude - ${dep}")
else()
    message(STATUS "include - ${dep}")
    list(APPEND pjsip_list "${dep}")
endif()
endforeach()

target_link_libraries(${project_name} PUBLIC ${pjsip_list} SIP_LIB  ${JRTPLIB_LIBRARIES} avcodec avformat avutil
      pjsua pjsua2 pjlib-util pjnath pjmedia pjsip pj pthread pjsip-ua srtp )

target_include_directories(${project_name} PUBLIC
                          "${PROJECT_SOURCE_DIR}/src/sip"
                          )
