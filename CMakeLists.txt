project(redis3m)
cmake_minimum_required(VERSION 2.8)

set(REDIS3M_VERSION_MAJOR 0)
set(REDIS3M_VERSION_MINOR 1)
set(REDIS3M_VERSION_PATCH 0)
set(REDIS3M_VERSION_STRING ${REDIS3M_VERSION_MAJOR}.${REDIS3M_VERSION_MINOR}.${REDIS3M_VERSION_PATCH})
 
find_package(Boost COMPONENTS system filesystem unit_test_framework date_time REQUIRED)

# Library
include_directories("${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})
file(GLOB SRC_FILES "src/*.cpp" "src/utils/*.cpp" "src/patterns/*.cpp")
file(GLOB INCLUDE_FILES "include/redis3m/*.h" "include/redis3m/utils/*.h" "include/redis3m/patterns/*.h")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME} hiredis ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${REDIS3M_VERSION_STRING}
                                           SOVERSION ${REDIS3M_VERSION_MAJOR})


install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (DIRECTORY "${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION include)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(includedir  ${CMAKE_INSTALL_PREFIX}/include)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION lib/pkgconfig)

# Tests
add_executable(connection_test tests/connection.cpp)
target_link_libraries(connection_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_executable(patterns_test tests/patterns.cpp)
target_link_libraries(patterns_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_executable(connection_pool_test tests/connection_pool.cpp)
target_link_libraries(connection_pool_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
enable_testing ()

add_test (NAME connection COMMAND connection_test)
add_test (NAME patterns COMMAND patterns_test)
