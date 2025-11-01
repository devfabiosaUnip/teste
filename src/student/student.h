#ifndef STUDENT_H
#define STUDENT_H

#include "models.h"
#include "protocol.h"

// Protótipos das funções do aluno
// int student_enroll_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
// int student_unenroll_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
void handle_student_menu(int client_socket, Student* student);
// void student_list_enrolled_subjects(int student_id, char* data_buffer, int buffer_size);

// Funções de gerenciamento de matérias para alunos (movidas para ADM)
// int student_enroll_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
// int student_unenroll_subject(int student_id, int subject_id, char* message_buffer, int buffer_size);
// void student_list_enrolled_subjects(int student_id, char* data_buffer, int buffer_size);

// Outras funções do aluno
void student_list_modules_in_subject(int student_id, int subject_id, char* data_buffer, int buffer_size);
void student_view_lessons_in_module(int student_id, int module_id, char* data_buffer, int buffer_size);
void student_view_quizzes_in_module(int student_id, int module_id, char* data_buffer, int buffer_size);
int student_submit_quiz_answers(int student_id, int quiz_id, const char* answers_str, char* message_buffer, int buffer_size);
void student_view_quiz_results(int student_id, char* data_buffer, int buffer_size);
void student_view_grades(int student_id, char* data_buffer, int buffer_size);
void student_view_absences(int student_id, char* data_buffer, int buffer_size);
int student_get_quiz_questions(int student_id, int quiz_id, char* data_buffer, int buffer_size, char* message_buffer, int message_buffer_size);
int get_student_by_id(int student_id, Student* student);

#endif // STUDENT_H
