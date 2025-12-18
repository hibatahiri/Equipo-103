#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>
#include "../Database/DBManager.h"
#include "../Users/Users.h"

class SystemController {
private:
    DBManager& db; // Referencia a la base de datos para no abrir mil conexiones
    User* currentUser; // Puntero al usuario que ha iniciado sesión

public:
    SystemController(DBManager& dbManager);
    
    // Función de Login: Comprueba si el usuario y pass existen en la DB
    bool login(std::string id, std::string password);
    
    // Logout
    void logout();

    // Utilidad: Devuelve la fecha/hora actual formateada para SQLite (YYYY-MM-DD HH:MM:SS)
    static std::string getCurrentDateTime();

    // Getter para saber quién está usando el programa
    User* getCurrentUser() { return currentUser; }
};

#endif