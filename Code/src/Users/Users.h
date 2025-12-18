#ifndef USERS_H
#define USERS_H

#include <string>
#include <iostream>

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

#endif