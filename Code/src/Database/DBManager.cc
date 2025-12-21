#include "DBManager.h"
#include <iostream>
#include <iomanip>

DBManager::DBManager() {
    db = nullptr; //Inicializamos el puntero a nulo por seguridad
}

DBManager::~DBManager() {
    closeConnection(); //Nos aseguramos de cerrar la conexión al destruir el objeto
}

bool DBManager::openConnection() {
    //Intentamos abrir el archivo ubicado en la carpeta data
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
    //Tabla de Usuarios (Perfil de los usuarios)
    //Atributos (id TEXT PRIMARY KEY, first_name TEXT, last_name TEXT, moodle_user TEXT, password TEXT, role TEXT, birth_date TEXT)
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

    // Tabla de Mensajes (CHAT)
    //Atributos (id INTEGER PRIMARY KEY AUTOINCREMENT, sender_id TEXT, receiver_id TEXT, content TEXT, timestamp TEXT)
    std::string sqlMessages = 
        "CREATE TABLE IF NOT EXISTS Messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id TEXT NOT NULL, "
        "receiver_id TEXT NOT NULL, "
        "content TEXT NOT NULL, "
        "timestamp TEXT NOT NULL, "
        "FOREIGN KEY(sender_id) REFERENCES Users(id), "
        "FOREIGN KEY(receiver_id) REFERENCES Users(id));";

    //Tabla de Alertas (Gestion de avisos y notificaciones)
    //Atributos (id INTEGER PRIMARY KEY AUTOINCREMENT, sender_id TEXT, receiver_id TEXT, date TEXT, subject TEXT, description TEXT, status TEXT)
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

    //Tabla de Asignaciones (Relacion Alumno-Tutor)
    //Atributos (student_id TEXT PRIMARY KEY, tutor_id TEXT, assignment_date TEXT)
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


//Función auxiliar para procesar y mostrar los resultados de una consulta SELECT
//Atributos (argc: numero de columnas, argv: valores de la fila, azColName: nombres de columnas)
static int callback(void* data, int argc, char** argv, char** azColName) {
    bool* isFirstRow = (bool*)data;

auto getWidth = [](std::string colName) {
        // CAMBIO: 20 espacios para IDs largos (Matrículas, DNIs, etc.)
        if (colName == "id") return 20; 
        
        // Espacio para "Nombre Apellido"
        if (colName == "Emisor" || colName == "Receptor") return 22; 
        
        // Espacio para "2025-01-20 10:30:00"
        if (colName == "date" || colName == "timestamp") return 22;
        
        // Asuntos y Contenidos
        if (colName == "subject") return 30;
        if (colName == "description" || colName == "content") return 50;
        if (colName == "status") return 15;
        
        return 20; // Por defecto para cualquier otra cosa
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

//Función auxiliar para procesar el LOGIN
static int loginCallback(void* data, int argc, char** argv, char** azColName) {
    LoginResult* result = static_cast<LoginResult*>(data);
    if (argc > 0) {
        // Si hay resultados, creamos el objeto User con los datos de la DB
        result->user = new User(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    }
    return 0;
}

User* DBManager::authenticateUser(std::string username, std::string pass) {
    LoginResult result;
    std::string sql = "SELECT * FROM Users WHERE moodle_user = '" + username + "' AND password = '" + pass + "';";
    
    sqlite3_exec(db, sql.c_str(), loginCallback, &result, nullptr);
    
    return result.user; // Retorna el usuario encontrado o nullptr si falló
}

// [USUARIOS]

//Insertar
//Atributos (id, first_name, last_name, moodle_user, password, role, birth_date)
bool DBManager::insertUser(const User& u) {
    std::string sql = "INSERT OR IGNORE INTO Users VALUES ('" + u.getId() + "', '" + u.getFirstName() + "', '" + u.getLastName1() + "', '" + u.getLastName2() + "', '" + u.getMoodle() + "', '" + u.getPass() + "', '" + u.getRole() + "', '" + u.getBirth() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Borrar
//Atributos (id TEXT)
bool DBManager::deleteUser(std::string id) {
    std::string sql = "DELETE FROM Users WHERE id = '" + id + "';";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Mostrar
//Atributos (SELECT * FROM Users)
void DBManager::showUsers() {
    bool headerNeeded = true; // El flag que usará el callback
    std::string sql = "SELECT * FROM Users;";
    
    std::cout << "\n--- LISTADO DE USUARIOS ---" << std::endl;
    // Pasamos &headerNeeded como 4º argumento
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
    std::cout << "---------------------------\n" << std::endl;
}


// [MENSAJES]

//Insertar
//Atributos (sender_id, receiver_id, content, timestamp)
bool DBManager::insertMessage(const Message& m) {
    std::string sql = "INSERT INTO Messages (sender_id, receiver_id, content, timestamp) VALUES ('" + 
                      m.getSender() + "', '" + m.getReceiver() + "', '" + 
                      m.getText() + "', '" + m.getTime() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Borrar
//Atributos (id INTEGER)
bool DBManager::deleteMessage(int id) {
    std::string sql = "DELETE FROM Messages WHERE id = " + std::to_string(id) + ";";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Mostrar (Chat específico entre dos usuarios)
//Atributos (id1 TEXT, id2 TEXT)
void DBManager::showMessages(std::string id1, std::string id2) {
    bool headerNeeded = true;
    
    // Solo traducimos el nombre del que envía el mensaje (Sender)
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

//Insertar
//Atributos (sender_id, receiver_id, date, subject, description, status)
bool DBManager::insertAlert(const Alert& a) {
    std::string sql = "INSERT INTO Alerts (sender_id, receiver_id, date, subject, description, status) VALUES ('" + 
                      a.getSender() + "', '" + a.getReceiver() + "', '" + 
                      a.getDate() + "', '" + a.getSubject() + "', '" + 
                      a.getDesc() + "', '" + a.getStatus() + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Borrar
//Atributos (id INTEGER)
bool DBManager::deleteAlert(int id) {
    std::string sql = "DELETE FROM Alerts WHERE id = " + std::to_string(id) + ";";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Mostrar
//Atributos (SELECT * FROM Alerts)
void DBManager::showAlerts() {
    bool headerNeeded = true; // <--- AÑADIR
    std::cout << "\n--- LISTA DE ALERTAS ---" << std::endl;
    sqlite3_exec(db, "SELECT * FROM Alerts;", callback, &headerNeeded, nullptr); // <--- CAMBIAR 0 POR &headerNeeded
}



// [ASIGNACIONES]

//Insertar
//Atributos (student_id, tutor_id, assignment_date)
bool DBManager::insertAssignment(std::string studentId, std::string tutorId, std::string date) {
    std::string sql = "INSERT OR IGNORE INTO Assignments VALUES ('" + studentId + "', '" + tutorId + "', '" + date + "');";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Borrar
//Atributos (student_id TEXT)
bool DBManager::deleteAssignment(std::string studentId) {
    std::string sql = "DELETE FROM Assignments WHERE student_id = '" + studentId + "';";
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

//Mostrar
//Atributos (SELECT * FROM Assignments)
void DBManager::showAssignments() {
    bool headerNeeded = true; // <--- AÑADIR
    std::cout << "\n--- RELACIONES ALUMNO-TUTOR ---" << std::endl;
    sqlite3_exec(db, "SELECT * FROM Assignments;", callback, &headerNeeded, nullptr); // <--- CAMBIAR 0 POR &headerNeeded
}

//--------------------------------------------------------------------------------------------------------------------------------

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
    return ids; // Devuelve la lista de IDs encontrados
}

bool DBManager::updateAlertStatus(int alertId, std::string status) {
    std::string sql = "UPDATE Alerts SET status = '" + status + 
                      "' WHERE rowid = " + std::to_string(alertId) + ";";
    // Nota: Usamos rowid si no definiste una clave primaria autoincremental en Alerts
    return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
}

bool DBManager::resetForNewAssignment() {

    bool m = sqlite3_exec(db, "DELETE FROM Messages;", nullptr, 0, nullptr) == SQLITE_OK;
    bool a = sqlite3_exec(db, "DELETE FROM Alerts;", nullptr, 0, nullptr) == SQLITE_OK;
    bool as = sqlite3_exec(db, "DELETE FROM Assignments;", nullptr, 0, nullptr) == SQLITE_OK;
    
    return (m && a && as); // Retorna true si las tres tablas se limpiaron
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
    bool headerNeeded = true;
    
    // SQL AVANZADO:
    // 1. Unimos la tabla Alerts (A) con Users (U) usando el ID del Emisor (sender_id).
    // 2. Concatenamos U.first_name + espacio + U.last_name1 y lo llamamos 'Emisor'.
    std::string sql = 
        "SELECT A.id, "
        "(U.first_name || ' ' || U.last_name1) AS Emisor, " // <--- LA MAGIA
        "A.date, A.subject, A.description, A.status "
        "FROM Alerts A "
        "JOIN Users U ON A.sender_id = U.id "
        "WHERE A.receiver_id = '" + userId + "';";

    std::cout << "\n--- MIS ALERTAS RECIBIDAS ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, &headerNeeded, nullptr);
}

void DBManager::showAlertsByTutor(std::string tutorId) {
    bool headerNeeded = true;

    std::string sql = 
        "SELECT A.id, "
        "(U1.first_name || ' ' || U1.last_name1) AS Emisor, "   // Traduce sender_id
        "(U2.first_name || ' ' || U2.last_name1) AS Receptor, " // Traduce receiver_id
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
    return tutorId; // Retorna el ID del tutor o un string vacío si no tiene
}