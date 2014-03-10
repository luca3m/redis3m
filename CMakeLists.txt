project(redis3m)
cmake_minimum_required(VERSION 2.8)

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB SRC_FILES "src/*.cpp" "src/utils/*.cpp" "src/patterns/*.cpp")
file(GLOB INCLUDE_FILES "include/redis3m/*.h" "include/redis3m/utils/*.h" "include/redis3m/patterns/*.h")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} hiredis ${Boost_SYSTEM_LIBRARY})

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES ${INCLUDE_FILES} DESTINATION include/redis3m)
