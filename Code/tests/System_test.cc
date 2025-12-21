#include <gtest/gtest.h>
#include "../src/System/System.h"
#include "../src/Database/DBManager.h"
#include "../src/Users/Users.h"

// Fixture (Configuración común) para no repetir código
class SystemTest : public ::testing::Test {
protected:
    DBManager db;
    SystemController* system;

    void SetUp() override {
        db.openConnection();
        db.initializeSchema();
        system = new SystemController(db);
        
        // Creamos un usuario de prueba SIEMPRE antes del test
        User u("SYS_USER", "Test", "System", "X", "usuario_sys", "pass123", "ADMIN", "2000");
        db.insertUser(u);
    }

    void TearDown() override {
        // Borramos el usuario y cerramos al acabar
        db.deleteUser("SYS_USER");
        delete system;
        db.closeConnection();
    }
};

// TEST 1: Login Exitoso (Usuario y contraseña correctos)
TEST_F(SystemTest, LoginSuccess) {
    // Intentamos entrar con los datos correctos
    bool result = system->login("usuario_sys", "pass123");

    // 1. Debe devolver true
    EXPECT_TRUE(result);

    // 2. El sistema debe tener un usuario cargado en memoria
    ASSERT_TRUE(system->getCurrentUser() != nullptr);
    EXPECT_EQ(system->getCurrentUser()->getId(), "SYS_USER");
}

// TEST 2: Login Fallido (Contraseña incorrecta)
TEST_F(SystemTest, LoginFailure) {
    // Intentamos entrar con contraseña MAL
    bool result = system->login("usuario_sys", "WRONG_PASS");

    // 1. Debe devolver false
    EXPECT_FALSE(result);

    // 2. El sistema NO debe tener usuario cargado
    EXPECT_TRUE(system->getCurrentUser() == nullptr);
}