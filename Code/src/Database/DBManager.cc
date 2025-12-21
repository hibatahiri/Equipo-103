#include "DBManager.h"
#include <iostream>
#include <iomanip>

DBManager::DBManager() {
    db = nullptr;
}

DBManager::~DBManager() {
    closeConnection();
}

bool DBManager::openConnection() {
    int result = sqlite3_open("data/tutoring.db", &db);
    
    if (result != SQLITE_OK) {
        std::cerr << "Error al abrir la base de datos" << std::endl;
        return false;
    }
    return true;
}

void DBManager::closeConnection() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void DBManager::initializeSchema() {
    std::string sqlUsers = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "id TEXT PRIMARY KEY, "
        "first_name TEXT NOT NULL, "
        "last_name1 TEXT NOT NULL, "
        "last_name2 TEXT NOT NULL, "
        "moodle_user TEXT UNIQUE, "
        "password TEXT NOT NULL, "
        "role TEXT NOT NULL, "
        "birth_date TEXT);";

    std::string sqlMessages = 
        "CREATE TABLE IF NOT EXISTS Messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id TEXT NOT NULL, "
        "receiver_id TEXT NOT NULL, "
        "content TEXT NOT NULL, "
        "timestamp TEXT NOT NULL, "
        "FOREIGN KEY(sender_id) REFERENCES Users(id), "
        "FOREIGN KEY(receiver_id) REFERENCES Users(id));";

    std::string sqlAlerts = 
        "CREATE TABLE IF NOT EXISTS Alerts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id TEXT, "
        "receiver_id TEXT, "
        "date TEXT, "
        "subject TEXT, "
        "description TEXT, "
        "status TEXT DEFAULT 'PENDING', "
        "FOREIGN KEY(sender_id) REFERENCES Users(id), "
        "FOREIGN KEY(receiver_id) REFERENCES Users(id));";

    std::string sqlAssignments = 
        "CREATE TABLE IF NOT EXISTS Assignments ("
        "student_id TEXT PRIMARY KEY, "
        "tutor_id TEXT, "
        "assignment_date TEXT, "
        "FOREIGN KEY(student_id) REFERENCES Users(id), "
        "FOREIGN KEY(tutor_id) REFERENCES Users(id));";

    char* errMsg = nullptr;
    sqlite3_exec(db, sqlUsers.c_str(), nullptr, 0, &errMsg);
    sqlite3_exec(db, sqlMessages.c_str(), nullptr, 0, nullptr);
    sqlite3_exec(db, sqlAlerts.c_str(), nullptr, 0, &errMsg);
    sqlite3_exec(db, sqlAssignments.c_str(), nullptr, 0, &errMsg);

    if (errMsg) {
        sqlite3_free(errMsg);
    }
}

static int callback(void* data, int argc, char** argv, char** azColName) {
    bool* isFirstRow = (bool*)data;

    auto getWidth = [](std::string colName) {
        if (colName == "id") return 20; 
        if (colName == "Emisor" || colName == "Receptor") return 22; 
        if (colName == "date" || colName == "timestamp") return 22;
        if (colName == "subject") return 30;
        if (colName == "description" || colName == "content") return 50;
        if (colName == "status") return 15;
        return 20; 
    };

    if (*isFirstRow) {
        std::cout << "\n";
        int totalLength = 0;
        for (int i = 0; i < argc; i++) {
            int w = getWidth(azColName[i]);
            std::cout << std::left << std::setw(w) << azColName[i];
            totalLength += w;
        }
        std::cout << "\n" << std::string(totalLength, '-') << std::endl;
        *isFirstRow = false;
    }

    for (int i = 0; i < argc; i++) {
        int w = getWidth(azColName[i]);
        std::string val = (argv[i] ? argv[i] : "NULL");
        if (val.length() > (w - 2)) val = val.substr(0, w - 5) + "...";
        std::cout << std::left << std::setw(w) << val;
    }
    std::cout << std::endl;
    return 0;
}

struct LoginResult {
    User* user = nullptr;
};

static int loginCallback(void* data, int argc, char** argv, char** azColName) {
    LoginResult* result = static_cast<LoginResult*>(data);
    if (argc > 0) {
        result->user = new User(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    }
    return 0;
}

User* DBManager::authenticateUser(std::string username, std::string pass) {
    LoginResult result;
    std::string sql = "SELECT * FROM Users WHERE moodle_user = '" + username + "' AND password = '" + pass + "';";
    
    sqlite3_exec(db, sql.c_str(), loginCallback, &result, nullptr);
    
    return result.user; 
}

// [USUARIOS]

bool DBManager::insertUser(const User& u) {
    std::string sql = "INSERT OR IGNORE INTO Users VALUES ('" + u.getId() + "', '" + u.getFirstName() + "', '" + u.getLastName1() + "', '" + u.getLastName2() + "', '" + u.getMoodle() + "', '" + u.getPass() + "', '" + u.getRole() + "', '" + u.getBirth() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::deleteUser(std::string id) {
    std::string sql = "DELETE FROM Users WHERE id = '" + id + "';";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

void DBManager::showUsers() {
    bool headerNeeded = true;
    std::string sql = "SELECT * FROM Users;";
    
    std::cout << "\n--- LISTADO DE USUARIOS ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
    std::cout << "---------------------------\n" << std::endl;
}


// [MENSAJES]

bool DBManager::insertMessage(const Message& m) {
    std::string sql = "INSERT INTO Messages (sender_id, receiver_id, content, timestamp) VALUES ('" + 
                      m.getSender() + "', '" + m.getReceiver() + "', '" + 
                      m.getText() + "', '" + m.getTime() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::deleteMessage(int id) {
    std::string sql = "DELETE FROM Messages WHERE id = " + std::to_string(id) + ";";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

void DBManager::showMessages(std::string id1, std::string id2) {
    bool headerNeeded = true;
    
    std::string sql = 
        "SELECT M.timestamp, "
        "(U.first_name || ' ' || U.last_name1) AS Emisor, "
        "M.content "
        "FROM Messages M "
        "JOIN Users U ON M.sender_id = U.id "
        "WHERE (M.sender_id='" + id1 + "' AND M.receiver_id='" + id2 + "') "
        "OR (M.sender_id='" + id2 + "' AND M.receiver_id='" + id1 + "') "
        "ORDER BY M.timestamp ASC;";

    std::cout << "\n--- HISTORIAL DE CHAT ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
}


// [ALERTAS]

bool DBManager::insertAlert(const Alert& a) {
    std::string sql = "INSERT INTO Alerts (sender_id, receiver_id, date, subject, description, status) VALUES ('" + 
                      a.getSender() + "', '" + a.getReceiver() + "', '" + 
                      a.getDate() + "', '" + a.getSubject() + "', '" + 
                      a.getDesc() + "', '" + a.getStatus() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::deleteAlert(int id) {
    std::string sql = "DELETE FROM Alerts WHERE id = " + std::to_string(id) + ";";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

void DBManager::showAlerts() {
    std::string sql = 
        "SELECT A.id, A.date, "
        "(U1.first_name || ' ' || U1.last_name1) AS Emisor, "
        "(U2.first_name || ' ' || U2.last_name1) AS Receptor, "
        "A.subject, A.status "
        "FROM Alerts A "
        "LEFT JOIN Users U1 ON A.sender_id = U1.id "
        "LEFT JOIN Users U2 ON A.receiver_id = U2.id "
        "ORDER BY A.date DESC;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << ">> Error SQL al cargar alertas: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- GESTION GLOBAL DE ALERTAS ---" << std::endl;

    bool hayAlertas = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayAlertas = true;
        
        int id = sqlite3_column_int(stmt, 0);
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* emi  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* rec  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* sub  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* stat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        std::string sDate = date ? date : "??";
        std::string sEmi  = emi  ? emi  : "Usuario Desconocido";
        std::string sRec  = rec  ? rec  : "Usuario Desconocido";
        std::string sSub  = sub  ? sub  : "(Sin asunto)";
        std::string sStat = stat ? stat : "UNKNOWN";

        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "ID: " << id << " | FECHA: " << sDate << " | ESTADO: " << sStat << std::endl;
        std::cout << "DE:   " << sEmi << std::endl;
        std::cout << "PARA: " << sRec << std::endl;
        std::cout << "ASUNTO: " << sSub << std::endl;
    }

    if (!hayAlertas) {
        std::cout << ">> No hay alertas registradas en el sistema." << std::endl;
    }
    std::cout << "------------------------------------------------\n";

    sqlite3_finalize(stmt);
}


// [ASIGNACIONES]

bool DBManager::insertAssignment(std::string studentId, std::string tutorId, std::string date) {
    std::string sql = "INSERT OR IGNORE INTO Assignments VALUES ('" + studentId + "', '" + tutorId + "', '" + date + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::deleteAssignment(std::string studentId) {
    std::string sql = "DELETE FROM Assignments WHERE student_id = '" + studentId + "';";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

void DBManager::showAssignments() {
    std::string sql = 
        "SELECT "
        "(S.first_name || ' ' || S.last_name1) AS Alumno, "
        "(T.first_name || ' ' || T.last_name1) AS Tutor, "
        "A.assignment_date "
        "FROM Assignments A "
        "JOIN Users S ON A.student_id = S.id "
        "JOIN Users T ON A.tutor_id = T.id "
        "ORDER BY A.assignment_date DESC;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << ">> Error SQL: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- LISTADO DE ASIGNACIONES ACTIVAS ---" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
    printf("%-25s | %-25s | %-12s\n", "ALUMNO", "TUTOR", "FECHA");
    std::cout << "---------------------------------------------------------" << std::endl;

    bool hayDatos = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayDatos = true;
        const char* alum = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* tut  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* fecha= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        printf("%-25s | %-25s | %s\n", 
               alum ? alum : "Desconocido", 
               tut ? tut : "Desconocido", 
               fecha ? fecha : "??");
    }

    if (!hayDatos) std::cout << ">> No hay asignaciones registradas." << std::endl;
    std::cout << "---------------------------------------------------------\n";

    sqlite3_finalize(stmt);
}


std::vector<std::string> DBManager::getUserIdsByRole(std::string role) {
    std::vector<std::string> ids;
    std::string sql = "SELECT id FROM Users WHERE role = '" + role + "';";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (text) ids.push_back(text);
        }
    }
    sqlite3_finalize(stmt);
    return ids;
}

bool DBManager::updateAlertStatus(int alertId, std::string status) {
    std::string sql = "UPDATE Alerts SET status = '" + status + 
                      "' WHERE rowid = " + std::to_string(alertId) + ";";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::resetForNewAssignment() {
    bool m = sqlite3_exec(db, "DELETE FROM Messages;", nullptr, 0, nullptr) == SQLITE_OK;
    bool a = sqlite3_exec(db, "DELETE FROM Alerts;", nullptr, 0, nullptr) == SQLITE_OK;
    bool as = sqlite3_exec(db, "DELETE FROM Assignments;", nullptr, 0, nullptr) == SQLITE_OK;
    
    return (m && a && as);
}

void DBManager::showStudentsByTutor(std::string tutorId) {
    bool headerNeeded = true; 
    std::string sql = "SELECT U.* FROM Users U "
                      "JOIN Assignments A ON U.id = A.student_id "
                      "WHERE A.tutor_id = '" + tutorId + "';";
    std::cout << "\n--- MIS ALUMNOS ASIGNADOS ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
}

void DBManager::showAlertsForUser(std::string userId) {
    std::string sql = 
        "SELECT A.id, A.date, "
        "(U.first_name || ' ' || U.last_name1) AS Emisor, "
        "A.subject, A.description, A.status "
        "FROM Alerts A "
        "JOIN Users U ON A.sender_id = U.id "
        "WHERE A.receiver_id = '" + userId + "' "
        "ORDER BY A.date DESC;";

    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error al cargar alertas: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- MIS NOTIFICACIONES Y ALERTAS ---" << std::endl;

    bool hayAlertas = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayAlertas = true;
        
        int idAlert      = sqlite3_column_int(stmt, 0);
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* subj = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* stat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        std::string sDate = date ? date : "??";
        std::string sName = name ? name : "Sistema";
        std::string sSubj = subj ? subj : "(Sin asunto)";
        std::string sDesc = desc ? desc : "";
        std::string sStat = stat ? stat : "UNKNOWN";

        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "ID: " << idAlert << " | FECHA: " << sDate << " | ESTADO: " << sStat << std::endl;
        std::cout << "DE: " << sName << std::endl;
        std::cout << "ASUNTO: " << sSubj << std::endl;
        std::cout << "MENSAJE: " << sDesc << std::endl;
    }

    if (!hayAlertas) {
        std::cout << ">> No tienes ninguna alerta nueva." << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;

    sqlite3_finalize(stmt);
}

void DBManager::showAlertsByTutor(std::string tutorId) {
    bool headerNeeded = true;

    std::string sql = 
        "SELECT A.id, "
        "(U1.first_name || ' ' || U1.last_name1) AS Emisor, "
        "(U2.first_name || ' ' || U2.last_name1) AS Receptor, "
        "A.date, A.subject, A.description, A.status "
        "FROM Alerts A "
        "LEFT JOIN Users U1 ON A.sender_id = U1.id "
        "LEFT JOIN Users U2 ON A.receiver_id = U2.id "
        "WHERE A.sender_id = '" + tutorId + "' OR A.receiver_id = '" + tutorId + "';";

    std::cout << "\n--- GESTIÓN DE ALERTAS (TUTOR) ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
}

std::string DBManager::getTutorIdByStudent(std::string studentId) {
    std::string tutorId = "";
    std::string sql = "SELECT tutor_id FROM Assignments WHERE student_id = '" + studentId + "';";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (text) tutorId = text;
        }
    }
    sqlite3_finalize(stmt);
    return tutorId;
}