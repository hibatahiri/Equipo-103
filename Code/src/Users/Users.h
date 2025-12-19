#ifndef USERS_H
#define USERS_H

#include <iostream>
#include <string>
#include <vector>

class DBManager;

class User {
private:
    std::string id;
    std::string firstName;
    std::string ln1;
    std::string ln2;
    std::string moodle;
    std::string pass;
    std::string role;
    std::string birth;

public:
    // Constructor completo para inicializar todos los atributos
    User(std::string id, std::string fn, std::string l1, std::string l2, 
         std::string m, std::string p, std::string r, std::string b);

    // Getters públicos: necesarios para que DBManager pueda guardar los datos en SQL
    std::string getId() const { return id; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName1() const { return ln1; }
    std::string getLastName2() const { return ln2; }
    std::string getMoodle() const { return moodle; }
    std::string getPass() const { return pass; }
    std::string getRole() const { return role; }
    std::string getBirth() const { return birth; }

    // Función útil para mostrar el perfil del usuario por consola
    void printProfile() const;
};


class Coordinator : public User {
public:
    // Constructor que hereda el de la clase base User
    Coordinator(std::string id, std::string fn, std::string ln1, std::string ln2, 
                std::string m, std::string p, std::string r, std::string b)
        : User(id, fn, ln1, ln2, m, p, r, b) {}

    // 1. Asignación automática y reinicio
    void autoAssignStudents(DBManager& db, std::string currentDate);

    // 2. Gestión de Alertas
    void manageAlerts(DBManager& db);

    // 3. Listar todos los usuarios
    void listUsers(DBManager& db);

    // 4. Listar las relaciones Alumno-Tutor
    void listAssignments(DBManager& db);
};

#endif