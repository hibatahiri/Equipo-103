#ifndef CHAT_H
#define CHAT_H

#include <string>
#include <iostream>

class Message {
private:
    std::string senderId;
    std::string receiverId;
    std::string content;
    std::string timestamp;

public:
    // Constructor para inicializar el mensaje
    Message(std::string sender, std::string receiver, std::string text, std::string time);

    // Getters para que DBManager pueda leer los datos
    std::string getSender() const { return senderId; }
    std::string getReceiver() const { return receiverId; }
    std::string getText() const { return content; }
    std::string getTime() const { return timestamp; }

    // Función para mostrar el mensaje con formato en consola
    void printMessage() const;
};

#endif