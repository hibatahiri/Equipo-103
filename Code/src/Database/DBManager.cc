#include "DBManager.h"
#include <iostream>

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
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
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
    std::cout << "\n--- USUARIOS REGISTRADOS ---" << std::endl;
    sqlite3_exec(db, "SELECT * FROM Users;", callback, 0, nullptr);
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
    std::string sql = "SELECT timestamp, sender_id, content FROM Messages WHERE "
                      "(sender_id='" + id1 + "' AND receiver_id='" + id2 + "') OR "
                      "(sender_id='" + id2 + "' AND receiver_id='" + id1 + "') "
                      "ORDER BY timestamp ASC;";
    std::cout << "\n--- CHAT ENTRE " << id1 << " Y " << id2 << " ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, 0, nullptr);
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
    std::cout << "\n--- LISTA DE ALERTAS ---" << std::endl;
    sqlite3_exec(db, "SELECT * FROM Alerts;", callback, 0, nullptr);
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
    std::cout << "\n--- RELACIONES ALUMNO-TUTOR ---" << std::endl;
    sqlite3_exec(db, "SELECT * FROM Assignments;", callback, 0, nullptr);
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
    // Consulta SQL con JOIN para filtrar por tutor asignado
    std::string sql = "SELECT U.* FROM Users U "
                      "JOIN Assignments A ON U.id = A.student_id "
                      "WHERE A.tutor_id = '" + tutorId + "';";

    std::cout << "\n--- MIS ALUMNOS ASIGNADOS ---" << std::endl;
    
    sqlite3_exec(db, sql.c_str(), callback, 0, nullptr);
}

void DBManager::showAlertsForUser(std::string userId) {
    // Seleccionamos rowid por si el alumno necesitara referenciarla
    // Filtramos por receptor_id para que sea privado
    std::string sql = "SELECT rowid, * FROM Alerts WHERE receiver_id = '" + userId + "';";

    std::cout << "\n--- MIS ALERTAS RECIBIDAS ---" << std::endl;
    sqlite3_exec(db, sql.c_str(), callback, 0, nullptr);
}

void DBManager::showAlertsByTutor(std::string tutorId) {
    // Filtramos para que solo vea lo relacionado con su ID
    std::string sql = "SELECT rowid, * FROM Alerts WHERE sender_id = '" + tutorId + 
                      "' OR receiver_id = '" + tutorId + "';";

    std::cout << "\n--- MIS ALERTAS GESTIONADAS ---" << std::endl;

    sqlite3_exec(db, sql.c_str(), callback, 0, nullptr);
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