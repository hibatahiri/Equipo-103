#include <iostream>
#include "DBManager.h"

int main() {
    DBManager dbManager;

    std::cout << "--- INICIANDO SIMULACION DEL SISTEMA UCO ---" << std::endl;

    if (dbManager.openConnection()) {
        // 1. Limpiamos y preparamos el esquema
        dbManager.initializeSchema();

        // 2. REGISTRO DE TUTORES (T1, T2)
        // Parametros: id, nombre, apellido1, apellido2, moodle, pass, rol, fecha_nac
        dbManager.insertUser("T1", "Maria", "Martin", "Fernandez", "mmartin", "tutor123", "TUTOR", "1985-03-20");
        dbManager.insertUser("T2", "Antonio", "Ramos", "Jimenez", "aramos", "tutor456", "TUTOR", "1990-07-12");

        // 3. REGISTRO DE ALUMNOS (A1, A2, A3)
        dbManager.insertUser("A1", "Juan", "Perez", "Garcia", "jperez", "alum1", "STUDENT", "2003-11-05");
        dbManager.insertUser("A2", "Ana", "Lopez", "Sanz", "alopez", "alum2", "STUDENT", "2004-01-15");
        dbManager.insertUser("A3", "Luis", "Gomez", "Ruiz", "lgomez", "alum3", "STUDENT", "2003-05-22");

        // 4. ASIGNACION DE TUTORIAS
        // Juan (A1) y Ana (A2) con Maria (T1)
        dbManager.insertAssignment("A1", "T1", "2025-12-17");
        dbManager.insertAssignment("A2", "T1", "2025-12-17");
        // Luis (A3) con Antonio (T2)
        dbManager.insertAssignment("A3", "T2", "2025-12-17");

        // 5. ENVIO DE ALERTAS DE PRUEBA
        dbManager.insertAlert("T1", "A1", "2025-12-17", "Bienvenida", "Hola Juan, soy tu tutora Maria.");
        dbManager.insertAlert("T2", "A3", "2025-12-17", "Cita", "Luis, tenemos reunion mañana a las 10:00.");

        // 6. MOSTRAR RESULTADOS POR TERMINAL
        // Nota: Los usuarios apareceran ordenados por apellido1 y apellido2
        dbManager.showUsers();
        dbManager.showAssignments();
        dbManager.showAlerts();

        // 7. Cierre seguro de la conexion
        dbManager.closeConnection();
        std::cout << "--- SIMULACION FINALIZADA ---" << std::endl;
    } else {
        std::cerr << "No se pudo conectar a la base de datos." << std::endl;
    }

    return 0;
}