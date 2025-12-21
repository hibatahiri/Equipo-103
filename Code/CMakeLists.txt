cmake_minimum_required(VERSION 3.10)
project(TutoringUCO)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. DIRECTORIOS DE INCLUSIÓN
include_directories(src/Database)
include_directories(src/Users)
include_directories(src/Alert)
include_directories(src/Chat)
include_directories(src/System)

# 2. DEFINICIÓN DEL EJECUTABLE PRINCIPAL
add_executable(TutUCO
    main.cc
    src/Database/DBManager.cc
    src/Database/sqlite3.c
    src/Users/Users.cc
    src/Alert/Alert.cc
    src/Chat/Chat.cc
    src/System/System.cc
)

# 3. LIBRERÍAS DEL SISTEMA
find_package(Threads REQUIRED)

# En Linux, SQLite suele necesitar 'dl' (dynamic loader) además de Threads
target_link_libraries(TutUCO PRIVATE Threads::Threads dl)

# 4. INTEGRACIÓN DE LOS TESTS (¡ESTO ES LO NUEVO!)
# Le dice a CMake que entre en la carpeta 'tests' y ejecute el CMakeLists.txt que hay allí
add_subdirectory(tests)