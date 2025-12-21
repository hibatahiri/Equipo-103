#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // system
#include <thread>  // sleep
#include <chrono>  // Para medir tiempo
#include "src/Database/DBManager.h"
#include "src/Users/Users.h"
#include "src/Alert/Alert.h"
#include "src/Chat/Chat.h"
#include "src/System/System.h"

// Prototipos
void mostrarMenuTutor(SystemController& system, DBManager& db);
void mostrarMenuAlumno(SystemController& system, DBManager& db);
void mostrarMenuCoordinador(SystemController& system, DBManager& db);

int main() {
    DBManager db;
    
    // 1. Inicialización de la Base de Datos
    if (!db.openConnection()) {
        std::cerr << "Error crítico: No se pudo conectar con la base de datos." << std::endl;
        return 1;
    }
    db.initializeSchema();
    

    // ==========================================
    //        ZONA DE DATOS DE PRUEBA
    // ==========================================
    
    // --- 1. CREACIÓN DE USUARIOS ---
    User t1("T100", "Nicolas", "Garcia", "Pedrajas", "ngarcia", "admin123", "TUTOR", "1980-05-12");
    User t2("T101", "Maria", "Gomez", "Ruiz", "mgomez", "tutor789", "TUTOR", "1985-10-20");

    User a1("A55", "Lucia", "Fernandez", "Perez", "lfernandez", "alum456", "STUDENT", "2004-02-28");
    User a2("A56", "Pablo", "Cano", "Ruiz", "pcano", "pablo2025", "STUDENT", "2003-05-15");
    User a3("A57", "Elena", "Marin", "Lara", "emarin", "elena99", "STUDENT", "2004-11-02");
    User a4("A58", "Jorge", "Saez", "Mota", "jsaez", "jorgeuco", "STUDENT", "2002-08-12");
    
    User c1("C1", "Fran", "Admin", "UCO", "fadmin", "coord123", "COORDINATOR", "1990-01-01");

    db.insertUser(t1); db.insertUser(t2);
    db.insertUser(a1); db.insertUser(a2); db.insertUser(a3); db.insertUser(a4);
    db.insertUser(c1);

    // --- 2. ASIGNACIONES (Relaciones Alumno-Tutor) ---
    // Nicolás (T100) tendrá a Lucía y Pablo
    db.insertAssignment("A55", "T100", "2025-01-01");
    db.insertAssignment("A56", "T100", "2025-01-01");
    
    // María (T101) tendrá a Elena y Jorge
    db.insertAssignment("A57", "T101", "2025-01-01");
    db.insertAssignment("A58", "T101", "2025-01-01");

    // --- 3. MENSAJES DE CHAT (Prueba Chat) ---
    // Conversación entre Lucía (A55) y Nicolás (T100)
    Message m1("A55", "T100", "Hola Nicolas, tengo una duda con la practica.", "2025-01-10 10:00");
    Message m2("T100", "A55", "Dime Lucia, ¿en que ejercicio?", "2025-01-10 10:05");
    Message m3("A55", "T100", "En el de punteros, me da error de segmento.", "2025-01-10 10:07");
    
    db.insertMessage(m1);
    db.insertMessage(m2);
    db.insertMessage(m3);

    // --- 4. ALERTAS (Prueba Estado PENDING/COMPLETED) ---
    // Alerta 1: Nicolás avisa a Pablo (Faltas) -> PENDING
    Alert al1("T100", "A56", "2025-01-15", "Faltas de Asistencia", "Pablo, llevas 3 faltas injustificadas.", "PENDING");
    
    // Alerta 2: Elena pide tutoría a María -> PENDING
    Alert al2("A57", "T101", "2025-01-16", "Solicitud Tutoria", "Necesito revisar el examen parcial.", "PENDING");
    
    // Alerta 3: Una alerta vieja que ya está cerrada -> COMPLETED
    Alert al3("T101", "A58", "2025-01-12", "Entrega Practica 1", "Practica recibida correctamente.", "COMPLETED");

    db.insertAlert(al1);
    db.insertAlert(al2);
    db.insertAlert(al3);

    // ==========================================

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
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::system("clear");
    return 0;
}

// ---------------------------------------------------------
//        FUNCIONES DE MENÚ (Ya corregidas)
// ---------------------------------------------------------

void mostrarMenuCoordinador(SystemController& system, DBManager& db) {
    int opt;
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
            case 2: coord->manageAlerts(db); break;
            case 3: coord->listUsers(db); break;
            case 4: coord->listAssignments(db); break;
        }
    } while (opt != 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::system("clear");
}

void mostrarMenuTutor(SystemController& system, DBManager& db) {
    int opt;
    std::string myId = system.getCurrentUser()->getId();

    do {
        std::cout << "\n--- PANEL DE CONTROL (TUTOR) ---" << std::endl;
        std::cout << "1. Listar MIS Alumnos" << std::endl;
        std::cout << "2. Enviar Alerta a Alumno" << std::endl;
        std::cout << "3. Ver MIS Alertas (Gestionar Estado)" << std::endl;
        std::cout << "4. Ver Chat con un Alumno" << std::endl;
        std::cout << "5. Enviar Mensaje Directo a Alumno" << std::endl;
        std::cout << "6. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: db.showStudentsByTutor(myId); break;
            case 2: { 
                std::string receptor, asunto, msg;
                std::cout << "ID del Alumno: "; std::cin >> receptor;
                std::cout << "Asunto: "; std::cin.ignore(); std::getline(std::cin, asunto);
                std::cout << "Mensaje: "; std::getline(std::cin, msg);
                Alert a(myId, receptor, system.getCurrentDateTime(), asunto, msg, "PENDING");
                db.insertAlert(a);
                break;
            }
            case 3: { 
                db.showAlertsByTutor(myId); 
                char respuesta;
                std::cout << "\n¿Deseas marcar alguna alerta como COMPLETADA? (s/n): ";
                std::cin >> respuesta;

                if (respuesta == 's' || respuesta == 'S') {
                    int alertId;
                    std::cout << "Introduce el ID de la alerta a cerrar: ";
                    std::cin >> alertId;
                    if (db.updateAlertStatus(alertId, "COMPLETED")) {
                        std::cout << "Estado actualizado." << std::endl;
                    } else {
                        std::cout << "Error al actualizar." << std::endl;
                    }
                }
                break;
            }
            case 4: { 
                std::string studentId;
                std::cout << "ID del alumno: "; std::cin >> studentId;
                db.showMessages(myId, studentId);
                break;
            }
            case 5: { 
                std::string studentId, texto;
                std::cout << "ID del alumno receptor: "; std::cin >> studentId;
                std::cout << "Mensaje: "; std::cin.ignore(); std::getline(std::cin, texto);
                Message m(myId, studentId, texto, system.getCurrentDateTime());
                if(db.insertMessage(m)) std::cout << ">> Mensaje enviado." << std::endl;
                break;
            }
        }
    } while (opt != 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::system("clear");
}

void mostrarMenuAlumno(SystemController& system, DBManager& db) {
    int opt;
    std::string myId = system.getCurrentUser()->getId();

    do {
        std::cout << "\n--- PANEL DE ALUMNO ---" << std::endl;
        std::cout << "1. Ver mis Alertas recibidas" << std::endl;
        std::cout << "2. Enviar Mensaje al Tutor" << std::endl;
        std::cout << "3. Ver Historial de Chat con Tutor" << std::endl;
        std::cout << "4. Enviar ALERTA al Tutor" << std::endl;
        std::cout << "5. Ver mis Datos de Perfil" << std::endl;
        std::cout << "6. Cerrar Sesion" << std::endl;
        std::cout << "Opcion: "; 
        std::cin >> opt;

        switch(opt) {
            case 1: db.showAlertsForUser(myId); break;
            case 2: { 
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) {
                    std::string texto;
                    std::cout << "Mensaje: "; std::cin.ignore(); std::getline(std::cin, texto);
                    Message m(myId, tutorId, texto, system.getCurrentDateTime());
                    db.insertMessage(m);
                } else {
                    std::cout << "No tienes tutor asignado." << std::endl;
                }
                break;
            }
            case 3: { 
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) db.showMessages(myId, tutorId);
                break;
            }
            case 4: { 
                std::string tutorId = db.getTutorIdByStudent(myId);
                if (!tutorId.empty()) {
                    std::string asunto, desc;
                    std::cout << "Asunto: "; std::cin.ignore(); std::getline(std::cin, asunto);
                    std::cout << "Descripcion: "; std::getline(std::cin, desc);
                    Alert a(myId, tutorId, system.getCurrentDateTime(), asunto, desc, "PENDING");
                    if(db.insertAlert(a)) std::cout << ">> Alerta enviada." << std::endl;
                }
                break;
            }
            case 5: system.getCurrentUser()->printProfile(); break;
        }
    } while (opt != 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::system("clear");
}