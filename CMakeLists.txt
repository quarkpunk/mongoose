cmake_minimum_required(VERSION 3.22)

# project
project(mongoose CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# options
option(MONGOOSE_BUILD_TESTS "Mongoose build tests" OFF)

# packages
find_package(mongocxx REQUIRED)
find_package(nlohmann_json REQUIRED)

# sources
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

# library
add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC
    mongo::mongocxx_static
    nlohmann_json::nlohmann_json
)

# precompile
target_precompile_headers(${PROJECT_NAME} PUBLIC
    <string>
    <nlohmann/json.hpp>
)

# tests
if(MONGOOSE_BUILD_TESTS)
    message("++ mongoose tests")
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()

# includes
message("++ mongoose includes")
message(${Boost_INCLUDE_DIRS})
message(${mongocxx_INCLUDE_DIRS})
message(${nlohmann_json_INCLUDE_DIRS})