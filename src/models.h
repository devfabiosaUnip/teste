#ifndef MODELS_H
#define MODELS_H

#include <time.h>

#define MAX_STR_LEN 100
#define MAX_EMAIL_LEN 50
#define MAX_PASSWORD_LEN 30
#define MAX_CLASSES_PER_PROF 5
#define MAX_SUBJECTS_PER_PROF 3
#define MAX_MODULES_PER_SUBJECT 10
#define MAX_LESSONS_PER_MODULE 20
#define MAX_QUIZZES_PER_MODULE 10
#define MAX_QUESTIONS_PER_QUIZ 10
#define MAX_OPTIONS_PER_QUESTION 4
#define MAX_ENROLLED_SUBJECTS_PER_STUDENT 10
#define MAX_GRADES_PER_STUDENT 50
#define MAX_ABSENCES_PER_STUDENT 50
#define MAX_STUDENTS_PER_CLASS 30
#define MAX_PROFESSORS 20
#define MAX_STUDENTS 100
#define MAX_CLASSES 10
#define MAX_ADMINS 1
#define MAX_QUIZ_RESULTS 500 // Adicionado

// Forward declarations
typedef struct Class Class;
typedef struct Subject Subject;
typedef struct Module Module;
typedef struct Lesson Lesson;
typedef struct Quiz Quiz;
typedef struct Question Question;

// User roles
typedef enum {
    ROLE_ADMIN,
    ROLE_PROFESSOR,
    ROLE_STUDENT
} UserRole;

// Generic User (for login)
typedef struct {
    int id;
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    UserRole role;
} User;

// Admin
typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
} Admin;

// Student
typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int class_id; // ID da turma
    int num_grades; // Número de notas registradas para o aluno
    int num_absences; // Número de faltas registradas para o aluno
    int enrolled_subject_ids[MAX_ENROLLED_SUBJECTS_PER_STUDENT]; // IDs das matérias matriculadas
    int num_enrolled_subjects; // Número de matérias matriculadas
    // Adicionar campos para notas e faltas depois
} Student;

// Professor
typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int assigned_classes[MAX_CLASSES_PER_PROF]; // IDs das turmas
    int num_assigned_classes;
    int assigned_subjects[MAX_SUBJECTS_PER_PROF]; // IDs das matérias
    int num_assigned_subjects;
} Professor;

// Class (Turma)
struct Class {
    int id;
    char name[MAX_STR_LEN];
    int professor_id; // Professor responsável pela turma (opcional, pode ter mais de um)
    int student_ids[MAX_STUDENTS_PER_CLASS];
    int num_students;
};

// Subject (Matéria)
struct Subject {
    int id;
    char name[MAX_STR_LEN];
    int professor_id; // Professor que leciona a matéria
    int module_ids[MAX_MODULES_PER_SUBJECT];
    int num_modules;
};

// Module
struct Module {
    int id;
    char name[MAX_STR_LEN];
    int subject_id;
    int lesson_ids[MAX_LESSONS_PER_MODULE];
    int num_lessons;
    int quiz_ids[MAX_QUIZZES_PER_MODULE];
    int num_quizzes;
};

// Lesson (Aula)
struct Lesson {
    int id;
    char title[MAX_STR_LEN];
    char youtube_link[MAX_STR_LEN];
    int module_id;
};

// Question
struct Question {
    int id;
    char text[MAX_STR_LEN];
    char options[4][MAX_STR_LEN]; // 4 opções de resposta
    int correct_option; // Índice da opção correta (0-3)
};

// Quiz (Questionário)
struct Quiz {
    int id;
    char title[MAX_STR_LEN];
    int module_id;
    Question questions[MAX_QUESTIONS_PER_QUIZ];
    int num_questions;
};

// Student's Quiz Result
typedef struct {
    int id;
    int student_id;
    int quiz_id;
    int score; // Pontuação obtida
    time_t timestamp;
    // Talvez adicionar um array de respostas do aluno para cada questão
} QuizResult;


// Grade (Nota)
typedef struct {
    int student_id;
    int subject_id;
    float grade_value;
    char description[MAX_STR_LEN];
    float score;
} Grade;

// Absence (Falta)
typedef struct {
    int student_id;
    int subject_id;
    char date[MAX_STR_LEN]; // Formato "DD/MM/AAAA"
    char justification[MAX_STR_LEN]; // Opcional
} Absence;


// Arrays para armazenar todos os dados
typedef struct {
    Admin admins[MAX_ADMINS];
    int num_admins;
    Professor professors[MAX_PROFESSORS];
    int num_professors;
    Class classes[MAX_CLASSES];
    int num_classes;
    Subject subjects[MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS]; // Estimativa máxima
    int num_subjects;
    Module modules[MAX_MODULES_PER_SUBJECT * MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS]; // Estimativa máxima
    int num_modules;
    Lesson lessons[MAX_LESSONS_PER_MODULE * MAX_MODULES_PER_SUBJECT * MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS]; // Estimativa máxima
    int num_lessons;
    Quiz quizzes[MAX_QUIZZES_PER_MODULE * MAX_MODULES_PER_SUBJECT * MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS]; // Estimativa máxima
    int num_quizzes;
    Grade grades[MAX_GRADES_PER_STUDENT * MAX_STUDENTS]; // Estimativa máxima
    int num_grades;
    Absence absences[MAX_ABSENCES_PER_STUDENT * MAX_STUDENTS]; // Estimativa máxima
    int num_absences;

    // Resultados de Questionários
    QuizResult quiz_results[MAX_STUDENTS * MAX_QUIZZES_PER_MODULE * MAX_MODULES_PER_SUBJECT]; // Estimativa máxima
    int num_quiz_results;

    Student students[MAX_STUDENTS];
    int num_students;

} SystemData;

extern SystemData global_data; // Variável global para todos os dados do sistema

#endif // MODELS_H
