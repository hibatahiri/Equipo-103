cmake_minimum_required(VERSION 3.10)
project(TutoringUCO)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/Database)
include_directories(src/Users)
include_directories(src/Alert)
include_directories(src/Chat)
include_directories(src/System)

add_executable(TutUCO
    main.cc
    src/Database/DBManager.cc
    src/Database/sqlite3.c
    src/Users/Users.cc
    src/Alert/Alert.cc
    src/Chat/Chat.cc
    src/System/System.cc
)

find_package(Threads REQUIRED)

target_link_libraries(TutUCO PRIVATE Threads::Threads dl)

add_subdirectory(tests)