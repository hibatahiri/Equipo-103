#include "Alert.h"

// Constructor: Inicializa todos los atributos de la alerta
Alert::Alert(std::string sender, std::string receiver, std::string date, 
             std::string subject, std::string desc, std::string status) {
    this->senderId = sender;
    this->receiverId = receiver;
    this->date = date;
    this->subject = subject;
    this->description = desc;
    this->status = status;
}

// Muestra la información de la alerta por pantalla
void Alert::printAlert() const {
    std::cout << "------------------------------------" << std::endl;
    std::cout << "ASUNTO: " << subject << " [" << status << "]" << std::endl;
    std::cout << "DE: " << senderId << " PARA: " << receiverId << " FECHA: " << date << std::endl;
    std::cout << "MENSAJE: " << description << std::endl;
    std::cout << "------------------------------------" << std::endl;
}