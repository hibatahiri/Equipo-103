#include "Users.h"

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
    std::cout << "ID: " << id << " (" << role << ")" << std::endl;
    std::cout << "NOMBRE: " << firstName << " " << ln1 << " " << ln2 << std::endl;
    std::cout << "MOODLE: " << moodle << " | NACIMIENTO: " << birth << std::endl;
    std::cout << "=====================================" << std::endl;
}