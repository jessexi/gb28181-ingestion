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

find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP libpjproject>=1.14 REQUIRED)


find_package(JRTPLIB REQUIRED)

include_directories(${JRTPLIB_INCLUDE_DIRS})
link_directories(${JRTPLIB_LIBRARY_DIRS})

foreach(dep ${JRTPLIB_LIBRARIES})
    message(STATUS "JRTPLIB - ${dep}")
endforeach()

include_directories(${PJSIP_INCLUDE_DIRS})
link_directories(${PJSIP_LIBRARY_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/)


pkg_check_modules(LIBAVFORMAT libavformat REQUIRED)
pkg_check_modules(LIBAVCODEC libavcodec REQUIRED)
pkg_check_modules(LIBAVUTIL  libavutil REQUIRED)
pkg_check_modules(LIBSWSCALE libswscale REQUIRED)


include_directories(	
	${LIBAVFORMAT_INCLUDE_DIRS}
	${LIBAVCODEC_INCLUDE_DIRS}
	${LIBAVUTIL_INCLUDE_DIRS}
	${LIBSWSCALE_INCLUDE_DIRS}
)


set( 
	FFMPEG_LIBRARIES	
	${LIBAVFORMAT_LIBRARIES}
	${LIBAVCODEC_LIBRARIES}
	${LIBAVUTIL_LIBRARIES}
	${LIBSWSCALE_LIBRARIES}
	${EXTRA_LIBRARIES}
)

link_directories(${LIBAVFORMAT_LIBRARY_DIRS})
link_directories(${LIBAVCODEC_LIBRARY_DIRS})
link_directories(${LIBAVUTIL_LIBRARY_DIRS})
link_directories(${LIBSWSCALE_LIBRARY_DIRS})

foreach(dep ${FFMPEG_LIBRARIES})
    message(STATUS "FFMPEG_LIBRARIES - ${dep}")
endforeach()

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

target_link_libraries(${project_name} PUBLIC ${pjsip_list} SIP_LIB ${JRTPLIB_LIBRARIES} avcodec avutil avformat swscale )
target_include_directories(${project_name} PUBLIC
                          "${PROJECT_SOURCE_DIR}/src/sip"
                          )
