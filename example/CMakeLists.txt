cmake_minimum_required (VERSION 3.9)

# projectname is the same as the main-executable
project(orb)

add_definitions('-g')
add_definitions('-Wall')
add_definitions('-std=c++17')
add_definitions('-O3')

find_package(arlite REQUIRED)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME} arlite)

add_custom_target(${PROJECT_NAME}-symlink ALL ln --force -s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})
