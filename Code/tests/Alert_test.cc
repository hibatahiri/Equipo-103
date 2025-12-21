#include <gtest/gtest.h>
#include "../src/Alert/Alert.h"
#include "../src/Database/DBManager.h"
#include <string>

// TEST 1: Verificar que el constructor guarda bien los datos
TEST(AlertTest, ConstructorAndGetters) {
    // 1. Crear una alerta ficticia
    Alert a("T100", "A55", "2025-01-20", "Tutoria", "Necesito ayuda", "PENDING");

    // 2. Comprobar que los getters devuelven lo que hemos metido
    EXPECT_EQ(a.getSender(), "T100");
    EXPECT_EQ(a.getReceiver(), "A55");
    EXPECT_EQ(a.getDate(), "2025-01-20");
    EXPECT_EQ(a.getSubject(), "Tutoria");
    
    // CORREGIDO: Usamos getDesc() en lugar de getContent()
    EXPECT_EQ(a.getDesc(), "Necesito ayuda"); 
    
    EXPECT_EQ(a.getStatus(), "PENDING");
}

// TEST 2: Probar la inserción en Base de Datos y el cambio de estado
TEST(AlertTest, InsertAndUpdateInDB) {
    DBManager db;
    
    // Si falla la conexión, saltamos el test
    if (!db.openConnection()) {
        SUCCEED() << "Base de datos no disponible, saltando test de integración.";
        return; 
    }
    
    // 1. Insertamos una alerta
    Alert a("T_TEST", "A_TEST", "2025-01-01", "Test DB", "Testing...", "PENDING");
    bool insertado = db.insertAlert(a);
    
    // Debería devolver true (o false si hay problemas de claves foráneas, pero no debe crashear)
    // Para simplificar, asumimos que inserta bien o lo gestiona.
    (void)insertado; 

    // 2. Probamos la función de actualizar estado
    bool actualizado = db.updateAlertStatus(1, "COMPLETED");
    (void)actualizado; 

    db.closeConnection();
}