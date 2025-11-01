#ifndef PROFESSOR_H
#define PROFESSOR_H

#include "models.h"

// Funções de Gerenciamento de Matérias/Turmas
void list_professor_assigned_classes(int professor_id, char* data_buffer, int buffer_size);
void list_professor_assigned_subjects(int professor_id, char* data_buffer, int buffer_size);

// Funções de Notas e Faltas
int post_grade(int professor_id, int student_id, int subject_id, float grade_value, const char* description, char* message_buffer, int buffer_size);
int register_absence(int professor_id, int student_id, int subject_id, const char* date, const char* justification, char* message_buffer, int buffer_size);
void view_student_grades_absences(int professor_id, int student_id, char* data_buffer, int buffer_size);

// Funções de Módulos e Aulas
int create_subject_module(int professor_id, int subject_id, const char* module_name, char* message_buffer, int buffer_size);
void list_subject_modules(int professor_id, int subject_id, char* data_buffer, int buffer_size);
int edit_module(int professor_id, int module_id, const char* new_name, char* message_buffer, int buffer_size);
int remove_module(int professor_id, int module_id, char* message_buffer, int buffer_size);
int add_lesson_to_module(int professor_id, int module_id, const char* lesson_title, const char* youtube_link, char* message_buffer, int buffer_size);
int edit_lesson(int professor_id, int lesson_id, const char* new_title, const char* new_youtube_link, char* message_buffer, int buffer_size);
int remove_lesson(int professor_id, int lesson_id, char* message_buffer, int buffer_size);
void list_module_lessons(int professor_id, int module_id, char* data_buffer, int buffer_size);

// Funções de Questionários
int create_module_quiz(int professor_id, int module_id, const char* quiz_title, char* message_buffer, int buffer_size);
int edit_quiz(int professor_id, int quiz_id, const char* new_title, char* message_buffer, int buffer_size);
int remove_quiz(int professor_id, int quiz_id, char* message_buffer, int buffer_size);
int add_question_to_quiz(int professor_id, int quiz_id, const char* question_text, const char* option1, const char* option2, const char* option3, const char* option4, int correct_option, char* message_buffer, int buffer_size);
int edit_question(int professor_id, int quiz_id, int question_id, const char* new_text, const char* new_option1, const char* new_option2, const char* new_option3, const char* new_option4, int new_correct_option, char* message_buffer, int buffer_size);
int remove_question(int professor_id, int quiz_id, int question_id, char* message_buffer, int buffer_size);
void list_module_quizzes(int professor_id, int module_id, char* data_buffer, int buffer_size);

#endif // PROFESSOR_H
