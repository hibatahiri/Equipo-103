#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "sqlite3.h"
#include "../Users/Users.h"
#include "../Chat/Chat.h"
#include "../Alert/Alert.h"
#include <string>
#include <vector>

class DBManager {
private:
    sqlite3* db; // Puntero interno para la gestión de SQLite

public:
    DBManager();
    ~DBManager();

    // Abre el archivo físico de la base de datos (tutoring.db)
    bool openConnection();
    
    // Cierra la conexión y libera los recursos
    void closeConnection();

    // Crea las tablas del sistema si no existen todavía
    void initializeSchema();

    // Devuelve un objeto User si las credenciales son correctas, si no, devuelve nullptr
    User* authenticateUser(std::string id, std::string pass);
    
    // Funciones de insercion, eliminacion y mostrar para Users
    bool insertUser(const User& u);
    bool deleteUser(std::string id);
    void showUsers();

    // Funciones de insercion, eliminacion y mostrar para Message
    bool insertMessage(const Message& m);
    bool deleteMessage(int id);
    void showMessages(std::string id1, std::string id2);


    // Funciones de insercion, eliminacion y mostrar para Alert
    bool insertAlert(const Alert& a);
    bool deleteAlert(int id);
    void showAlerts();

    // Funciones de insercion, eliminacion y mostrar para Assignment
    bool insertAssignment(std::string studentId, std::string tutorId, std::string date);
    bool deleteAssignment(std::string studentId);
    void showAssignments();
};

#endif