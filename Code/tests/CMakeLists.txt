# Buscamos el paquete de Google Test en el sistema
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Creamos el ejecutable para las pruebas unitarias
# Es importante incluir los archivos de src que queremos probar
add_executable(run_unit_tests 
    Users_test.cc 
    ../src/Users/Users.cc 
    ../src/Database/DBManager.cc
)

# Enlazamos con las librerías de Google Test y los hilos del sistema
target_link_libraries(run_unit_tests 
    GTest::GTest 
    GTest::Main 
    pthread
)

# Registramos el test para poder ejecutarlo con el comando 'ctest'
add_test(NAME AllUnitTests COMMAND run_unit_tests)