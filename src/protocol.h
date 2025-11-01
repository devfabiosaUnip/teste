#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_MESSAGE_LEN 512
#define MAX_DATA_LEN 2048

// Definições de códigos para comandos do cliente para o servidor
typedef enum {
    CMD_UNKNOWN,
    CMD_LOGIN,
    // Comandos de Administrador - Professores
    CMD_ADMIN_ADD_PROFESSOR,
    CMD_ADMIN_LIST_PROFESSORS,
    CMD_ADMIN_EDIT_PROFESSOR,
    CMD_ADMIN_REMOVE_PROFESSOR,
    // Novos Comandos de Administrador - Atribuição Professor-Matéria/Turma
    CMD_ADMIN_ASSIGN_SUBJECT_TO_PROFESSOR,
    CMD_ADMIN_UNASSIGN_SUBJECT_FROM_PROFESSOR,
    CMD_ADMIN_ASSIGN_CLASS_TO_PROFESSOR,
    CMD_ADMIN_UNASSIGN_CLASS_FROM_PROFESSOR,
    // Comandos de Administrador - Matérias/Turmas
    CMD_ADMIN_ASSIGN_SUBJECT,
    CMD_ADMIN_UNASSIGN_SUBJECT,
    CMD_ADMIN_ASSIGN_CLASS,
    CMD_ADMIN_UNASSIGN_CLASS,
    // Comandos de Administrador - Turmas
    CMD_ADMIN_ADD_CLASS,
    CMD_ADMIN_LIST_CLASSES,
    CMD_ADMIN_EDIT_CLASS,
    CMD_ADMIN_REMOVE_CLASS,
    // Comandos de Administrador - Alunos
    CMD_ADMIN_ADD_STUDENT,
    CMD_ADMIN_LIST_STUDENTS,
    CMD_ADMIN_EDIT_STUDENT,
    CMD_ADMIN_REMOVE_STUDENT,
    // Comandos de Professor - Gerenciamento de Matérias/Turmas
    CMD_PROFESSOR_LIST_ASSIGNED_CLASSES,
    CMD_PROFESSOR_LIST_ASSIGNED_SUBJECTS,
    // Comandos de Professor - Notas e Faltas
    CMD_PROFESSOR_POST_GRADE,
    CMD_PROFESSOR_REGISTER_ABSENCE,
    CMD_PROFESSOR_VIEW_GRADES_ABSENCES,
    // Comandos de Professor - Módulos e Aulas
    CMD_PROFESSOR_CREATE_SUBJECT_MODULE,
    CMD_PROFESSOR_LIST_SUBJECT_MODULES,
    CMD_PROFESSOR_EDIT_MODULE,
    CMD_PROFESSOR_REMOVE_MODULE,
    CMD_PROFESSOR_ADD_LESSON_TO_MODULE,
    CMD_PROFESSOR_EDIT_LESSON,
    CMD_PROFESSOR_REMOVE_LESSON,
    CMD_PROFESSOR_LIST_MODULE_LESSONS,
    // Comandos de Professor - Questionários
    CMD_PROFESSOR_CREATE_MODULE_QUIZ,
    CMD_PROFESSOR_EDIT_QUIZ,
    CMD_PROFESSOR_REMOVE_QUIZ,
    CMD_PROFESSOR_ADD_QUESTION_TO_QUIZ,
    CMD_PROFESSOR_EDIT_QUESTION,
    CMD_PROFESSOR_REMOVE_QUESTION,
    CMD_PROFESSOR_LIST_MODULE_QUIZZES,

    // Comandos de Aluno
    CMD_STUDENT_ENROLL_SUBJECT,
    CMD_STUDENT_UNENROLL_SUBJECT,
    CMD_STUDENT_LIST_ENROLLED_SUBJECTS,
    CMD_STUDENT_LIST_MODULES_IN_SUBJECT,
    CMD_STUDENT_VIEW_LESSONS_IN_MODULE,
    CMD_STUDENT_VIEW_QUIZZES_IN_MODULE,
    CMD_STUDENT_TAKE_QUIZ,
    CMD_STUDENT_REQUEST_QUIZ_QUESTIONS,
    CMD_STUDENT_VIEW_GRADES,
    CMD_STUDENT_VIEW_ABSENCES,
} CommandType;

// Definição de uma estrutura de mensagem genérica para comunicação
typedef struct {
    CommandType command; // Tipo do comando
    int user_id;         // ID do usuário que enviou o comando (para autenticação)
    char data[MAX_DATA_LEN];      // Dados da mensagem (pode ser formatado como JSON ou string simples)
} ClientMessage;

typedef struct {
    int status;          // 0 para sucesso, -1 para erro, ou outros códigos de status
    int user_id;         // ID do usuário logado (se aplicável)
    char message[MAX_MESSAGE_LEN];   // Mensagem de resposta ou erro
    char data[MAX_DATA_LEN];     // Dados de retorno (e.g., lista de professores, detalhes de um aluno)
} ServerResponse;

#endif // PROTOCOL_H
