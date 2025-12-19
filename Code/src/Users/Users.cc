#include "Users.h"
#include <iostream>
#include <vector>
#include "../Database/DBManager.h"

// Implementación del constructor
User::User(std::string id, std::string fn, std::string l1, std::string l2, 
           std::string m, std::string p, std::string r, std::string b) {
    this->id = id;
    this->firstName = fn;
    this->ln1 = l1;
    this->ln2 = l2;
    this->moodle = m;
    this->pass = p;
    this->role = r;
    this->birth = b;
}

// Implementación de la función para imprimir el perfil
void User::printProfile() const {
    std::cout << "========= PERFIL DE USUARIO =========" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "NOMBRE: " << firstName << " " << ln1 << " " << ln2 << std::endl;
    std::cout << "MOODLE: " << moodle << " | NACIMIENTO: " << birth << std::endl;
    std::cout << "=====================================" << std::endl;
}




void Coordinator::autoAssignStudents(DBManager& db, std::string currentDate) {
    // 1. Reiniciar la Bdd: Borra mensajes, alertas y asignaciones previas
    if (db.resetForNewAssignment()) {
        std::cout << ">> Base de datos reiniciada: se han borrado chats, alertas y asignaciones." << std::endl;
    } else {
        std::cout << ">> Error al intentar reiniciar la base de datos." << std::endl;
        return;
    }

    // 2. Obtener los listados de IDs por rol
    std::vector<std::string> students = db.getUserIdsByRole("STUDENT");
    std::vector<std::string> tutors = db.getUserIdsByRole("TUTOR");

    if (tutors.empty() || students.empty()) {
        std::cout << ">> Error: No hay suficientes usuarios para el reparto." << std::endl;
        return;
    }

    // 3. Reparto equilibrado (Round Robin)
    // El tutor asignado a cada alumno i se calcula como i mod n, donde n es el total de tutores
    for (size_t i = 0; i < students.size(); ++i) {
        std::string s_id = students[i];
        std::string t_id = tutors[i % tutors.size()]; // Reparto equilibrado (ej: 9 alumnos / 2 tutores = 5 y 4)
        
        db.insertAssignment(s_id, t_id, currentDate); 
    }

    std::cout << ">> Asignacion finalizada: " << students.size() << " alumnos repartidos entre " << tutors.size() << " tutores." << std::endl;
}

void Coordinator::manageAlerts(DBManager& db) {
    // Listar la tabla con todas las alertas
    db.showAlerts(); 
    
    int alertId;
    std::cout << "\nIntroduce el ID de la alerta para marcar como COMPLETADA (0 para cancelar): ";
    std::cin >> alertId;
    
    if (alertId > 0) {
        // Marcar la alerta como completada (status = 1)
        if (db.updateAlertStatus(alertId, "COMPLETED")) {
            std::cout << ">> Alerta marcada como completada con exito." << std::endl;
        } else {
            std::cout << ">> No se pudo actualizar el estado de la alerta." << std::endl;
        }
    }
}

void Coordinator::listUsers(DBManager& db) {
    // Listar la tabla con todos los usuarios
    db.showUsers(); 
}

void Coordinator::listAssignments(DBManager& db) {
    // Llama a la función de la base de datos que muestra la tabla Assignments
    db.showAssignments(); 
}