#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "sqlite3.h"
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

    // Funciones de insercion, eliminacion y mostrar para Users
    bool insertUser(std::string id, std::string fn, std::string ln1, std::string ln2, std::string moodle, std::string pass, std::string role, std::string birth);
    bool deleteUser(std::string id);
    void showUsers();

    // Funciones de insercion, eliminacion y mostrar para Alert
    bool insertAlert(std::string sender, std::string receiver, std::string date, std::string subject, std::string desc);
    bool deleteAlert(int id);
    void showAlerts();

    // Funciones de insercion, eliminacion y mostrar para Assignment
    bool insertAssignment(std::string studentId, std::string tutorId, std::string date);
    bool deleteAssignment(std::string studentId);
    void showAssignments();
};

#endif