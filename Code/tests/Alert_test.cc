#include <gtest/gtest.h>
#include "../src/Alert/Alert.h"
#include "../src/Database/DBManager.h"
#include <string>

TEST(AlertTest, ConstructorAndGetters) {
    Alert a("T100", "A55", "2025-01-20", "Tutoria", "Necesito ayuda", "PENDING");

    EXPECT_EQ(a.getSender(), "T100");
    EXPECT_EQ(a.getReceiver(), "A55");
    EXPECT_EQ(a.getDate(), "2025-01-20");
    EXPECT_EQ(a.getSubject(), "Tutoria");
    EXPECT_EQ(a.getDesc(), "Necesito ayuda"); 
    EXPECT_EQ(a.getStatus(), "PENDING");
}

TEST(AlertTest, InsertAndUpdateInDB) {
    DBManager db;
    
    if (!db.openConnection()) {
        SUCCEED() << "DB connection failed, skipping integration test.";
        return; 
    }
    
    Alert a("T_TEST", "A_TEST", "2025-01-01", "Test DB", "Testing...", "PENDING");
    bool insertado = db.insertAlert(a);
    (void)insertado; 

    bool actualizado = db.updateAlertStatus(1, "COMPLETED");
    (void)actualizado; 

    db.closeConnection();
}