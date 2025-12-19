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
void mostrarMenuCoordinador(SystemController& system, DBManager& db); // Nuevo prototipo

int main() {
    DBManager db;
    
    // 1. Inicialización de la Base de Datos
    if (!db.openConnection()) {
        std::cerr << "Error crítico: No se pudo conectar con la base de datos." << std::endl;
        return 1;
    }
    db.initializeSchema();
    

    // --- SOLO PARA PRUEBAS ---
    // Tutores
    User t1("T100", "Nicolas", "Garcia", "Pedrajas", "ngarcia", "admin123", "TUTOR", "1980-05-12");
    User t2("T101", "Maria", "Gomez", "Ruiz", "mgomez", "tutor789", "TUTOR", "1985-10-20");

    // Alumnos
    User a1("A55", "Lucia", "Fernandez", "Perez", "lfernandez", "alum456", "STUDENT", "2004-02-28");
    User a2("A56", "Pablo", "Cano", "Ruiz", "pcano", "pablo2025", "STUDENT", "2003-05-15");
    User a3("A57", "Elena", "Marin", "Lara", "emarin", "elena99", "STUDENT", "2004-11-02");
    User a4("A58", "Jorge", "Saez", "Mota", "jsaez", "jorgeuco", "STUDENT", "2002-08-12");
    
    // Coordinador (Nueva cuenta de prueba)
    User c1("C1", "Fran", "Admin", "UCO", "fadmin", "coord123", "COORDINATOR", "1990-01-01");

    // Insertar en la Base de Datos
    db.insertUser(t1); db.insertUser(t2);
    db.insertUser(a1); db.insertUser(a2); db.insertUser(a3); db.insertUser(a4);
    db.insertUser(c1);
    // -------------------------

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

            if (system.login(id, pass)) {
                std::string rol = system.getCurrentUser()->getRole();
                
                // Nueva lógica de detección de rol
                if (rol == "COORDINATOR") {
                    mostrarMenuCoordinador(system, db);
                } else if (rol == "TUTOR" || rol == "ADMIN") {
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

// --- PANEL PARA EL ROL COORDINADOR ---
void mostrarMenuCoordinador(SystemController& system, DBManager& db) {
    int opt;
    // Hacemos el cast para tratar al usuario actual como Coordinador
    Coordinator* coord = static_cast<Coordinator*>(system.getCurrentUser());

    do {
std::cout << "\n--- PANEL DE COORDINADOR ---" << std::endl;
        std::cout << "1. Asignacion Automatica (Reinicia Sistema)" << std::endl;
        std::cout << "2. Listar y Gestionar Alertas" << std::endl;
        std::cout << "3. Listar todos los Usuarios" << std::endl;
        std::cout << "4. Ver Asignaciones" << std::endl;
        std::cout << "5. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: ";
        std::cin >> opt;

        switch(opt) {
            case 1:
                std::cout << "ATENCION: Se borraran chats y alertas existentes." << std::endl;
                coord->autoAssignStudents(db, system.getCurrentDateTime());
                break;
            case 2:
                coord->manageAlerts(db);
                break;
            case 3:
                coord->listUsers(db);
                break;
            case 4:
                coord->listAssignments(db);
                break;
        }
    } while (opt != 5);
}

// --- PANEL PARA EL ROL TUTOR ---
void mostrarMenuTutor(SystemController& system, DBManager& db) {
    int opt;
    std::string myId = system.getCurrentUser()->getId();

    do {
        std::cout << "\n--- PANEL DE CONTROL (TUTOR) ---" << std::endl;
        std::cout << "1. Listar MIS Alumnos" << std::endl;
        std::cout << "2. Enviar Alerta a Alumno" << std::endl;
        std::cout << "3. Ver MIS Alertas (Gestionar Estado)" << std::endl;
        std::cout << "4. Ver Chat con un Alumno" << std::endl;
        std::cout << "5. Enviar Mensaje Directo a Alumno" << std::endl; // Nueva opción
        std::cout << "6. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: db.showStudentsByTutor(myId); break;
            case 2: { // Enviar Alerta
                std::string receptor, asunto, msg;
                std::cout << "ID del Alumno: "; std::cin >> receptor;
                std::cout << "Asunto: "; std::cin.ignore(); std::getline(std::cin, asunto);
                std::cout << "Mensaje: "; std::getline(std::cin, msg);
                Alert a(myId, receptor, system.getCurrentDateTime(), asunto, msg, "PENDING");
                db.insertAlert(a);
                break;
            }
            case 3: db.showAlertsByTutor(myId); break;
            case 4: { // Ver Chat
                std::string studentId;
                std::cout << "ID del alumno: "; std::cin >> studentId;
                db.showMessages(myId, studentId);
                break;
            }
            case 5: { // Enviar Mensaje (Misma función que el alumno)
                std::string studentId, texto;
                std::cout << "ID del alumno receptor: "; std::cin >> studentId;
                std::cout << "Mensaje: "; std::cin.ignore(); std::getline(std::cin, texto);
                Message m(myId, studentId, texto, system.getCurrentDateTime());
                if(db.insertMessage(m)) std::cout << ">> Mensaje enviado." << std::endl;
                break;
            }
        }
    } while (opt != 6);
}

// --- PANEL PARA EL ROL ALUMNO ---
void mostrarMenuAlumno(SystemController& system, DBManager& db) {
    int opt;
    std::string myId = system.getCurrentUser()->getId();

    do {
        std::cout << "\n--- PANEL DE ALUMNO ---" << std::endl;
        std::cout << "1. Ver mis Alertas recibidas" << std::endl;
        std::cout << "2. Enviar Mensaje al Tutor" << std::endl;
        std::cout << "3. Ver Historial de Chat con Tutor" << std::endl;
        std::cout << "4. Enviar ALERTA al Tutor" << std::endl; // Nueva opción
        std::cout << "5. Ver mis Datos de Perfil" << std::endl;
        std::cout << "6. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: db.showAlertsForUser(myId); break;
            case 2: { // Enviar Mensaje
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) {
                    std::string texto;
                    std::cout << "Mensaje: "; std::cin.ignore(); std::getline(std::cin, texto);
                    Message m(myId, tutorId, texto, system.getCurrentDateTime());
                    db.insertMessage(m);
                }
                break;
            }
            case 3: { // Ver Chat
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) db.showMessages(myId, tutorId);
                break;
            }
            case 4: { // Enviar Alerta
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) {
                    std::string asunto, desc;
                    std::cout << "Asunto de la Alerta: "; std::cin.ignore(); std::getline(std::cin, asunto);
                    std::cout << "Descripcion: "; std::getline(std::cin, desc);
                    // El alumno envía la alerta, pero nace como 'PENDING'
                    Alert a(myId, tutorId, system.getCurrentDateTime(), asunto, desc, "PENDING");
                    if(db.insertAlert(a)) std::cout << ">> Alerta enviada al tutor." << std::endl;
                }
                break;
            }
            case 5: system.getCurrentUser()->printProfile(); break;
        }
    } while (opt != 6);
}