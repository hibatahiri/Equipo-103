#include <iostream>
#include <string>
#include <vector>
#include "src/Database/DBManager.h"
#include "src/Users/Users.h"
#include "src/Alert/Alert.h"
#include "src/Chat/Chat.h"
#include "src/System/System.h"

// Prototipos de funciones para organizar el código
void mostrarMenuTutor(SystemController& system, DBManager& db);
void mostrarMenuAlumno(SystemController& system, DBManager& db);

int main() {
    DBManager db;
    
    // 1. Inicialización de la Base de Datos
    if (!db.openConnection()) {
        std::cerr << "Error crítico: No se pudo conectar con la base de datos." << std::endl;
        return 1;
    }
    db.initializeSchema();
    

    // --- SOLO PARA PRUEBAS ESTO NO DEBE ESTAR EN LA VERSION FINAL----------------------------------------------------------------------------------------------------
    // --- CARGA DE DATOS DE PRUEBA (2 TUTORES Y 4 ALUMNOS) ---
    
    // Tutores
    User t1("T100", "Nicolas", "Garcia", "Pedrajas", "ngarcia", "admin123", "TUTOR", "1980-05-12");
    User t2("T101", "Maria", "Gomez", "Ruiz", "mgomez", "tutor789", "TUTOR", "1985-10-20");

    // Alumnos
    User a1("A55", "Lucia", "Fernandez", "Perez", "lfernandez", "alum456", "STUDENT", "2004-02-28");
    User a2("A56", "Pablo", "Cano", "Ruiz", "pcano", "pablo2025", "STUDENT", "2003-05-15");
    User a3("A57", "Elena", "Marin", "Lara", "emarin", "elena99", "STUDENT", "2004-11-02");
    User a4("A58", "Jorge", "Saez", "Mota", "jsaez", "jorgeuco", "STUDENT", "2002-08-12");

    // Insertar en la Base de Datos
    db.insertUser(t1); db.insertUser(t2);
    db.insertUser(a1); db.insertUser(a2); db.insertUser(a3); db.insertUser(a4);
    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------

    SystemController system(db);
    int opcionPrincipal = 0;

    while (true) {
        std::cout << "\n====================================" << std::endl;
        std::cout << "   SISTEMA DE TUTORIAS UCO - 2025   " << std::endl;
        std::cout << "====================================" << std::endl;
        std::cout << "1. Iniciar Sesion" << std::endl;
        std::cout << "2. Salir" << std::endl;
        std::cout << "Opcion: ";
        std::cin >> opcionPrincipal;

        if (opcionPrincipal == 2) break;

        if (opcionPrincipal == 1) {
            std::string id, pass;
            std::cout << "\nUsuario: "; 
            std::cin >> id;
            std::cout << "Contrasena: "; 
            std::cin >> pass;

            // 2. Autenticación Real contra la DB
            if (system.login(id, pass)) {
                std::string rol = system.getCurrentUser()->getRole();
                
                if (rol == "TUTOR" || rol == "ADMIN") {
                    mostrarMenuTutor(system, db);
                } else {
                    mostrarMenuAlumno(system, db);
                }
                system.logout();
            }
        }
    }

    db.closeConnection();
    std::cout << "Saliendo del sistema de forma segura..." << std::endl;
    return 0;
}

// --- PANEL PARA EL ROL TUTOR ---
void mostrarMenuTutor(SystemController& system, DBManager& db) {
    int opt;
    do {
        std::cout << "\n--- PANEL DE CONTROL (TUTOR) ---" << std::endl;
        std::cout << "1. Listar todos los Alumnos" << std::endl;
        std::cout << "2. Enviar Alerta a Alumno" << std::endl;
        std::cout << "3. Ver Chat con un Alumno" << std::endl;
        std::cout << "4. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: 
                db.showUsers(); 
                break;
            case 2: {
                std::string receptor, asunto, msg;
                std::cout << "ID del Alumno receptor: "; 
                std::cin >> receptor;
                std::cout << "Asunto de la alerta: "; 
                std::cin.ignore(); // Limpiar buffer
                std::getline(std::cin, asunto);
                std::cout << "Cuerpo del mensaje: "; 
                std::getline(std::cin, msg);
                
                Alert a(system.getCurrentUser()->getId(), receptor, system.getCurrentDateTime(), asunto, msg);
                if(db.insertAlert(a)) std::cout << "Alerta guardada en DB." << std::endl;
                break;
            }
            case 3: {
                std::string studentId;
                std::cout << "Introduce el ID del alumno para cargar el chat: ";
                std::cin >> studentId;
                // Uso de tu función personalizada
                db.showMessages(system.getCurrentUser()->getId(), studentId); 
                break;
            }
        }
    } while (opt != 4);
}

// --- PANEL PARA EL ROL ALUMNO ---
void mostrarMenuAlumno(SystemController& system, DBManager& db) {
    int opt;
    do {
        std::cout << "\n--- PANEL DE ALUMNO ---" << std::endl;
        std::cout << "1. Ver mis Alertas recibidas" << std::endl;
        std::cout << "2. Enviar Mensaje al Tutor" << std::endl;
        std::cout << "3. Ver Historial de Chat con Tutor" << std::endl;
        std::cout << "4. Ver mis Datos de Perfil" << std::endl;
        std::cout << "5. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: 
                db.showAlerts(); 
                break;
            case 2: {
                std::string tutorId, texto;
                std::cout << "ID del Tutor: "; 
                std::cin >> tutorId;
                std::cout << "Escribe tu mensaje: "; 
                std::cin.ignore(); 
                std::getline(std::cin, texto);
                
                Message m(system.getCurrentUser()->getId(), tutorId, texto, system.getCurrentDateTime());
                if(db.insertMessage(m)) std::cout << "Mensaje enviado." << std::endl;
                break;
            }
            case 3: {
                std::string tutorId;
                std::cout << "ID del Tutor para ver conversacion: ";
                std::cin >> tutorId;
                db.showMessages(system.getCurrentUser()->getId(), tutorId);
                break;
            }
            case 4: 
                system.getCurrentUser()->printProfile(); 
                break;
        }
    } while (opt != 5);
}