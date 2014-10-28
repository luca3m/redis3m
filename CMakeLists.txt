project(redis3m)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*"
    AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build")
endif()

set(REDIS3M_VERSION_MAJOR 2)
set(REDIS3M_VERSION_MINOR 1)
set(REDIS3M_VERSION_PATCH 0)
set(REDIS3M_VERSION_STRING ${REDIS3M_VERSION_MAJOR}.${REDIS3M_VERSION_MINOR}.${REDIS3M_VERSION_PATCH})
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -pipe")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe")
if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(datadir ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
else()
   set(datadir data)
endif()
find_package(Boost COMPONENTS thread system filesystem unit_test_framework date_time regex REQUIRED)

# Library
include_directories("${PROJECT_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS})
file(GLOB SRC_FILES "src/*.cpp" "src/utils/*.cpp" "src/patterns/*.cpp")
file(GLOB INCLUDE_FILES "include/redis3m/*.h*" "include/redis3m/utils/*.h" "include/redis3m/patterns/*.h*")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/utils/datadir.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/datadir.cpp @ONLY)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/datadir.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME} hiredis ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_REGEX_LIBRARY})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${REDIS3M_VERSION_STRING}
                                           SOVERSION ${REDIS3M_VERSION_MAJOR})


install (TARGETS ${PROJECT_NAME} DESTINATION lib${LIB_SUFFIX})
install (DIRECTORY "${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION include)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib${LIB_SUFFIX})
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(includedir  ${CMAKE_INSTALL_PREFIX}/include)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)


set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELEASE "DATADIR=\"${datadir}\"")
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "DATADIR=\"${CMAKE_SOURCE_DIR}/data\"")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${libdir}/pkgconfig)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/lua" DESTINATION ${datadir})

# Tests
add_executable(connection_test tests/connection.cpp tests/common.h)
target_link_libraries(connection_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_executable(patterns_test tests/patterns.cpp)
target_link_libraries(patterns_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_executable(connection_pool_test tests/connection_pool.cpp)
target_link_libraries(connection_pool_test ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY}
                              ${Boost_THREAD_LIBRARY}
                              ${Boost_SYSTEM_LIBRARY}
                              ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
enable_testing ()

add_test (NAME connection COMMAND connection_test)
#add_test (NAME connection_pool COMMAND connection_pool_test)
add_test (NAME patterns COMMAND patterns_test)

# Docs
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
        SET(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/Doxyfile")
  SET(DOXYGEN_OUTPUT "docs")

  ADD_CUSTOM_COMMAND(
      OUTPUT ${DOXYGEN_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${DOXYGEN_INPUT}
  )

  ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

  ADD_CUSTOM_TARGET(apidoc_forced
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

ENDIF (DOXYGEN_FOUND)
