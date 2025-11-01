#include "professor/professor.h"
#include "models.h"
#include "storage/storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h" // Para MAX_DATA_LEN e outras macros

// Implementação das funções de gerenciamento de Professores

// Funções de Gerenciamento de Matérias/Turmas
void list_professor_assigned_classes(int professor_id, char* data_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    if (prof->num_assigned_classes == 0) {
        snprintf(data_buffer, buffer_size, "Nenhuma turma atribuída ao professor %s.", prof->name);
        return;
    }

    char temp_buffer[200];
    data_buffer[0] = '\0';

    snprintf(temp_buffer, sizeof(temp_buffer), "Turmas atribuídas a %s (ID: %d):\n", prof->name, professor_id);
    if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
        strcat(data_buffer, temp_buffer);
    } else {
        snprintf(data_buffer, buffer_size, "... (lista truncada)");
        return;
    }

    for (int i = 0; i < prof->num_assigned_classes; i++) {
        int class_id = prof->assigned_classes[i];
        char class_name[MAX_STR_LEN] = "Desconhecida";
        for (int j = 0; j < global_data.num_classes; j++) {
            if (global_data.classes[j].id == class_id) {
                strncpy(class_name, global_data.classes[j].name, MAX_STR_LEN - 1);
                class_name[MAX_STR_LEN - 1] = '\0';
                break;
            }
        }
        snprintf(temp_buffer, sizeof(temp_buffer), "  - ID: %d, Nome: %s\n", class_id, class_name);
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)");
            break;
        }
    }
}

void list_professor_assigned_subjects(int professor_id, char* data_buffer, int buffer_size) {
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    if (prof->num_assigned_subjects == 0) {
        snprintf(data_buffer, buffer_size, "Nenhuma matéria atribuída ao professor %s.", prof->name);
        return;
    }

    char temp_buffer[200];
    data_buffer[0] = '\0';

    snprintf(temp_buffer, sizeof(temp_buffer), "Matérias atribuídas a %s (ID: %d):\n", prof->name, professor_id);
    if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
        strcat(data_buffer, temp_buffer);
    } else {
        snprintf(data_buffer, buffer_size, "... (lista truncada)");
        return;
    }

    for (int i = 0; i < prof->num_assigned_subjects; i++) {
        int subject_id = prof->assigned_subjects[i];
        char subject_name[MAX_STR_LEN] = "Desconhecida";
        for (int j = 0; j < global_data.num_subjects; j++) {
            if (global_data.subjects[j].id == subject_id) {
                strncpy(subject_name, global_data.subjects[j].name, MAX_STR_LEN - 1);
                subject_name[MAX_STR_LEN - 1] = '\0';
                break;
            }
        }
        snprintf(temp_buffer, sizeof(temp_buffer), "  - ID: %d, Nome: %s\n", subject_id, subject_name);
        if (strlen(data_buffer) + strlen(temp_buffer) < (size_t)buffer_size) {
            strcat(data_buffer, temp_buffer);
        } else {
            strcat(data_buffer, "... (lista truncada)");
            break;
        }
    }
}

// Funções de Notas e Faltas
int post_grade(int professor_id, int student_id, int subject_id, float grade_value, const char* description, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Verificar se o aluno existe
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; i++) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    // Verificar se a matéria existe (e se o professor está associado a ela)
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }
    if (!subject) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return -1;
    }

    // Verificar limite de notas para o aluno
    if (global_data.num_grades >= MAX_GRADES_PER_STUDENT * MAX_STUDENTS) {
        snprintf(message_buffer, buffer_size, "Erro: Limite total de notas atingido no sistema.");
        return -1;
    }

    // Adicionar a nota
    Grade new_grade;
    new_grade.student_id = student_id;
    new_grade.subject_id = subject_id;
    new_grade.grade_value = grade_value;
    strncpy(new_grade.description, description, MAX_STR_LEN - 1);
    new_grade.description[MAX_STR_LEN - 1] = '\0';

    global_data.grades[global_data.num_grades++] = new_grade;
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Nota %.2f lançada para o aluno %s na matéria %s com sucesso!", grade_value, student->name, subject->name);
    return 0;
}

int register_absence(int professor_id, int student_id, int subject_id, const char* date, const char* justification, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Verificar se o aluno existe
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; i++) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(message_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    // Verificar se a matéria existe (e se o professor está associado a ela)
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }
    if (!subject) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return -1;
    }

    // Verificar limite de faltas para o aluno
    if (global_data.num_absences >= MAX_ABSENCES_PER_STUDENT * MAX_STUDENTS) {
        snprintf(message_buffer, buffer_size, "Erro: Limite total de faltas atingido no sistema.");
        return -1;
    }

    // Adicionar a falta
    Absence new_absence;
    new_absence.student_id = student_id;
    new_absence.subject_id = subject_id;
    strncpy(new_absence.date, date, MAX_STR_LEN - 1);
    new_absence.date[MAX_STR_LEN - 1] = '\0';
    strncpy(new_absence.justification, justification, MAX_STR_LEN - 1);
    new_absence.justification[MAX_STR_LEN - 1] = '\0';

    global_data.absences[global_data.num_absences++] = new_absence;
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Falta registrada para o aluno %s na matéria %s em %s com sucesso!", student->name, subject->name, date);
    return 0;
}

void view_student_grades_absences(int professor_id, int student_id, char* data_buffer, int buffer_size) {
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    // Verificar se o professor existe
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    // Verificar se o aluno existe
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; i++) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return;
    }

    char temp_buffer[MAX_DATA_LEN]; // Usar um buffer auxiliar grande o suficiente
    int offset = 0;

    offset += snprintf(data_buffer + offset, buffer_size - offset, "Notas e Faltas de %s (ID: %d):\n", student->name, student_id);

    // Listar Notas
    offset += snprintf(data_buffer + offset, buffer_size - offset, "--- Notas ---\n");
    int grades_found = 0;
    for (int i = 0; i < global_data.num_grades; i++) {
        if (global_data.grades[i].student_id == student_id) {
            Subject* subject = NULL;
            for (int j = 0; j < global_data.num_subjects; j++) {
                if (global_data.subjects[j].id == global_data.grades[i].subject_id) {
                    subject = &global_data.subjects[j];
                    break;
                }
            }
            snprintf(temp_buffer, sizeof(temp_buffer), "  - Matéria: %s (ID: %d), Nota: %.2f, Descrição: %s\n",
                     subject ? subject->name : "Desconhecida",
                     global_data.grades[i].subject_id,
                     global_data.grades[i].grade_value,
                     global_data.grades[i].description);
            if ((size_t)offset + strlen(temp_buffer) < (size_t)buffer_size) {
                strcat(data_buffer + offset, temp_buffer);
                offset += strlen(temp_buffer);
            } else { break; }
            grades_found = 1;
        }
    }
    if (!grades_found) {
        offset += snprintf(data_buffer + offset, buffer_size - offset, "  Nenhuma nota registrada.\n");
    }

    // Listar Faltas
    offset += snprintf(data_buffer + offset, buffer_size - offset, "--- Faltas ---\n");
    int absences_found = 0;
    for (int i = 0; i < global_data.num_absences; i++) {
        if (global_data.absences[i].student_id == student_id) {
            Subject* subject = NULL;
            for (int j = 0; j < global_data.num_subjects; j++) {
                if (global_data.subjects[j].id == global_data.absences[i].subject_id) {
                    subject = &global_data.subjects[j];
                    break;
                }
            }
            snprintf(temp_buffer, sizeof(temp_buffer), "  - Matéria: %s (ID: %d), Data: %s, Justificativa: %s\n",
                     subject ? subject->name : "Desconhecida",
                     global_data.absences[i].subject_id,
                     global_data.absences[i].date,
                     global_data.absences[i].justification[0] != '\0' ? global_data.absences[i].justification : "Nenhuma");
            if ((size_t)offset + strlen(temp_buffer) < (size_t)buffer_size) {
                strcat(data_buffer + offset, temp_buffer);
                offset += strlen(temp_buffer);
            } else {
                strcat(data_buffer, "... (lista truncada)");
                break;
            }
        }
    }
    if (!absences_found) {
        offset += snprintf(data_buffer + offset, buffer_size - offset, "  Nenhuma falta registrada.\n");
    }
}

// Funções de Módulos e Aulas
int create_subject_module(int professor_id, int subject_id, const char* module_name, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Verificar se a matéria existe
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }
    if (!subject) {
        snprintf(message_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria
    int is_prof_assigned_to_subject = 0;
    for (int i = 0; i < prof->num_assigned_subjects; i++) {
        if (prof->assigned_subjects[i] == subject_id) {
            is_prof_assigned_to_subject = 1;
            break;
        }
    }
    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria com ID %d.", professor_id, subject_id);
        return -1;
    }

    // Verificar limite de módulos para a matéria
    if (subject->num_modules >= MAX_MODULES_PER_SUBJECT) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de módulos atingido para a matéria %s.", subject->name);
        return -1;
    }

    // Verificar limite total de módulos
    if (global_data.num_modules >= (int)(sizeof(global_data.modules) / sizeof(Module))) {
        snprintf(message_buffer, buffer_size, "Erro: Limite total de módulos atingido no sistema.");
        return -1;
    }

    // Criar novo módulo
    Module new_module;
    new_module.id = (global_data.num_modules > 0) ? global_data.modules[global_data.num_modules - 1].id + 1 : 1; // ID incremental
    strncpy(new_module.name, module_name, MAX_STR_LEN - 1);
    new_module.name[MAX_STR_LEN - 1] = '\0';
    new_module.subject_id = subject_id;
    new_module.num_lessons = 0;
    new_module.num_quizzes = 0;

    global_data.modules[global_data.num_modules] = new_module;
    subject->module_ids[subject->num_modules++] = new_module.id;
    global_data.num_modules++;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Módulo \"%s\" criado com sucesso na matéria %s! (ID: %d)", new_module.name, subject->name, new_module.id);
    return 0;
}

void list_subject_modules(int professor_id, int subject_id, char* data_buffer, int buffer_size) {
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    // Verificar se o professor existe
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    // Verificar se a matéria existe
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }
    if (!subject) {
        snprintf(data_buffer, buffer_size, "Erro: Matéria com ID %d não encontrada.", subject_id);
        return;
    }

    // Verificar se o professor está associado à matéria
    int is_prof_assigned_to_subject = 0;
    for (int i = 0; i < prof->num_assigned_subjects; i++) {
        if (prof->assigned_subjects[i] == subject_id) {
            is_prof_assigned_to_subject = 1;
            break;
        }
    }
    if (!is_prof_assigned_to_subject) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria com ID %d.", professor_id, subject_id);
        return;
    }

    if (subject->num_modules == 0) {
        snprintf(data_buffer, buffer_size, "Nenhum módulo cadastrado para a matéria %s (ID: %d).", subject->name, subject_id);
        return;
    }

    char temp_buffer[200];
    int offset = 0;

    offset += snprintf(data_buffer + offset, buffer_size - offset, "Módulos da matéria %s (ID: %d):\n", subject->name, subject_id);

    for (int i = 0; i < subject->num_modules; i++) {
        int module_id = subject->module_ids[i];
        Module* module = NULL;
        for (int j = 0; j < global_data.num_modules; j++) {
            if (global_data.modules[j].id == module_id) {
                module = &global_data.modules[j];
                break;
            }
        }

        if (module) {
            snprintf(temp_buffer, sizeof(temp_buffer), "  - ID: %d, Nome: %s, Aulas: %d, Questionários: %d\n",
                     module->id, module->name, module->num_lessons, module->num_quizzes);
            if ((size_t)offset + strlen(temp_buffer) < (size_t)buffer_size) {
                strcat(data_buffer + offset, temp_buffer);
                offset += strlen(temp_buffer);
            } else {
                strcat(data_buffer, "... (lista truncada)");
                break;
            }
        }
    }
}

int edit_module(int professor_id, int module_id, const char* new_name, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o módulo
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    strncpy(module->name, new_name, MAX_STR_LEN - 1);
    module->name[MAX_STR_LEN - 1] = '\0';
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Módulo com ID %d atualizado para \"%s\" com sucesso!", module_id, new_name);
    return 0;
}

int remove_module(int professor_id, int module_id, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o módulo e sua posição em global_data.modules
    int module_index_in_global_data = -1;
    Module* module_to_remove = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module_to_remove = &global_data.modules[i];
            module_index_in_global_data = i;
            break;
        }
    }
    if (!module_to_remove) {
        snprintf(message_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module_to_remove->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module_to_remove->subject_id);
        return -1;
    }

    // Remover o módulo do array de módulos da matéria
    if (subject) {
        int found_module_in_subject_index = -1;
        for (int i = 0; i < subject->num_modules; i++) {
            if (subject->module_ids[i] == module_id) {
                found_module_in_subject_index = i;
                break;
            }
        }
        if (found_module_in_subject_index != -1) {
            for (int i = found_module_in_subject_index; i < subject->num_modules - 1; i++) {
                subject->module_ids[i] = subject->module_ids[i+1];
            }
            subject->num_modules--;
        }
    }

    // Remover o módulo de global_data.modules (e todas as suas aulas e questionários)
    if (module_to_remove) {
        // Remover aulas associadas
        for (int i = 0; i < module_to_remove->num_lessons; i++) {
            int lesson_id = module_to_remove->lesson_ids[i];
            // Encontrar e remover a aula de global_data.lessons
            for (int j = 0; j < global_data.num_lessons; j++) {
                if (global_data.lessons[j].id == lesson_id) {
                    for (int k = j; k < global_data.num_lessons - 1; k++) {
                        global_data.lessons[k] = global_data.lessons[k+1];
                    }
                    global_data.num_lessons--;
                    j--; // Ajustar o índice devido à remoção
                }
            }
        }

        // Remover questionários associados
        for (int i = 0; i < module_to_remove->num_quizzes; i++) {
            int quiz_id = module_to_remove->quiz_ids[i];
            // Encontrar e remover o questionário de global_data.quizzes
            // (e também suas perguntas associadas)
            for (int j = 0; j < global_data.num_quizzes; j++) {
                if (global_data.quizzes[j].id == quiz_id) {
                    // Remover perguntas do quiz (elas são inline na struct Quiz)
                    // Não é necessário um loop separado para perguntas aqui
                    for (int k = j; k < global_data.num_quizzes - 1; k++) {
                        global_data.quizzes[k] = global_data.quizzes[k+1];
                    }
                    global_data.num_quizzes--;
                    j--; // Ajustar o índice devido à remoção
                }
            }
        }

        // Remover o módulo do array global_data.modules
        if (module_index_in_global_data != -1) {
            for (int i = module_index_in_global_data; i < global_data.num_modules - 1; i++) {
                global_data.modules[i] = global_data.modules[i+1];
            }
            global_data.num_modules--;
        }
    }

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Módulo com ID %d e seus conteúdos removidos com sucesso!", module_id);
    return 0;
}

int add_lesson_to_module(int professor_id, int module_id, const char* lesson_title, const char* youtube_link, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o módulo
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Verificar limite de aulas para o módulo
    if (module->num_lessons >= MAX_LESSONS_PER_MODULE) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de aulas atingido para o módulo \"%s\".", module->name);
        return -1;
    }

    // Verificar limite total de aulas
    if (global_data.num_lessons >= (int)(sizeof(global_data.lessons) / sizeof(Lesson))) {
        snprintf(message_buffer, buffer_size, "Erro: Limite total de aulas atingido no sistema.");
        return -1;
    }

    // Criar nova aula
    Lesson new_lesson;
    new_lesson.id = (global_data.num_lessons > 0) ? global_data.lessons[global_data.num_lessons - 1].id + 1 : 1; // ID incremental
    strncpy(new_lesson.title, lesson_title, MAX_STR_LEN - 1);
    new_lesson.title[MAX_STR_LEN - 1] = '\0';
    strncpy(new_lesson.youtube_link, youtube_link, MAX_STR_LEN - 1);
    new_lesson.youtube_link[MAX_STR_LEN - 1] = '\0';
    new_lesson.module_id = module_id;

    global_data.lessons[global_data.num_lessons] = new_lesson;
    module->lesson_ids[module->num_lessons++] = new_lesson.id;
    global_data.num_lessons++;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Aula \"%s\" adicionada ao módulo \"%s\" com sucesso! (ID: %d)", new_lesson.title, module->name, new_lesson.id);
    return 0;
}

int edit_lesson(int professor_id, int lesson_id, const char* new_title, const char* new_youtube_link, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar a aula
    Lesson* lesson = NULL;
    for (int i = 0; i < global_data.num_lessons; i++) {
        if (global_data.lessons[i].id == lesson_id) {
            lesson = &global_data.lessons[i];
            break;
        }
    }
    if (!lesson) {
        snprintf(message_buffer, buffer_size, "Erro: Aula com ID %d não encontrada.", lesson_id);
        return -1;
    }

    // Encontrar o módulo da aula
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == lesson->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo da aula com ID %d não encontrado.", lesson->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo da aula
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo da aula com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    strncpy(lesson->title, new_title, MAX_STR_LEN - 1);
    lesson->title[MAX_STR_LEN - 1] = '\0';
    strncpy(lesson->youtube_link, new_youtube_link, MAX_STR_LEN - 1);
    lesson->youtube_link[MAX_STR_LEN - 1] = '\0';
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Aula com ID %d atualizada para \"%s\" com sucesso!", lesson_id, new_title);
    return 0;
}

int remove_lesson(int professor_id, int lesson_id, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar a aula e sua posição em global_data.lessons
    int lesson_index_in_global_data = -1;
    Lesson* lesson_to_remove = NULL;
    for (int i = 0; i < global_data.num_lessons; i++) {
        if (global_data.lessons[i].id == lesson_id) {
            lesson_to_remove = &global_data.lessons[i];
            lesson_index_in_global_data = i;
            break;
        }
    }
    if (!lesson_to_remove) {
        snprintf(message_buffer, buffer_size, "Erro: Aula com ID %d não encontrada.", lesson_id);
        return -1;
    }

    // Encontrar o módulo da aula
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == lesson_to_remove->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo da aula com ID %d não encontrado.", lesson_to_remove->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo da aula
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo da aula com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Remover a aula do array de aulas do módulo
    if (module) {
        int found_lesson_in_module_index = -1;
        for (int i = 0; i < module->num_lessons; i++) {
            if (module->lesson_ids[i] == lesson_id) {
                found_lesson_in_module_index = i;
                break;
            }
        }
        if (found_lesson_in_module_index != -1) {
            for (int i = found_lesson_in_module_index; i < module->num_lessons - 1; i++) {
                module->lesson_ids[i] = module->lesson_ids[i+1];
            }
            module->num_lessons--;
        }
    }

    // Remover a aula de global_data.lessons
    if (lesson_index_in_global_data != -1) {
        for (int i = lesson_index_in_global_data; i < global_data.num_lessons - 1; i++) {
            global_data.lessons[i] = global_data.lessons[i+1];
        }
        global_data.num_lessons--;
    }

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Aula com ID %d removida com sucesso do módulo \"%s\"!", lesson_id, module->name);
    return 0;
}

void list_module_lessons(int professor_id, int module_id, char* data_buffer, int buffer_size) {
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    // Verificar se o professor existe
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    // Encontrar o módulo
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(data_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module->subject_id);
        return;
    }

    if (module->num_lessons == 0) {
        snprintf(data_buffer, buffer_size, "Nenhuma aula cadastrada para o módulo \"%s\" (ID: %d).", module->name, module_id);
        return;
    }

    char temp_buffer[256];
    int offset = 0;

    offset += snprintf(data_buffer + offset, buffer_size - offset, "Aulas do módulo \"%s\" (ID: %d):\n", module->name, module_id);

    for (int i = 0; i < module->num_lessons; i++) {
        int lesson_id = module->lesson_ids[i];
        Lesson* lesson = NULL;
        for (int j = 0; j < global_data.num_lessons; j++) {
            if (global_data.lessons[j].id == lesson_id) {
                lesson = &global_data.lessons[j];
                break;
            }
        }

        if (lesson) {
            snprintf(temp_buffer, sizeof(temp_buffer), "  - ID: %d, Título: %s, Link: %s\n",
                     lesson->id, lesson->title, lesson->youtube_link);
            if ((size_t)offset + strlen(temp_buffer) < (size_t)buffer_size) {
                strcat(data_buffer + offset, temp_buffer);
                offset += strlen(temp_buffer);
            } else {
                strcat(data_buffer, "... (lista truncada)");
                break;
            }
        }
    }
}

// Funções de Questionários
int create_module_quiz(int professor_id, int module_id, const char* quiz_title, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o módulo
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Verificar limite de questionários para o módulo
    if (module->num_quizzes >= MAX_QUIZZES_PER_MODULE) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de questionários atingido para o módulo \"%s\".", module->name);
        return -1;
    }

    // Verificar limite total de questionários
    if (global_data.num_quizzes >= (int)(sizeof(global_data.quizzes) / sizeof(Quiz))) {
        snprintf(message_buffer, buffer_size, "Erro: Limite total de questionários atingido no sistema.");
        return -1;
    }

    // Criar novo questionário
    Quiz new_quiz;
    new_quiz.id = (global_data.num_quizzes > 0) ? global_data.quizzes[global_data.num_quizzes - 1].id + 1 : 1; // ID incremental
    strncpy(new_quiz.title, quiz_title, MAX_STR_LEN - 1);
    new_quiz.title[MAX_STR_LEN - 1] = '\0';
    new_quiz.module_id = module_id;
    new_quiz.num_questions = 0;

    global_data.quizzes[global_data.num_quizzes] = new_quiz;
    module->quiz_ids[module->num_quizzes++] = new_quiz.id;
    global_data.num_quizzes++;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Questionário \"%s\" criado com sucesso no módulo \"%s\"! (ID: %d)", new_quiz.title, module->name, new_quiz.id);
    return 0;
}

int edit_quiz(int professor_id, int quiz_id, const char* new_title, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o questionário
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; i++) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;
        }
    }
    if (!quiz) {
        snprintf(message_buffer, buffer_size, "Erro: Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // Encontrar o módulo do questionário
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == quiz->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo do questionário com ID %d não encontrado.", quiz->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo do questionário
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo do questionário com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    strncpy(quiz->title, new_title, MAX_STR_LEN - 1);
    quiz->title[MAX_STR_LEN - 1] = '\0';
    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Questionário com ID %d atualizado para \"%s\" com sucesso!", quiz_id, new_title);
    return 0;
}

int remove_quiz(int professor_id, int quiz_id, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o questionário e sua posição em global_data.quizzes
    int quiz_index_in_global_data = -1;
    Quiz* quiz_to_remove = NULL;
    for (int i = 0; i < global_data.num_quizzes; i++) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz_to_remove = &global_data.quizzes[i];
            quiz_index_in_global_data = i;
            break;
        }
    }
    if (!quiz_to_remove) {
        snprintf(message_buffer, buffer_size, "Erro: Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // Encontrar o módulo do questionário
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == quiz_to_remove->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo do questionário com ID %d não encontrado.", quiz_to_remove->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo do questionário
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo do questionário com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Remover o questionário do array de questionários do módulo
    if (module) {
        int found_quiz_in_module_index = -1;
        for (int i = 0; i < module->num_quizzes; i++) {
            if (module->quiz_ids[i] == quiz_id) {
                found_quiz_in_module_index = i;
                break;
            }
        }
        if (found_quiz_in_module_index != -1) {
            for (int i = found_quiz_in_module_index; i < module->num_quizzes - 1; i++) {
                module->quiz_ids[i] = module->quiz_ids[i+1];
            }
            module->num_quizzes--;
        }
    }

    // Remover o questionário de global_data.quizzes
    if (quiz_index_in_global_data != -1) {
        for (int i = quiz_index_in_global_data; i < global_data.num_quizzes - 1; i++) {
            global_data.quizzes[i] = global_data.quizzes[i+1];
        }
        global_data.num_quizzes--;
    }

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Questionário com ID %d removido com sucesso do módulo \"%s\"!", quiz_id, module->name);
    return 0;
}

int add_question_to_quiz(int professor_id, int quiz_id, const char* question_text, const char* option1, const char* option2, const char* option3, const char* option4, int correct_option, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o questionário
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; i++) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;
        }
    }
    if (!quiz) {
        snprintf(message_buffer, buffer_size, "Erro: Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // Encontrar o módulo do questionário
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == quiz->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo do questionário com ID %d não encontrado.", quiz->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo do questionário
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo do questionário com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Verificar limite de perguntas para o questionário
    if (quiz->num_questions >= MAX_QUESTIONS_PER_QUIZ) {
        snprintf(message_buffer, buffer_size, "Erro: Limite de perguntas atingido para o questionário \"%s\".", quiz->title);
        return -1;
    }

    // Validar opção correta
    if (correct_option < 0 || correct_option > 3) {
        snprintf(message_buffer, buffer_size, "Erro: Opção correta deve ser entre 0 e 3.");
        return -1;
    }

    // Adicionar nova pergunta
    Question new_question;
    // ID da pergunta pode ser incremental dentro do quiz ou global se necessário
    // Por simplicidade, usaremos o índice dentro do quiz como ID temporário
    new_question.id = quiz->num_questions;
    strncpy(new_question.text, question_text, MAX_STR_LEN - 1);
    new_question.text[MAX_STR_LEN - 1] = '\0';
    strncpy(new_question.options[0], option1, MAX_STR_LEN - 1);
    new_question.options[0][MAX_STR_LEN - 1] = '\0';
    strncpy(new_question.options[1], option2, MAX_STR_LEN - 1);
    new_question.options[1][MAX_STR_LEN - 1] = '\0';
    strncpy(new_question.options[2], option3, MAX_STR_LEN - 1);
    new_question.options[2][MAX_STR_LEN - 1] = '\0';
    strncpy(new_question.options[3], option4, MAX_STR_LEN - 1);
    new_question.options[3][MAX_STR_LEN - 1] = '\0';
    new_question.correct_option = correct_option;

    quiz->questions[quiz->num_questions++] = new_question;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Pergunta adicionada ao questionário \"%s\" com sucesso!", quiz->title);
    return 0;
}

int edit_question(int professor_id, int quiz_id, int question_id, const char* new_text, const char* new_option1, const char* new_option2, const char* new_option3, const char* new_option4, int new_correct_option, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o questionário
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; i++) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;
        }
    }
    if (!quiz) {
        snprintf(message_buffer, buffer_size, "Erro: Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // Encontrar o módulo do questionário
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == quiz->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo do questionário com ID %d não encontrado.", quiz->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo do questionário
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo do questionário com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Encontrar a pergunta
    Question* question = NULL;
    if (question_id >= 0 && question_id < quiz->num_questions) {
        question = &quiz->questions[question_id];
    }
    if (!question) {
        snprintf(message_buffer, buffer_size, "Erro: Pergunta com ID %d não encontrada no questionário %s.", question_id, quiz->title);
        return -1;
    }

    // Validar nova opção correta
    if (new_correct_option < 0 || new_correct_option > 3) {
        snprintf(message_buffer, buffer_size, "Erro: Nova opção correta deve ser entre 0 e 3.");
        return -1;
    }

    strncpy(question->text, new_text, MAX_STR_LEN - 1);
    question->text[MAX_STR_LEN - 1] = '\0';
    strncpy(question->options[0], new_option1, MAX_STR_LEN - 1);
    question->options[0][MAX_STR_LEN - 1] = '\0';
    strncpy(question->options[1], new_option2, MAX_STR_LEN - 1);
    question->options[1][MAX_STR_LEN - 1] = '\0';
    strncpy(question->options[2], new_option3, MAX_STR_LEN - 1);
    question->options[2][MAX_STR_LEN - 1] = '\0';
    strncpy(question->options[3], new_option4, MAX_STR_LEN - 1);
    question->options[3][MAX_STR_LEN - 1] = '\0';
    question->correct_option = new_correct_option;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Pergunta com ID %d do questionário \"%s\" atualizada com sucesso!", question_id, quiz->title);
    return 0;
}

int remove_question(int professor_id, int quiz_id, int question_id, char* message_buffer, int buffer_size) {
    // Verificar se o professor existe
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

    // Encontrar o questionário
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; i++) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;;
        }
    }
    if (!quiz) {
        snprintf(message_buffer, buffer_size, "Erro: Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // Encontrar o módulo do questionário
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == quiz->module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(message_buffer, buffer_size, "Erro interno: Módulo do questionário com ID %d não encontrado.", quiz->module_id);
        return -1;
    }

    // Verificar se o professor está associado à matéria do módulo do questionário
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(message_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo do questionário com ID %d.", professor_id, module->subject_id);
        return -1;
    }

    // Remover a pergunta do array de perguntas do questionário
    if (question_id < 0 || question_id >= quiz->num_questions) {
        snprintf(message_buffer, buffer_size, "Erro: Pergunta com ID %d não encontrada no questionário %s.", question_id, quiz->title);
        return -1;
    }

    for (int i = question_id; i < quiz->num_questions - 1; i++) {
        quiz->questions[i] = quiz->questions[i+1];
    }
    quiz->num_questions--;

    save_data("system_data.dat");
    snprintf(message_buffer, buffer_size, "Pergunta com ID %d removida com sucesso do questionário \"%s\"!", question_id, quiz->title);
    return 0;
}

void list_module_quizzes(int professor_id, int module_id, char* data_buffer, int buffer_size) {
    data_buffer[0] = '\0'; // Garante que o buffer de saída comece vazio

    // Verificar se o professor existe
    Professor* prof = NULL;
    for (int i = 0; i < global_data.num_professors; i++) {
        if (global_data.professors[i].id == professor_id) {
            prof = &global_data.professors[i];
            break;
        }
    }
    if (!prof) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não encontrado.", professor_id);
        return;
    }

    // Encontrar o módulo
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; i++) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(data_buffer, buffer_size, "Erro: Módulo com ID %d não encontrado.", module_id);
        return;
    }

    // Verificar se o professor está associado à matéria do módulo
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; i++) {
        if (global_data.subjects[i].id == module->subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }

    int is_prof_assigned_to_subject = 0;
    if (subject) {
        for (int i = 0; i < prof->num_assigned_subjects; i++) {
            if (prof->assigned_subjects[i] == subject->id) {
                is_prof_assigned_to_subject = 1;
                break;
            }
        }
    }

    if (!is_prof_assigned_to_subject) {
        snprintf(data_buffer, buffer_size, "Erro: Professor com ID %d não está atribuído à matéria do módulo com ID %d.", professor_id, module->subject_id);
        return;
    }

    if (module->num_quizzes == 0) {
        snprintf(data_buffer, buffer_size, "Nenhum questionário cadastrado para o módulo \"%s\" (ID: %d).", module->name, module_id);
        return;
    }

    char temp_buffer[256];
    int offset = 0;

    offset += snprintf(data_buffer + offset, buffer_size - offset, "Questionários do módulo \"%s\" (ID: %d):\n", module->name, module_id);

    for (int i = 0; i < module->num_quizzes; i++) {
        int quiz_id = module->quiz_ids[i];
        Quiz* quiz = NULL;
        for (int j = 0; j < global_data.num_quizzes; j++) {
            if (global_data.quizzes[j].id == quiz_id) {
                quiz = &global_data.quizzes[j];
                break;
            }
        }

        if (quiz) {
            snprintf(temp_buffer, sizeof(temp_buffer), "  - ID: %d, Título: %s\n",
                     quiz->id, quiz->title);
            if ((size_t)offset + strlen(temp_buffer) < (size_t)buffer_size) {
                strcat(data_buffer + offset, temp_buffer);
                offset += strlen(temp_buffer);
            } else {
                strcat(data_buffer, "... (lista truncada)");
                break;
            }
        }
    }
}