project(redis3m)
cmake_minimum_required(VERSION 2.8)
include_directories("${PROJECT_SOURCE_DIR}/include")
add_library(${PROJECT_NAME} SHARED src/connection.cpp src/reply.cpp src/connection_pool.cpp src/utils.cpp)
target_link_libraries(${PROJECT_NAME} hiredis boost_system)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES include/redis3m/* DESTINATION include)
