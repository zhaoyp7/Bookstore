cmake_minimum_required(VERSION 3.16)
project(BookStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(include)

add_executable(code src/BlockLinkedList.cpp
                    src/account.cpp
                    src/command.cpp
                    src/book.cpp
                    src/log.cpp
                    src/main.cpp)