cmake_minimum_required(VERSION 3.11)

# 1. Descarga automática de GoogleTest (La forma moderna y segura)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Forzamos que se descargue y se haga disponible
FetchContent_MakeAvailable(googletest)

# 2. Habilitamos testing
enable_testing()

# 3. Definimos los archivos de TU código fuente (El que quieres probar)
# IMPORTANTE: Aquí NO pongas main.cc, porque los tests tienen su propio main.
# Añadimos Alert, System y Chat por si los usas en los otros tests.
set(SOURCE_FILES
    ../src/Users/Users.cc
    ../src/Database/DBManager.cc
    ../src/Alert/Alert.cc
    ../src/System/System.cc
    ../src/Chat/Chat.cc
)

# 4. Definimos los archivos de tus TESTS
set(TEST_FILES
    Users_test.cc
    Alert_test.cc   # Asegúrate de que este archivo existe o quita la línea
    System_test.cc  # Asegúrate de que este archivo existe o quita la línea
)

# 5. Creamos el ejecutable de pruebas uniendo tus tests + tu código fuente
add_executable(run_unit_tests 
    ${TEST_FILES} 
    ${SOURCE_FILES}
)

# 6. ENLAZADO DE LIBRERÍAS (CRÍTICO)
# - gtest_main: Proporciona el punto de entrada main() para los tests
# - sqlite3: NECESARIO porque DBManager usa sqlite3
# - pthread: Necesario para GTest en Linux
target_link_libraries(run_unit_tests 
    gtest_main 
    sqlite3 
    pthread
)

# 7. Registro para CTest
include(GoogleTest)
gtest_discover_tests(run_unit_tests)