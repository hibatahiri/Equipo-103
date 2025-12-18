#include <iostream>
#include <vector>
#include <string>
#include "src/Database/DBManager.h"
#include "src/Users/Users.h"
#include "src/Alert/Alert.h"
#include "src/Chat/Chat.h"
#include "src/System/System.h"

int main() {
    DBManager db;

    std::cout << "=== INICIANDO PRUEBA DEL SISTEMA UCO ===" << std::endl;

    // 1. Conexión e Inicialización
    if (!db.openConnection()) {
        std::cerr << "Error: No se pudo abrir la base de datos." << std::endl;
        return 1;
    }
    db.initializeSchema();

    // 2. CREACIÓN DE USUARIOS
    // Estos datos se guardarán físicamente en la DB
    User tutor("T100", "Nicolas", "Garcia", "Pedrajas", "ngarcia", "admin123", "TUTOR", "1980-05-12");
    User alumno("A55", "Lucia", "Fernandez", "Perez", "lfernandez", "alum456", "STUDENT", "2004-02-28");

    std::cout << "\n> Insertando usuarios en la base de datos..." << std::endl;
    db.insertUser(tutor);
    db.insertUser(alumno);

    // 3. CREACIÓN DE UNA ALERTA
    Alert aviso("T100", "A55", "2025-12-18", "Nueva Practica", "Ya teneis subida la P3 a Moodle.");
    
    std::cout << "> Enviando alerta..." << std::endl;
    db.insertAlert(aviso);

    // 4. SIMULACIÓN DE CHAT
    Message m1("A55", "T100", "Hola profesor, ¿la entrega es el viernes?", "10:00:05");
    Message m2("T100", "A55", "Si Lucia, antes de las 23:59.", "10:05:12");
    Message m3("A55", "T100", "Perfecto, gracias.", "10:06:01");

    std::cout << "> Registrando mensajes de chat..." << std::endl;
    db.insertMessage(m1);
    db.insertMessage(m2);
    db.insertMessage(m3);

    // 5. PRUEBA DE LOGIN REA
    // El sistema buscará en la DB si el ID y la PASS coinciden
    SystemController system(db);
    std::string id_login, pass_login;

    std::cout << "\n--- SIMULACION DE INICIO DE SESION ---" << std::endl;
    std::cout << "Introduce ID de usuario (Prueba con T100): ";
    std::cin >> id_login;
    std::cout << "Introduce Password (Prueba con admin123): ";
    std::cin >> pass_login;

    if (system.login(id_login, pass_login)) {
        std::cout << "Acceso concedido como: " << system.getCurrentUser()->getFirstName() << std::endl;
        std::cout << "Rol del usuario: " << system.getCurrentUser()->getRole() << std::endl;
    } else {
        std::cout << "Acceso denegado. Credenciales incorrectas." << std::endl;
    }

    // 6. MOSTRAR RESULTADOS GENERALES
    std::cout << "\n--- ESTADO FINAL DE LA BASE DE DATOS ---" << std::endl;
    db.showUsers();
    db.showAlerts();
    
    // Ver el chat especifico entre los dos usuarios de prueba
    db.showMessages("A55", "T100");

    // 7. Cierre seguro de recursos
    db.closeConnection();
    std::cout << "\n=== PRUEBA FINALIZADA CORRECTAMENTE ===" << std::endl;

    return 0;
}