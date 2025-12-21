cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(SOURCE_FILES
    ../src/Users/Users.cc
    ../src/Database/DBManager.cc
    ../src/Alert/Alert.cc
    ../src/System/System.cc
    ../src/Chat/Chat.cc
)

set(TEST_FILES
    Users_test.cc
    Alert_test.cc
    System_test.cc
)

add_executable(run_unit_tests 
    ${TEST_FILES} 
    ${SOURCE_FILES}
)

target_link_libraries(run_unit_tests 
    gtest_main 
    sqlite3 
    pthread
)

include(GoogleTest)
gtest_discover_tests(run_unit_tests)