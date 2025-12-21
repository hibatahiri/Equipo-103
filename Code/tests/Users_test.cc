#include <gtest/gtest.h>
#include "../src/Users/Users.h"
#include "../src/Database/DBManager.h"

TEST(UsersTest, ConstructorAndGetters) {
    User u("U_TEST_01", "Test", "User", "Garcia", "u_test", "1234", "STUDENT", "2000-01-01");
    
    EXPECT_EQ(u.getId(), "U_TEST_01");
    EXPECT_EQ(u.getFirstName(), "Test");
    EXPECT_EQ(u.getRole(), "STUDENT");
    EXPECT_EQ(u.getPass(), "1234");
}

TEST(UsersTest, InsertUserInDB) {
    DBManager db;
    ASSERT_TRUE(db.openConnection()); 
    db.initializeSchema(); 
    
    User u("U_DB_01", "Juan", "Base", "Datos", "juan_db", "pass123", "TUTOR", "1980-05-20");
    
    EXPECT_TRUE(db.insertUser(u)); 
    
    db.deleteUser("U_DB_01");
    db.closeConnection();
}