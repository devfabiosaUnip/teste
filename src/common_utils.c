#include "common_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Limpa o buffer de entrada
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Obtém uma entrada de inteiro com validação
int get_int_input(const char* prompt) {
    int value;
    char buffer[MAX_STR_LEN];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            } else {
                printf("Entrada inválida. Por favor, digite um número inteiro.\n");
            }
        } else {
            printf("Erro na leitura da entrada.\n");
            clear_input_buffer();
        }
    }
}

// Obtém uma entrada de string com validação de tamanho
void get_string_input(const char* prompt, char* buffer, int max_len) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, max_len, stdin) != NULL) {
            // Remove o '\n' final se presente
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0) {
                return;
            } else {
                printf("A entrada não pode ser vazia. Tente novamente.\n");
            }
        } else {
            printf("Erro na leitura da entrada.\n");
            clear_input_buffer();
        }
    }
}

// Menus (implementação simples para o CLIENTE)
void display_main_menu() {
    printf("\n--- MENU PRINCIPAL ---\n");
    printf("1. Login\n");
    printf("0. Sair\n");
    printf("--------------------\n");
}

void display_admin_main_menu() {
    printf("\n--- MENU ADMINISTRADOR PRINCIPAL ---\n");
    printf("1. Gerenciar Professores\n");
    printf("2. Gerenciar Turmas\n");
    printf("3. Gerenciar Alunos\n");
    printf("4. Gerenciar Atribuições Professor-Matéria/Turma\n");
    printf("0. Logout\n");
    printf("-----------------------------------------------------------\n");
}

void display_admin_professor_menu() {
    printf("\n--- ADMIN: GERENCIAR PROFESSORES ---\n");
    printf("1. Adicionar Professor\n");
    printf("2. Listar Professores\n");
    printf("3. Editar Professor\n");
    printf("4. Remover Professor\n");
    printf("0. Voltar ao Menu Admin Principal\n");
    printf("------------------------------------\n");
}

void display_admin_class_menu() {
    printf("\n--- ADMIN: GERENCIAR TURMAS ---\n");
    printf("1. Adicionar Turma\n");
    printf("2. Listar Turmas\n");
    printf("3. Editar Turma\n");
    printf("4. Remover Turma\n");
    printf("0. Voltar ao Menu Admin Principal\n");
    printf("------------------------------\n");
}

void display_admin_student_menu() {
    printf("\n--- ADMIN: GERENCIAR ALUNOS ---\n");
    printf("1. Adicionar Aluno\n");
    printf("2. Listar Alunos\n");
    printf("3. Editar Aluno\n");
    printf("4. Remover Aluno\n");
    printf("0. Voltar ao Menu Admin Principal\n");
    printf("-------------------------------\n");
}

void display_professor_menu() {
    printf("\n--- MENU PROFESSOR ---\n");
    printf("1. Minhas Matérias e Módulos (em desenvolvimento)\n");
    printf("2. Postar Notas (em desenvolvimento)\n");
    printf("0. Logout\n");
    printf("--------------------\n");
}

void display_student_menu() {
    printf("\n--- MENU ALUNO ---\n");
    printf("1. Consultar Meus Dados (Notas, Faltas) (em desenvolvimento)\n");
    printf("2. Acessar Matérias e Módulos (em desenvolvimento)\n");
    printf("0. Logout\n");
    printf("----------------\n");
}

// Novo: Menu Principal do Professor
void display_professor_main_menu() {
    printf("\n--- MENU PROFESSOR PRINCIPAL ---\n");
    printf("1. Gerenciar Matérias e Turmas Atribuídas\n");
    printf("2. Gerenciar Notas e Faltas de Alunos\n");
    printf("3. Gerenciar Módulos e Aulas\n");
    printf("4. Gerenciar Questionários\n");
    printf("0. Logout\n");
    printf("------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Novo: Menu de Gerenciamento de Matérias e Turmas do Professor
void display_professor_subject_class_menu() {
    printf("\n--- PROFESSOR: GERENCIAR MATÉRIAS/TURMAS ---\n");
    printf("1. Atribuir Matéria\n");
    printf("2. Desatribuir Matéria\n");
    printf("3. Atribuir Turma\n");
    printf("4. Desatribuir Turma\n");
    printf("5. Listar Matérias Atribuídas\n");
    printf("6. Listar Turmas Atribuídas\n");
    printf("0. Voltar ao Menu Professor Principal\n");
    printf("------------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Novo: Menu de Gerenciamento de Notas e Faltas do Professor
void display_professor_grades_absences_menu() {
    printf("\n--- PROFESSOR: GERENCIAR NOTAS/FALTAS ---\n");
    printf("1. Lançar Nota\n");
    printf("2. Registrar Falta\n");
    printf("3. Visualizar Notas e Faltas de Aluno\n");
    printf("0. Voltar ao Menu Professor Principal\n");
    printf("----------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Novo: Menu de Gerenciamento de Módulos e Aulas do Professor
void display_professor_modules_lessons_menu() {
    printf("\n--- PROFESSOR: GERENCIAR MÓDULOS/AULAS ---\n");
    printf("1. Criar Módulo em Matéria\n");
    printf("2. Listar Módulos de Matéria\n");
    printf("3. Editar Módulo\n");
    printf("4. Remover Módulo\n");
    printf("5. Adicionar Aula a Módulo\n");
    printf("6. Editar Aula\n");
    printf("7. Remover Aula\n");
    printf("8. Listar Aulas de Módulo\n");
    printf("0. Voltar ao Menu Professor Principal\n");
    printf("----------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Novo: Menu de Gerenciamento de Questionários do Professor
void display_professor_quizzes_menu() {
    printf("\n--- PROFESSOR: GERENCIAR QUESTIONÁRIOS ---\n");
    printf("1. Criar Questionário em Módulo\n");
    printf("2. Editar Questionário\n");
    printf("3. Remover Questionário\n");
    printf("4. Adicionar Pergunta a Questionário\n");
    printf("5. Editar Pergunta de Questionário\n");
    printf("6. Remover Pergunta de Questionário\n");
    printf("7. Listar Questionários de Módulo\n");
    printf("0. Voltar ao Menu Professor Principal\n");
    printf("----------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Novo: Menu de Gerenciamento de Matérias e Turmas para Administrador
void display_admin_professor_subject_class_assignment_menu() {
    printf("\n--- ADMINISTRADOR: GERENCIAR ATRIBUIÇÕES PROFESSOR-MATÉRIA/TURMA ---\n");
    printf("1. Atribuir Matéria a Professor\n");
    printf("2. Desatribuir Matéria de Professor\n");
    printf("3. Atribuir Turma a Professor\n");
    printf("4. Desatribuir Turma de Professor\n");
    printf("0. Voltar ao Menu Admin Principal\n");
    printf("-----------------------------------------------------------\n");
    printf("Escolha uma opção: ");
}

// Funções auxiliares de busca de entidades
Student* get_student_by_id_ptr(int student_id) {
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            return &global_data.students[i];
        }
    }
    return NULL;
}

Subject* get_subject_by_id_ptr(int subject_id) {
    for (int i = 0; i < global_data.num_subjects; ++i) {
        if (global_data.subjects[i].id == subject_id) {
            return &global_data.subjects[i];
        }
    }
    return NULL;
}

// Função para gerar um novo ID único
static int next_id_counter = 1; // Começa em 1
int generate_next_id() {
    return next_id_counter++;
}