cmake_minimum_required(VERSION 3.10)
project(TutoringUCO)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. DIRECTORIOS DE INCLUSIÓN
# Esto permite que hagas #include "Users.h" sin rutas largas
include_directories(src/Database)
include_directories(src/Users)
include_directories(src/Alert)
include_directories(src/Chat)

# 2. DEFINICIÓN DEL EJECUTABLE
# Listamos todos los archivos .cc y el .c de sqlite para que se compilen juntos
add_executable(tutoring_app
    main.cc
    src/Database/DBManager.cc
    src/Database/sqlite3.c
    src/Users/Users.cc
    src/Alert/Alert.cc
    src/Chat/Chat.cc
    src/System/System.cc
)

# 3. LIBRERÍAS DEL SISTEMA
# SQLite requiere soporte de hilos en muchos sistemas
find_package(Threads REQUIRED)
target_link_libraries(tutoring_app PRIVATE Threads::Threads)