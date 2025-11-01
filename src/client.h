#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h" // Necessário para ServerResponse e CommandType

void start_client(const char* server_ip, int port);

// Funções de manipulação de ações do cliente (Admin, Professor, Aluno)
void handle_admin_actions(int admin_id, int client_socket);
void handle_professor_actions(int professor_id, int client_socket);
void handle_professor_grades_absences_menu(int professor_id, int client_socket);
void handle_professor_modules_lessons_menu(int professor_id, int client_socket);
void handle_professor_quizzes_menu(int professor_id, int client_socket);
void handle_admin_professor_subject_class_assignment_menu(int admin_id, int client_socket);

// Protótipos das funções de manipulação do aluno
void handle_student_actions(int student_id, int client_socket);
void display_student_main_menu();
void display_student_subject_menu();
void display_student_module_menu();
void display_student_quiz_menu();

ServerResponse send_command_to_server(int client_socket, CommandType command, int user_id, const char* data);

#endif // CLIENT_H
