#include "Chat.h"

// Implementación del constructor
Message::Message(std::string sender, std::string receiver, std::string text, std::string time) {
    this->senderId = sender;
    this->receiverId = receiver;
    this->content = text;
    this->timestamp = time;
}

// Implementación de la visualización del mensaje
void Message::printMessage() const {
    std::cout << "[" << timestamp << "] " << senderId << " dice: " << content << std::endl;
}