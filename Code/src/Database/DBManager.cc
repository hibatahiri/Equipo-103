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



// [USUARIOS]

//Insertar
//Atributos (id, first_name, last_name, moodle_user, password, role, birth_date)
bool DBManager::insertUser(std::string id, std::string fn, std::string ln1, std::string ln2, std::string moodle, std::string pass, std::string role, std::string birth) {
    std::string sql = "INSERT OR IGNORE INTO Users VALUES ('" + id + "', '" + fn + "', '" + ln1 + "', '" + ln2 + "', '" + moodle + "', '" + pass + "', '" + role + "', '" + birth + "');";
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



// [ALERTAS]

//Insertar
//Atributos (sender_id, receiver_id, date, subject, description, status)
bool DBManager::insertAlert(std::string sender, std::string receiver, std::string date, std::string subject, std::string desc) {
    std::string sql = "INSERT INTO Alerts (sender_id, receiver_id, date, subject, description) VALUES ('" 
                      + sender + "', '" + receiver + "', '" + date + "', '" + subject + "', '" + desc + "');";
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