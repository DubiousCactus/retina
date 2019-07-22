cmake_minimum_required (VERSION 2.8)

# projectname is the same as the main-executable
project(ARKit)

add_definitions('-g')
add_definitions('-Wall')
add_definitions('-std=c++11')

find_package(Threads)

add_executable(${PROJECT_NAME} src/libArkit_FeatureExtractor.cpp src/main.cpp)

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

add_custom_target(${PROJECT_NAME}-symlink ALL ln --force -s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
