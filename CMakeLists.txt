project(redis3m)
cmake_minimum_required(VERSION 2.8)

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

# Library
include_directories("${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})
file(GLOB SRC_FILES "src/*.cpp" "src/utils/*.cpp" "src/patterns/*.cpp")
file(GLOB INCLUDE_FILES "include/redis3m/*.h" "include/redis3m/utils/*.h" "include/redis3m/patterns/*.h")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} hiredis ${Boost_SYSTEM_LIBRARY})

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES ${INCLUDE_FILES} DESTINATION include/redis3m)

# Tests
add_executable(tests/connection tests/connection.cpp)
target_link_libraries(tests/connection ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
enable_testing ()
add_test (NAME connection COMMAND tests/connection)
