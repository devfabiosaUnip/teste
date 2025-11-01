#include "admin/admin.h"
#include "common_utils.h"
#include "storage/storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Implementação das funções de gerenciamento de Professores
int add_professor(const char* name, const char* email, const char* password, char* message_buffer, int buffer_size) {
    if (global_data.num_professors >= MAX_PROFESSORS) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de professores atingido.");
        return -1;
    }

    Professor new_prof;
    new_prof.id = global_data.num_professors + 1; // ID simples incremental
    strncpy(new_prof.name, name, MAX_STR_LEN - 1);
    new_prof.name[MAX_STR_LEN - 1] = '\0';
    strncpy(new_prof.email, email, MAX_EMAIL_LEN - 1);
    new_prof.email[MAX_EMAIL_LEN - 1] = '\0';
    strncpy(new_prof.password, password, MAX_PASSWORD_LEN - 1);
    new_prof.password[MAX_PASSWORD_LEN - 1] = '\0';

    // Lógica para atribuir turmas e matérias (simplificado por enquanto)
    new_prof.num_assigned_classes = 0;
    new_prof.num_assigned_subjects = 0;

    global_data.professors[global_data.num_professors] = new_prof;
    global_data.num_professors++;
    snprintf(message_buffer, buffer_size, "Professor %s adicionado com sucesso!", new_prof.name);
    return 0;
}

void list_professors(char* data_buffer, int buffer_size) {
    if (global_data.num_professors == 0) {
        snprintf(data_buffer, buffer_size, "Nenhum professor cadastrado.");
        return;
    }

    char temp_buffer[200]; // Buffer temporário para cada linha de professor
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    for (int i = 0; i < global_data.num_professors; i++) {
        snprintf(temp_buffer, sizeof(temp_buffer), "ID: %d, Nome: %s, Email: %s\n", 
                 global_data.professors[i].id, 
                 global_data.professors[i].name, 
                 global_data.professors[i].email);
        // Concatena ao data_buffer, garantindo que não ultrapasse o tamanho
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)");
            break;
        }
    }
}

int edit_professor(int id, const char* name, const char* email, const char* password, char* message_buffer, int buffer_size) {
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == id) {
            strncpy(global_data.professors[i].name, name, MAX_STR_LEN - 1);
            global_data.professors[i].name[MAX_STR_LEN - 1] = '\0';
            strncpy(global_data.professors[i].email, email, MAX_EMAIL_LEN - 1);
            global_data.professors[i].email[MAX_EMAIL_LEN - 1] = '\0';
            strncpy(global_data.professors[i].password, password, MAX_PASSWORD_LEN - 1);
            global_data.professors[i].password[MAX_PASSWORD_LEN - 1] = '\0';
            snprintf(message_buffer, buffer_size, "Professor com ID %d atualizado com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", id);
    return -1;
}

int remove_professor(int id, char* message_buffer, int buffer_size) {
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == id) {
            // Move os últimos professores para preencher o espaço
            for (int j = i; j < global_data.num_professors - 1; j++) {
                global_data.professors[j] = global_data.professors[j+1];
            }
            global_data.num_professors--;
            snprintf(message_buffer, buffer_size, "Professor com ID %d removido com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", id);
    return -1;
}

// Implementação das funções de gerenciamento de Turmas
int add_class(const char* name, char* message_buffer, int buffer_size) {
    if (global_data.num_classes >= MAX_CLASSES) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de turmas atingido.");
        return -1;
    }

    Class new_class;
    new_class.id = global_data.num_classes + 1;
    strncpy(new_class.name, name, MAX_STR_LEN - 1);
    new_class.name[MAX_STR_LEN - 1] = '\0';
    new_class.num_students = 0;
    // Professor ID opcional por enquanto
    new_class.professor_id = 0;

    global_data.classes[global_data.num_classes] = new_class;
    global_data.num_classes++;
    snprintf(message_buffer, buffer_size, "Turma %s adicionada com sucesso!", new_class.name);
    return 0;
}

void list_classes(char* data_buffer, int buffer_size) {
    if (global_data.num_classes == 0) {
        snprintf(data_buffer, buffer_size, "Nenhuma turma cadastrada.");
        return;
    }

    char temp_buffer[200]; // Buffer temporário para cada linha de turma
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    for (int i = 0; i < global_data.num_classes; i++) {
        snprintf(temp_buffer, sizeof(temp_buffer), "ID: %d, Nome: %s\n", 
                 global_data.classes[i].id, 
                 global_data.classes[i].name);
        // Concatena ao data_buffer, garantindo que não ultrapasse o tamanho
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)");
            break;
        }
    }
}

int edit_class(int id, const char* name, char* message_buffer, int buffer_size) {
    for (int i = 0; i < global_data.num_classes; i++) {
        if (global_data.classes[i].id == id) {
            strncpy(global_data.classes[i].name, name, MAX_STR_LEN - 1);
            global_data.classes[i].name[MAX_STR_LEN - 1] = '\0';
            snprintf(message_buffer, buffer_size, "Turma com ID %d atualizada com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d não encontrada.", id);
    return -1;
}

int remove_class(int id, char* message_buffer, int buffer_size) {
    for (int i = 0; i < global_data.num_classes; i++) {
        if (global_data.classes[i].id == id) {
            // Move as últimas turmas para preencher o espaço
            for (int j = i; j < global_data.num_classes - 1; j++) {
                global_data.classes[j] = global_data.classes[j+1];
            }
            global_data.num_classes--;
            snprintf(message_buffer, buffer_size, "Turma com ID %d removida com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d não encontrada.", id);
    return -1;
}

// Implementação das funções de gerenciamento de Alunos
int add_student(const char* name, const char* email, const char* password, int class_id, char* message_buffer, int buffer_size) {
    if (global_data.num_students >= MAX_STUDENTS) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de alunos atingido.");
        return -1;
    }

    // Verificar se a turma existe
    int class_found = 0;
    for (int i = 0; i < global_data.num_classes; i++) {
        if (global_data.classes[i].id == class_id) {
            class_found = 1;
            break;
        }
    }
    if (!class_found) {
        snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d não encontrada.", class_id);
        return -1;
    }

    Student new_student;
    new_student.id = global_data.num_students + 1;
    strncpy(new_student.name, name, MAX_STR_LEN - 1);
    new_student.name[MAX_STR_LEN - 1] = '\0';
    strncpy(new_student.email, email, MAX_EMAIL_LEN - 1);
    new_student.email[MAX_EMAIL_LEN - 1] = '\0';
    strncpy(new_student.password, password, MAX_PASSWORD_LEN - 1);
    new_student.password[MAX_PASSWORD_LEN - 1] = '\0';
    new_student.class_id = class_id;
    new_student.num_grades = 0;
    new_student.num_absences = 0;

    global_data.students[global_data.num_students] = new_student;
    global_data.num_students++;
    snprintf(message_buffer, buffer_size, "Aluno %s adicionado com sucesso na turma %d!", new_student.name, class_id);
    return 0;
}

void list_students(char* data_buffer, int buffer_size) {
    if (global_data.num_students == 0) {
        snprintf(data_buffer, buffer_size, "Nenhum aluno cadastrado.");
        return;
    }

    char temp_buffer[200]; // Buffer temporário para cada linha de aluno
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    for (int i = 0; i < global_data.num_students; i++) {
        snprintf(temp_buffer, sizeof(temp_buffer), "ID: %d, Nome: %s, Email: %s, Turma ID: %d\n", 
                 global_data.students[i].id, 
                 global_data.students[i].name, 
                 global_data.students[i].email,
                 global_data.students[i].class_id);
        // Concatena ao data_buffer, garantindo que não ultrapasse o tamanho
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)");
            break;
        }
    }
}

int edit_student(int id, const char* name, const char* email, const char* password, int new_class_id, char* message_buffer, int buffer_size) {
    // Verificar se a nova turma existe, se fornecida
    if (new_class_id != 0) { // Assumindo 0 como ID de turma inválido/não fornecido
        int class_found = 0;
        for (int i = 0; i < global_data.num_classes; i++) {
            if (global_data.classes[i].id == new_class_id) {
                class_found = 1;
                break;
            }
        }
        if (!class_found) {
            snprintf(message_buffer, buffer_size, "Erro: Nova turma com ID %d não encontrada.", new_class_id);
            return -1;
        }
    }

    for (int i = 0; i < global_data.num_students; i++) {
        if (global_data.students[i].id == id) {
            strncpy(global_data.students[i].name, name, MAX_STR_LEN - 1);
            global_data.students[i].name[MAX_STR_LEN - 1] = '\0';
            strncpy(global_data.students[i].email, email, MAX_EMAIL_LEN - 1);
            global_data.students[i].email[MAX_EMAIL_LEN - 1] = '\0';
            strncpy(global_data.students[i].password, password, MAX_PASSWORD_LEN - 1);
            global_data.students[i].password[MAX_PASSWORD_LEN - 1] = '\0';
            if (new_class_id != 0) {
                global_data.students[i].class_id = new_class_id;
            }
            snprintf(message_buffer, buffer_size, "Aluno com ID %d atualizado com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", id);
    return -1;
}

int remove_student(int id, char* message_buffer, int buffer_size) {
    for (int i = 0; i < global_data.num_students; i++) {
        if (global_data.students[i].id == id) {
            // Move os últimos alunos para preencher o espaço
            for (int j = i; j < global_data.num_students - 1; j++) {
                global_data.students[j] = global_data.students[j+1];
            }
            global_data.num_students--;
            snprintf(message_buffer, buffer_size, "Aluno com ID %d removido com sucesso!", id);
            return 0;
        }
    }
    snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", id);
    return -1;
}

// Funções de Gerenciamento de Matérias/Turmas (agora para o Admin)
int add_subject(const char* name, char* message_buffer, int buffer_size) {
    if (global_data.num_subjects >= MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de matérias atingido no sistema.");
        return -1;
    }

    Subject new_subject;
    new_subject.id = (global_data.num_subjects > 0) ? global_data.subjects[global_data.num_subjects - 1].id + 1 : 1;
    strncpy(new_subject.name, name, MAX_STR_LEN - 1);
    new_subject.name[MAX_STR_LEN - 1] = '\0';
    new_subject.professor_id = 0; // Ninguém atribuído inicialmente
    new_subject.num_modules = 0;

    global_data.subjects[global_data.num_subjects] = new_subject;
    global_data.num_subjects++;
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Matéria '%s' (ID: %d) criada com sucesso!", new_subject.name, new_subject.id);
    return 0;
}

int admin_assign_subject_to_professor(int professor_id, int subject_id, char* message_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return -1;
    }

    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    // Se a matéria não existe, criá-la (simplificado, poderia ser um comando ADMIN)
    if (!subject) {
        if (global_data.num_subjects >= MAX_SUBJECTS_PER_PROF * MAX_PROFESSORS) {
            snprintf(message_buffer, buffer_size, "Erro: Limite de matérias atingido no sistema.");
            return -1;
        }
        Subject new_subject;
        new_subject.id = (global_data.num_subjects > 0) ? global_data.subjects[global_data.num_subjects - 1].id + 1 : 1; // ID incremental
        snprintf(new_subject.name, MAX_STR_LEN, "Nova Matéria %d", new_subject.id); // Nome genérico
        new_subject.professor_id = professor_id;
        new_subject.num_modules = 0;
        global_data.subjects[global_data.num_subjects] = new_subject;
        subject = &global_data.subjects[global_data.num_subjects];
        global_data.num_subjects++;
        //snprintf(message_buffer, buffer_size, "Matéria com ID %d criada automaticamente.\n", new_subject.id); // Mensagem adicional se criado
    }

    if (prof->num_assigned_subjects >= MAX_SUBJECTS_PER_PROF) {
        snprintf(message_buffer, buffer_size, "Erro: Professor já atribuído ao número máximo de matérias (%d).", MAX_SUBJECTS_PER_PROF);
        return -1;
    }

    for (int i = 0; i < prof->num_assigned_subjects; i++) {
        if (prof->assigned_subjects[i] == subject_id) {
            snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d já atribuída ao professor %s.", subject_id, prof->name);
            return -1;
        }
    }

    prof->assigned_subjects[prof->num_assigned_subjects++] = subject_id;
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Matéria com ID %d atribuída ao professor %s com sucesso!", subject_id, prof->name);
    return 0;
}

int admin_unassign_subject_from_professor(int professor_id, int subject_id, char* message_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return -1;
    }

    int found_index = -1;
    for (int i = 0; i < prof->num_assigned_subjects; i++) {
        if (prof->assigned_subjects[i] == subject_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não está atribuída ao professor %s.", subject_id, prof->name);
        return -1;
    }

    // Remove a matéria movendo os elementos seguintes
    for (int i = found_index; i < prof->num_assigned_subjects - 1; i++) {
        prof->assigned_subjects[i] = prof->assigned_subjects[i+1];
    }
    prof->num_assigned_subjects--;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Matéria com ID %d desatribuída do professor %s com sucesso!", subject_id, prof->name);
    return 0;
}

int admin_assign_class_to_professor(int professor_id, int class_id, char* message_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return -1;
    }

    Class* class_to_assign = NULL;
    for (int i = 0; i < global_data.num_classes; i++) {
        if (global_data.classes[i].id == class_id) {
            class_to_assign = &global_data.classes[i];
            break;
        }
    }
    if (!class_to_assign) {
        snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d não encontrada.", class_id);
        return -1;
    }

    if (prof->num_assigned_classes >= MAX_CLASSES_PER_PROF) {
        snprintf(message_buffer, buffer_size, "Erro: Professor já atribuído ao número máximo de turmas (%d).", MAX_CLASSES_PER_PROF);
        return -1;
    }

    for (int i = 0; i < prof->num_assigned_classes; i++) {
        if (prof->assigned_classes[i] == class_id) {
            snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d já atribuída ao professor %s.", class_id, prof->name);
            return -1;
        }
    }

    prof->assigned_classes[prof->num_assigned_classes++] = class_id;
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Turma com ID %d atribuída ao professor %s com sucesso!", class_id, prof->name);
    return 0;
}

int admin_unassign_class_from_professor(int professor_id, int class_id, char* message_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return -1;
    }

    int found_index = -1;
    for (int i = 0; i < prof->num_assigned_classes; i++) {
        if (prof->assigned_classes[i] == class_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        snprintf(message_buffer, buffer_size, "Erro: Turma com ID %d não está atribuída ao professor %s.", class_id, prof->name);
        return -1;
    }

    // Remove a turma movendo os elementos seguintes
    for (int i = found_index; i < prof->num_assigned_classes - 1; i++) {
        prof->assigned_classes[i] = prof->assigned_classes[i+1];
    }
    prof->num_assigned_classes--;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Turma com ID %d desatribuída do professor %s com sucesso!", class_id, prof->name);
    return 0;
}

// Funções de Gerenciamento de Matrículas de Alunos por Admin
int admin_enroll_student_in_subject(int student_id, int subject_id, char* message_buffer, int buffer_size) {
    Student* student = get_student_by_id_ptr(student_id);
    if (!student) {
        snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    Subject* subject = get_subject_by_id_ptr(subject_id);
    if (!subject) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return -1;
    }

    // Verificar se o aluno já está matriculado nesta matéria
    for (int j = 0; j < student->num_enrolled_subjects; ++j) {
        if (student->enrolled_subject_ids[j] == subject_id) {
            snprintf(message_buffer, buffer_size, "Erro: Aluno já matriculado na matéria '%s'.", subject->name);
            return -1;
        }
    }

    if (student->num_enrolled_subjects >= MAX_ENROLLED_SUBJECTS_PER_STUDENT) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de matérias (%d) atingido para o aluno com ID %d.", MAX_ENROLLED_SUBJECTS_PER_STUDENT, student_id);
        return -1;
    }

    student->enrolled_subject_ids[student->num_enrolled_subjects] = subject_id;
    student->num_enrolled_subjects += 1;

    if (save_data("system_data.dat") != 0) {
        student->num_enrolled_subjects -= 1;
        snprintf(message_buffer, buffer_size, "Falha ao salvar dados após matricular o aluno.");
        return -1;
    }

    snprintf(message_buffer, buffer_size, "Aluno com ID %d matriculado com sucesso na matéria '%s' (ID: %d).", student_id, subject->name, subject_id);
    return 0;
}

int admin_unenroll_student_from_subject(int student_id, int subject_id, char* message_buffer, int buffer_size) {
    Student* student = get_student_by_id_ptr(student_id);
    if (!student) {
        snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    Subject* subject = get_subject_by_id_ptr(subject_id);
    if (!subject) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return -1;
    }

    int found_index = -1;
    for (int j = 0; j < student->num_enrolled_subjects; ++j) {
        if (student->enrolled_subject_ids[j] == subject_id) {
            found_index = j;
            break;
        }
    }

    if (found_index == -1) {
        snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não está matriculado na matéria '%s' (ID: %d).", student_id, subject->name, subject_id);
        return -1;
    }

    for (int k = found_index; k < student->num_enrolled_subjects - 1; ++k) {
        student->enrolled_subject_ids[k] = student->enrolled_subject_ids[k + 1];
    }
    student->num_enrolled_subjects--;

    if (save_data("system_data.dat") != 0) {
        // Reverter em caso de falha de salvamento (simplificado)
        snprintf(message_buffer, buffer_size, "Falha ao salvar dados após desmatricular o aluno.");
        return -1;
    }

    snprintf(message_buffer, buffer_size, "Aluno com ID %d desmatriculado com sucesso da matéria '%s' (ID: %d).", student_id, subject->name, subject_id);
    return 0;
}

void admin_list_student_subjects(int student_id, char* data_buffer, int buffer_size) {
    data_buffer[0] = '\0';
    Student* student = get_student_by_id_ptr(student_id);
    if (!student) {
        snprintf(data_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return;
    }

    char temp_buffer[MAX_STR_LEN * 2];
    if (student->num_enrolled_subjects <= 0) {
        snprintf(data_buffer, buffer_size, "Aluno com ID %d não está matriculado em nenhuma matéria.\n", student_id);
        return;
    }

    snprintf(data_buffer, buffer_size, "Matérias Matriculadas para o Aluno %d (%s):\n", student->id, student->name);
    for (int i = 0; i < student->num_enrolled_subjects; ++i) {
        int sid = student->enrolled_subject_ids[i];
        Subject* subject = get_subject_by_id_ptr(sid);
        if (subject) {
            snprintf(temp_buffer, sizeof(temp_buffer), "- ID: %d, Nome: %s\n", subject->id, subject->name);
        } else {
            snprintf(temp_buffer, sizeof(temp_buffer), "- ID: %d, Nome: Desconhecida (Erro)\n", sid);
        }
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)\n");
            break;
        }
    }
}