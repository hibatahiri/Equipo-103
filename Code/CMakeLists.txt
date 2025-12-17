cmake_minimum_required(VERSION 3.10)
project(UCO_Tutoring_System)

# Usamos el estándar C++17
set(CMAKE_CXX_STANDARD 17)

# 1. Directorios de cabeceras: para que el compilador encuentre los archivos .h
include_directories(
    src/Users 
    src/System 
    src/Alert 
    src/Chat 
    src/Database
)

# 2. Buscamos todos los archivos de código fuente (.cc y el motor de sqlite .c)
# El asterisco busca automáticamente en todas las subcarpetas de src
file(GLOB_RECURSE SOURCES 
    "src/*.cc" 
    "src/*.c" 
    "main.cc"
)

# 3. Creamos el ejecutable principal del sistema
add_executable(tutoring_app ${SOURCES})

# 4. Bibliotecas necesarias para que SQLite funcione en Linux (UCO)
if(UNIX)
    target_link_libraries(tutoring_app dl pthread)
endif()

# 5. Configuración de los Tests: le decimos que mire en la carpeta 'tests'
#enable_testing()
#add_subdirectory(tests)