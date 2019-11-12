cmake_minimum_required (VERSION 3.9)

# projectname is the same as the main-executable
project(arlite VERSION 0.1 DESCRIPTION "An (almost) zero-dependency AR library")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions('-std=c++17')

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Download and unpack googletest at configure time
configure_file(cmake/CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

find_package(Threads)

add_library(${PROJECT_NAME} SHARED
  src/Frame.cpp
  src/StreamParser.cpp
  src/ORB.cpp
  src/Harris.cpp
  src/FAST.cpp
  src/BRIEF.cpp
)

#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION}
  #PUBLIC_HEADER include/${PROJECT_NAME}.h)

configure_file(cmake/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

include(GNUInstallDirs)

set(YOU_NEED_X11 1)
set(YOU_NEED_PNG 0)
set(YOU_NEED_JPG 1)

if(${YOU_NEED_X11} EQUAL 1)
    message(STATUS "Looking for X11...")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE cimg_display=0)
endif()

if(${YOU_NEED_JPG} EQUAL 1)
    message(STATUS "Looking for libjpg...")
    find_package(JPEG REQUIRED)
    include_directories(${JPEG_INCLUDE_DIR})
    target_link_libraries (${PROJECT_NAME} ${JPEG_LIBRARY})
    target_compile_definitions(${PROJECT_NAME} PRIVATE cimg_use_jpeg=1)
endif()

if(${YOU_NEED_PNG} EQUAL 1)
    message(STATUS "Looking for libpng...")
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries (${PROJECT_NAME} ${PNG_LIBRARY})
    target_compile_definitions(${PROJECT_NAME} PRIVATE cimg_use_png=1)
endif()


message(STATUS "Looking for ffmpeg...")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(FFmpeg REQUIRED)
if (FFMPEG_FOUND)
    include_directories(${FFMPEG_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})
endif(FFMPEG_FOUND)

enable_testing()
add_executable(${PROJECT_NAME}_test test/Matrix_test.cpp)
target_link_libraries(${PROJECT_NAME}_test gtest_main)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
  ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-config
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

install(EXPORT ${PROJECT_NAME}-config DESTINATION share/${PROJECT_NAME}/cmake)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
