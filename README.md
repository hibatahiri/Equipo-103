# Sistema de tutorias uco (c + SQL):
Aplicación de consola desarrollada en *C++* que simula un sistema de tutorías universitarias utilizando *SQL* como base de datos.


# Funcuinalidades:
- Gestión de usuarios (tutor y estudiante)
- Inicio de sesión con validación de credenciales
- Envío y consulta de alertas académicas
- Mensajería entre tutor y estudiante
- Visualización del estado de la base de datos


# Tecnoligias:
- Lenguaje C
- SQL
- Ejecución por terminal

  
# Ejecucion:
Desde la terminal:
.\build\tutoring_app.exe

# Compilacion:
g++ main.cc DBManager.cc sqlite3.c -o tutoring_app
