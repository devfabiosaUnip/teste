#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "models.h" // Necessário para MAX_STR_LEN e outras definições

// Funções de utilidade para entrada de dados
void clear_input_buffer();
int get_int_input(const char* prompt);
void get_string_input(const char* prompt, char* buffer, int max_len);

// Funções de utilidade para exibição de menus (para o cliente)
void display_main_menu();
void display_admin_main_menu();
void display_admin_professor_menu();
void display_admin_class_menu();
void display_admin_student_menu();
void display_professor_main_menu(); // Novo
void display_professor_subject_class_menu(); // Novo
void display_professor_grades_absences_menu(); // Novo
void display_professor_modules_lessons_menu(); // Novo
void display_professor_quizzes_menu(); // Novo
void display_admin_professor_subject_class_assignment_menu(); // Novo

// Protótipos para funções auxiliares de busca
Student* get_student_by_id_ptr(int student_id);
Subject* get_subject_by_id_ptr(int subject_id);
int generate_next_id(); // Adicionado para resolver 'multiple definition'

#endif // COMMON_UTILS_H
