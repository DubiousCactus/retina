cmake_minimum_required (VERSION 2.8)

# projectname is the same as the main-executable
project(ARKit)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions('-std=c++11')

find_package(Threads)

add_executable(
  ${PROJECT_NAME}
  src/libArkit_Frame.cpp
  src/libArkit_FeatureExtractor.cpp
  src/libArkit_StreamParser.cpp
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

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

add_custom_target(${PROJECT_NAME}-symlink ALL ln --force -s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
