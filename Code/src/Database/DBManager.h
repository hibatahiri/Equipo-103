#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "sqlite3.h"
#include "../Users/Users.h"
#include "../Chat/Chat.h"
#include "../Alert/Alert.h"
#include <string>
#include <vector>

class DBManager {
private:
    sqlite3* db;

public:
    DBManager();
    ~DBManager();

    bool openConnection();
    void closeConnection();
    void initializeSchema();

    User* authenticateUser(std::string id, std::string pass);

    // User Management
    bool insertUser(const User& u);
    bool deleteUser(std::string id);
    void showUsers();

    // Message Management
    bool insertMessage(const Message& m);
    bool deleteMessage(int id);
    void showMessages(std::string id1, std::string id2);

    // Alert Management
    bool insertAlert(const Alert& a);
    bool deleteAlert(int id);
    void showAlerts();
    bool updateAlertStatus(int alertId, std::string status);
    void showAlertsForUser(std::string userId);
    void showAlertsByTutor(std::string tutorId);

    // Assignment Management
    bool insertAssignment(std::string studentId, std::string tutorId, std::string date);
    bool deleteAssignment(std::string studentId);
    void showAssignments();
    bool resetForNewAssignment();
    
    // Utilities
    std::vector<std::string> getUserIdsByRole(std::string role);
    void showStudentsByTutor(std::string tutorId);
    std::string getTutorIdByStudent(std::string studentId);
};

#endif