cmake_minimum_required(VERSION 3.16)
project(Algorep)

# External libraries
find_package(MPI REQUIRED)

# Compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER mpic++)

# Default build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, fallback to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Compilation flags
set(CMAKE_CXX_FLAGS "-pedantic -Werror -Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DRELEASE -DNDEBUG -march=native")

add_subdirectory(src/messages)

add_executable(my_aws_s3 src/main.cpp)

target_sources(my_aws_s3 PRIVATE
    src/client.cpp
    src/client.hh
    src/message_handlers.cpp
    src/repl_handlers.cpp
    src/repl_process.cpp
    src/repl_process.hh
    src/repl.cpp
    src/repl.hh
    src/rpc_handlers.cpp
    src/server.cpp
    src/server.h
)

target_include_directories(my_aws_s3 PRIVATE
    includes
)

target_link_libraries(my_aws_s3 PRIVATE
    messages
)