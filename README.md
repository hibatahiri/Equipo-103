# Sistema de tutorias uco (c++ + SQLite):
Aplicación de consola desarrollada en *C++* que simula un sistema de tutorías universitarias utilizando *SQLite* como base de datos.


# Funcionalidades:
- Gestión de usuarios (Coordinador, Tutor y Estudiante)
- Inicio de sesión con validación de credenciales
- Asignación automática y equilibrada de alumnos a tutores
- Envío y consulta de alertas académicas
- Mensajería bidireccional entre tutor y estudiante
- Visualización y gestión del estado de la base de datos


# Tecnologías:
- Lenguaje: C++
- Base de datos: SQLite
- Ejecución: Terminal / Consola

  
# Ejecución:
Desde la terminal:
.\build\tutoring_app.exe

# Compilación:
g++ main.cc src/Database/DBManager.cc src/Users/Users.cc sqlite3.c -o tutoring_app
