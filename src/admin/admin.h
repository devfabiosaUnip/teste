#ifndef ADMIN_H
#define ADMIN_H

#include "models.h" 

// Funções de gerenciamento de Professores
int add_professor(const char* name, const char* email, const char* password, char* message_buffer, int buffer_size);
void list_professors(char* data_buffer, int buffer_size); // Retorna lista formatada
int edit_professor(int id, const char* name, const char* email, const char* password, char* message_buffer, int buffer_size);
int remove_professor(int id, char* message_buffer, int buffer_size);

// Funções de gerenciamento de Turmas
int add_class(const char* name, char* message_buffer, int buffer_size);
void list_classes(char* data_buffer, int buffer_size);
int edit_class(int id, const char* name, char* message_buffer, int buffer_size);
int remove_class(int id, char* message_buffer, int buffer_size);

// Funções de gerenciamento de Alunos
int add_student(const char* name, const char* email, const char* password, int class_id, char* message_buffer, int buffer_size);
void list_students(char* data_buffer, int buffer_size);
int edit_student(int id, const char* name, const char* email, const char* password, int new_class_id, char* message_buffer, int buffer_size);
int remove_student(int id, char* message_buffer, int buffer_size);

// Funções de Gerenciamento de Matérias (ADM)
int add_subject(const char* name, char* message_buffer, int buffer_size);
int admin_assign_subject_to_professor(int professor_id, int subject_id, char* message_buffer, int buffer_size);
int admin_unassign_subject_from_professor(int professor_id, int subject_id, char* message_buffer, int buffer_size);
int admin_assign_class_to_professor(int professor_id, int class_id, char* message_buffer, int buffer_size);
int admin_unassign_class_from_professor(int professor_id, int class_id, char* message_buffer, int buffer_size);

// Funções de Gerenciamento de Matrículas de Alunos (ADM)
int admin_enroll_student_in_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
int admin_unenroll_student_from_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
void admin_list_student_subjects(int student_id, char* data_buffer, int buffer_size);

#endif // ADMIN_H
