cmake_minimum_required(VERSION 3.17)
project(retina)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include_directories(include)
include_directories(src)
find_package(Threads)

add_executable(retina
        include/retina.h
        include/BRIEF.h
        include/FAST.h
        include/FeatureExtractor.h
        include/Frame.h
        include/Harris.h
        include/Matrix.h
        include/ORB.h
        include/StreamParser.h
        include/CImg.h
        src/BRIEF.cpp
        src/debug.cpp
        include/debug.h
        src/FAST.cpp
        src/Frame.cpp
        src/Harris.cpp
        src/ORB.cpp
        src/StreamParser.cpp)


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
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
