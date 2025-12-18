#ifndef ALERT_H
#define ALERT_H

#include <string>
#include <iostream>

class Alert {
private:
    int id; // ID autoincremental de la base de datos
    std::string senderId;
    std::string receiverId;
    std::string date;
    std::string subject;
    std::string description;
    std::string status;

public:
    // Constructor para crear alertas nuevas (el ID lo pone la DB)
    Alert(std::string sender, std::string receiver, std::string date, 
          std::string subject, std::string desc, std::string status = "PENDING");

    // Getters para que DBManager pueda leer los datos
    std::string getSender() const { return senderId; }
    std::string getReceiver() const { return receiverId; }
    std::string getDate() const { return date; }
    std::string getSubject() const { return subject; }
    std::string getDesc() const { return description; }
    std::string getStatus() const { return status; }

    // Función para imprimir la alerta de forma bonita en el terminal
    void printAlert() const;
};

#endif