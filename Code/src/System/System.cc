#include "System.h"
#include <ctime>

SystemController::SystemController(DBManager& dbManager) : db(dbManager) {
    currentUser = nullptr; // Al empezar nadie está logueado
}

// Lógica de Login
bool SystemController::login(std::string id, std::string password) {
    User* userFound = db.authenticateUser(id, password);
    
    if (userFound != nullptr) {
        currentUser = userFound; // Guardamos quién ha entrado en la sesión
        std::cout << "¡Bienvenido/a, " << currentUser->getFirstName() << "!" << std::endl;
        return true;
    }
    
    std::cout << "Error: ID o contraseña incorrectos." << std::endl;
    return false;
}

void SystemController::logout() {
    currentUser = nullptr;
}

// Función para obtener el tiempo real del sistema
std::string SystemController::getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    // Formato ISO 8601 compatible con SQLite
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
}