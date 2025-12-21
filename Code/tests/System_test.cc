#include <gtest/gtest.h>
#include "../src/System/System.h"
#include "../src/Database/DBManager.h"
#include "../src/Users/Users.h"

class SystemTest : public ::testing::Test {
protected:
    DBManager db;
    SystemController* system;

    void SetUp() override {
        db.openConnection();
        db.initializeSchema();
        system = new SystemController(db);
        
        User u("SYS_USER", "Test", "System", "X", "usuario_sys", "pass123", "ADMIN", "2000");
        db.insertUser(u);
    }

    void TearDown() override {
        db.deleteUser("SYS_USER");
        delete system;
        db.closeConnection();
    }
};

TEST_F(SystemTest, LoginSuccess) {
    bool result = system->login("usuario_sys", "pass123");

    EXPECT_TRUE(result);
    ASSERT_TRUE(system->getCurrentUser() != nullptr);
    EXPECT_EQ(system->getCurrentUser()->getId(), "SYS_USER");
}

TEST_F(SystemTest, LoginFailure) {
    bool result = system->login("usuario_sys", "WRONG_PASS");

    EXPECT_FALSE(result);
    EXPECT_TRUE(system->getCurrentUser() == nullptr);
}