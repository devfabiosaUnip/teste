#include "student/student.h"
#include "storage/storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Implementações das funções de gerenciamento de Alunos

// Funções de gerenciamento de matérias para alunos - MOVIDAS PARA ADMIN

// Função auxiliar para obter um ponteiro para o módulo pelo ID
Module* get_module_by_id_ptr(int module_id) {
    for (int i = 0; i < global_data.num_modules; ++i) {
        if (global_data.modules[i].id == module_id) {
            return &global_data.modules[i];
        }
    }
    return NULL;
}

// Função auxiliar para obter um ponteiro para a aula pelo ID
Lesson* get_lesson_by_id_ptr(int lesson_id) {
    for (int i = 0; i < global_data.num_lessons; ++i) {
        if (global_data.lessons[i].id == lesson_id) {
            return &global_data.lessons[i];
        }
    }
    return NULL;
}

// Função auxiliar para obter um ponteiro para o quiz pelo ID
Quiz* get_quiz_by_id_ptr(int quiz_id) {
    for (int i = 0; i < global_data.num_quizzes; ++i) {
        if (global_data.quizzes[i].id == quiz_id) {
            return &global_data.quizzes[i];
        }
    }
    return NULL;
}

void student_list_modules_in_subject(int student_id, int subject_id, char* data_buffer, int buffer_size) {
    /* Inicializa buffer */
    if (buffer_size > 0 && data_buffer) data_buffer[0] = '\0';
    if (!data_buffer || buffer_size <= 0) return;

    /* Encontra aluno */
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Aluno com id %d não encontrado.", student_id);
        return;
    }

    /* Encontra matéria */
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; ++i) {
        if (global_data.subjects[i].id == subject_id) {
            subject = &global_data.subjects[i];
            break;
        }
    }
    if (!subject) {
        snprintf(data_buffer, buffer_size, "Matéria com id %d não encontrada.", subject_id);
        return;
    }

    /* Verifica matrícula do aluno na matéria */
    int enrolled = 0;
    for (int i = 0; i < student->num_enrolled_subjects; ++i) {
        if (student->enrolled_subject_ids[i] == subject_id) {
            enrolled = 1;
            break;
        }
    }
    if (!enrolled) {
        snprintf(data_buffer, buffer_size, "Aluno id=%d não está matriculado na matéria id=%d.", student_id, subject_id);
        return;
    }

    /* Verifica se há módulos na matéria */
    if (subject->num_modules <= 0) {
        snprintf(data_buffer, buffer_size, "Matéria id=%d não possui módulos cadastrados.", subject_id);
        return;
    }

    /* Percorre módulos e formata no buffer com cuidado para não estourar */
    int used = 0;
    for (int i = 0; i < subject->num_modules; ++i) {
        int mid = subject->module_ids[i];
        Module* module = NULL;
        for (int j = 0; j < global_data.num_modules; ++j) {
            if (global_data.modules[j].id == mid) {
                module = &global_data.modules[j];
                break;
            }
        }

        const char* mname = "(sem nome)";
        int n_lessons = 0;
        int n_quizzes = 0;
        if (module) {
            if (strlen(module->name) > 0) mname = module->name;
            /* tenta usar campos convencionais; se não existirem, ficam em 0 */
            n_lessons = module->num_lessons;
            n_quizzes = module->num_quizzes;
        } else {
            /* módulo não encontrado globalmente */
            mname = "(módulo não encontrado)";
        }

        int remaining = buffer_size - used;
        if (remaining <= 1) break;

        int n = snprintf(data_buffer + used, remaining, "ID %d - %s | Aulas: %d | Questionários: %d\n", mid, mname, n_lessons, n_quizzes);
        if (n < 0) break;

        if (n >= remaining) {
            const char ell[] = "...(truncated)";
            size_t ell_len = strlen(ell);
            if (buffer_size > (int)ell_len + 1) {
                int pos = buffer_size - (int)ell_len - 1;
                memcpy(data_buffer + pos, ell, ell_len);
                data_buffer[buffer_size - 1] = '\0';
            }
            return;
        }

        used += n;
    }
    /* buffer contém a lista (ou parte dela) já formatada */
}

void student_view_lessons_in_module(int student_id, int module_id, char* data_buffer, int buffer_size) {
    /* Inicializa buffer */
    if (buffer_size > 0 && data_buffer) data_buffer[0] = '\0';
    if (!data_buffer || buffer_size <= 0) return;

    /* Encontra aluno */
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Aluno com id %d não encontrado.", student_id);
        return;
    }

    /* Encontra módulo */
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; ++i) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(data_buffer, buffer_size, "Módulo com id %d não encontrado.", module_id);
        return;
    }

    /* Encontra matéria que contém o módulo */
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; ++i) {
        Subject* s = &global_data.subjects[i];
        for (int j = 0; j < s->num_modules; ++j) {
            if (s->module_ids[j] == module_id) {
                subject = s;
                break;
            }
        }
        if (subject) break;
    }
    if (!subject) {
        snprintf(data_buffer, buffer_size, "Matéria do módulo id=%d não encontrada.", module_id);
        return;
    }

    /* Verifica matrícula do aluno na matéria */
    int enrolled = 0;
    for (int i = 0; i < student->num_enrolled_subjects; ++i) {
        if (student->enrolled_subject_ids[i] == subject->id) {
            enrolled = 1;
            break;
        }
    }
    if (!enrolled) {
        snprintf(data_buffer, buffer_size, "Aluno id=%d não está matriculado na matéria id=%d.", student_id, subject->id);
        return;
    }

    /* Verifica se há aulas no módulo */
    if (module->num_lessons <= 0) {
        snprintf(data_buffer, buffer_size, "Módulo id=%d não possui aulas cadastradas.", module_id);
        return;
    }

    /* Percorre aulas e formata no buffer com cuidado para não estourar */
    int used = 0;
    for (int i = 0; i < module->num_lessons; ++i) {
        int lid = module->lesson_ids[i];
        Lesson* lesson = NULL;
        for (int j = 0; j < global_data.num_lessons; ++j) {
            if (global_data.lessons[j].id == lid) {
                lesson = &global_data.lessons[j];
                break;
            }
        }

        const char* title = "(sem título)";
        const char* yt = "(sem link)";
        if (lesson) {
            if (strlen(lesson->title) > 0) title = lesson->title;
            if (strlen(lesson->youtube_link) > 0) yt = lesson->youtube_link;
        } else {
            title = "(aula não encontrada)";
            yt = "(sem link)";
        }

        int remaining = buffer_size - used;
        if (remaining <= 1) break;

        int n = snprintf(data_buffer + used, remaining, "ID %d - %s | YouTube: %s\n", lid, title, yt);
        if (n < 0) break;

        if (n >= remaining) {
            const char ell[] = "...(truncated)";
            size_t ell_len = strlen(ell);
            if (buffer_size > (int)ell_len + 1) {
                int pos = buffer_size - (int)ell_len - 1;
                memcpy(data_buffer + pos, ell, ell_len);
                data_buffer[buffer_size - 1] = '\0';
            }
            return;
        }

        used += n;
    }
    /* buffer contém a lista (ou parte dela) já formatada */
}

void student_view_quizzes_in_module(int student_id, int module_id, char* data_buffer, int buffer_size) {
    /* Inicializa buffer */
    if (buffer_size > 0 && data_buffer) data_buffer[0] = '\0';
    if (!data_buffer || buffer_size <= 0) return;

    /* Encontra aluno */
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Aluno com id %d não encontrado.", student_id);
        return;
    }

    /* Encontra módulo */
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; ++i) {
        if (global_data.modules[i].id == module_id) {
            module = &global_data.modules[i];
            break;
        }
    }
    if (!module) {
        snprintf(data_buffer, buffer_size, "Módulo com id %d não encontrado.", module_id);
        return;
    }

    /* Encontra matéria que contém o módulo */
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; ++i) {
        Subject* s = &global_data.subjects[i];
        for (int j = 0; j < s->num_modules; ++j) {
            if (s->module_ids[j] == module_id) {
                subject = s;
                break;
            }
        }
        if (subject) break;
    }
    if (!subject) {
        snprintf(data_buffer, buffer_size, "Matéria do módulo id=%d não encontrada.", module_id);
        return;
    }

    /* Verifica matrícula do aluno na matéria */
    int enrolled = 0;
    for (int i = 0; i < student->num_enrolled_subjects; ++i) {
        if (student->enrolled_subject_ids[i] == subject->id) {
            enrolled = 1;
            break;
        }
    }
    if (!enrolled) {
        snprintf(data_buffer, buffer_size, "Aluno id=%d não está matriculado na matéria id=%d.", student_id, subject->id);
        return;
    }

    /* Verifica se há questionários no módulo */
    if (module->num_quizzes <= 0) {
        snprintf(data_buffer, buffer_size, "Módulo id=%d não possui questionários cadastrados.", module_id);
        return;
    }

    /* Percorre questionários e formata no buffer com cuidado para não estourar */
    int used = 0;
    for (int i = 0; i < module->num_quizzes; ++i) {
        int qid = module->quiz_ids[i];
        Quiz* quiz = NULL;
        for (int j = 0; j < global_data.num_quizzes; ++j) {
            if (global_data.quizzes[j].id == qid) {
                quiz = &global_data.quizzes[j];
                break;
            }
        }

        const char* qtitle = "(sem título)";
        if (quiz && strlen(quiz->title) > 0) qtitle = quiz->title;
        if (!quiz) qtitle = "(questionário não encontrado)";

        int remaining = buffer_size - used;
        if (remaining <= 1) break;

        int n = snprintf(data_buffer + used, remaining, "ID %d - %s\n", qid, qtitle);
        if (n < 0) break;

        if (n >= remaining) {
            const char ell[] = "...(truncated)";
            size_t ell_len = strlen(ell);
            if (buffer_size > (int)ell_len + 1) {
                int pos = buffer_size - (int)ell_len - 1;
                memcpy(data_buffer + pos, ell, ell_len);
                data_buffer[buffer_size - 1] = '\0';
            }
            return;
        }

        used += n;
    }
    /* buffer contém a lista (ou parte dela) já formatada */
}

void student_view_grades(int student_id, char* data_buffer, int buffer_size) {
    /* Inicializa buffer */
    if (buffer_size > 0 && data_buffer) data_buffer[0] = '\0';
    if (!data_buffer || buffer_size <= 0) return;

    /* Encontra aluno */
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Aluno com id %d não encontrado.", student_id);
        return;
    }

    int used = 0;
    char temp_entry_buffer[MAX_STR_LEN * 3]; // Buffer maior para detalhes da nota individual
    char header_buffer[MAX_STR_LEN * 2];     // Buffer para o cabeçalho

    // Primeiro, verifica se há alguma nota para o aluno
    int found_grades = 0;
    for (int i = 0; i < global_data.num_grades; ++i) {
        if (global_data.grades[i].student_id == student_id) {
            found_grades = 1;
            break;
        }
    }

    if (!found_grades) {
        snprintf(data_buffer, buffer_size, "Nenhuma nota registrada para o aluno id=%d.\n", student_id);
        return;
    }

    // Se encontrou notas, formata o cabeçalho primeiro no data_buffer
    int n_header = snprintf(header_buffer, sizeof(header_buffer), "\n--- NOTAS DO ALUNO %d (%s) ---\n", student->id, student->name);
    if (n_header < 0 || (size_t)n_header >= sizeof(header_buffer)) {
        snprintf(data_buffer, buffer_size, "Erro ao formatar cabeçalho de notas.");
        return;
    }
    // Copia o cabeçalho para o início do data_buffer
    strncpy(data_buffer, header_buffer, buffer_size);
    data_buffer[buffer_size - 1] = '\0'; // Garante terminação nula
    used += n_header;

    /* Percorre notas procurando as do aluno e as adiciona após o cabeçalho */
    for (int i = 0; i < global_data.num_grades; ++i) {
        Grade* g = &global_data.grades[i];
        if (g->student_id != student_id) continue;

        /* encontra nome da matéria */
        const char* subject_name = "(sem nome)";
        for (int s = 0; s < global_data.num_subjects; ++s) {
            if (global_data.subjects[s].id == g->subject_id) {
                if (strlen(global_data.subjects[s].name) > 0) subject_name = global_data.subjects[s].name;
                break;
            }
        }

        /* prepara campos da nota */
        double value = g->score;
        /* descrição */
        const char* desc = "(sem descrição)";
        if (strlen(g->description) > 0) desc = g->description;

        int remaining_for_entry = sizeof(temp_entry_buffer);
        int n_entry = snprintf(temp_entry_buffer, remaining_for_entry, "Matéria: %s (id=%d) | Nota: %.2f | %s\n",
                                 subject_name, g->subject_id, value, desc);
        
        if (n_entry < 0 || (size_t)n_entry >= remaining_for_entry) {
            continue; // Erro ou buffer temporário muito pequeno para esta entrada, pula para a próxima
        }

        int remaining_in_data_buffer = buffer_size - used;
        if (remaining_in_data_buffer <= 1) { // Deixa espaço para o terminador nulo
            const char ell[] = "... (lista truncada)\n";
            size_t ell_len = strlen(ell);
            if (buffer_size > used + ell_len + 1) {
                strncat(data_buffer + used, ell, remaining_in_data_buffer); // Usar strncat para segurança
                used += ell_len;
            }
            break; // Buffer principal cheio
        }
        
        // Tenta concatenar a entrada formatada no buffer principal
        // Usar snprintf para controlar o tamanho máximo a ser escrito
        int n_concat = snprintf(data_buffer + used, remaining_in_data_buffer, "%s", temp_entry_buffer);
        if (n_concat < 0 || n_concat >= remaining_in_data_buffer) {
            // Não foi possível adicionar a entrada completa, truncar ou parar
            const char ell[] = "... (lista truncada)\n";
            size_t ell_len = strlen(ell);
            if (buffer_size > used + ell_len + 1) {
                strncat(data_buffer + used, ell, remaining_in_data_buffer); // Usar strncat para segurança
                used += ell_len;
            }
            break; // Buffer principal cheio
        }
        used += n_concat;
    }
}

int student_get_quiz_questions(int student_id, int quiz_id, char* data_buffer, int buffer_size, char* response_message, int max_response_len) {
    if (buffer_size <= 0 || !data_buffer) {
        if (max_response_len > 0 && response_message) snprintf(response_message, max_response_len, "Buffer de dados inválido.");
        return -1;
    }
    data_buffer[0] = '\0'; // Clear data buffer

    if (max_response_len <= 0 || !response_message) return -1;
    response_message[0] = '\0'; // Clear response message buffer

    // 1. Find Student
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(response_message, max_response_len, "Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    // 2. Find Quiz
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; ++i) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;
        }
    }
    if (!quiz) {
        snprintf(response_message, max_response_len, "Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    // 3. Find Module containing the Quiz
    Module* module = NULL;
    for (int i = 0; i < global_data.num_modules; ++i) {
        Module* m = &global_data.modules[i];
        for (int j = 0; j < m->num_quizzes; ++j) {
            if (m->quiz_ids[j] == quiz_id) {
                module = m;
                break;
            }
        }
        if (module) break;
    }
    if (!module) {
        snprintf(response_message, max_response_len, "Módulo do questionário %d não encontrado.", quiz_id);
        return -1;
    }

    // 4. Find Subject containing the Module
    Subject* subject = NULL;
    for (int i = 0; i < global_data.num_subjects; ++i) {
        Subject* s = &global_data.subjects[i];
        for (int j = 0; j < s->num_modules; ++j) {
            if (s->module_ids[j] == module->id) {
                subject = s;
                break;
            }
        }
        if (subject) break;
    }
    if (!subject) {
        snprintf(response_message, max_response_len, "Matéria do módulo %d não encontrada.", module->id);
        return -1;
    }

    // 5. Verify if the student is enrolled in the subject
    int enrolled = 0;
    for (int i = 0; i < student->num_enrolled_subjects; ++i) {
        if (student->enrolled_subject_ids[i] == subject->id) {
            enrolled = 1;
            break;
        }
    }
    if (!enrolled) {
        snprintf(response_message, max_response_len, "Aluno %d não está matriculado na matéria %d do questionário %d.", student_id, subject->id, quiz_id);
        return -1;
    }

    // 6. Format questions into data_buffer
    if (quiz->num_questions <= 0) {
        snprintf(response_message, max_response_len, "Questionário %d não possui perguntas.", quiz_id);
        return -1;
    }

    int used_buffer_size = 0;
    int remaining_buffer_size = buffer_size;
    int n_written;

    // First line: number of questions
    n_written = snprintf(data_buffer + used_buffer_size, remaining_buffer_size, "%d\n", quiz->num_questions);
    if (n_written < 0 || n_written >= remaining_buffer_size) {
        snprintf(response_message, max_response_len, "Buffer insuficiente para perguntas do questionário.");
        return -1;
    }
    used_buffer_size += n_written;
    remaining_buffer_size -= n_written;

    for (int i = 0; i < quiz->num_questions; ++i) {
        Question* q = &quiz->questions[i];
        if (remaining_buffer_size <= 1) { // Leave space for null terminator
            snprintf(response_message, max_response_len, "Buffer insuficiente para todas as perguntas do questionário.");
            return -1;
        }

        // Novo formato: ID|LEN_TEXTO|TEXTO|LEN_OPCAO1|OPCAO1|LEN_OPCAO2|OPCAO2|LEN_OPCAO3|OPCAO3|LEN_OPCAO4|OPCAO4\n
        // Calculando comprimentos para evitar problemas com caracteres especiais e quebras de linha
        int len_text = strlen(q->text);
        int len_opt1 = strlen(q->options[0]);
        int len_opt2 = strlen(q->options[1]);
        int len_opt3 = strlen(q->options[2]);
        int len_opt4 = strlen(q->options[3]);

        n_written = snprintf(data_buffer + used_buffer_size, remaining_buffer_size,
                             "%d|%d|%s|%d|%s|%d|%s|%d|%s|%d|%s\n",
                             q->id, len_text, q->text,
                             len_opt1, q->options[0],
                             len_opt2, q->options[1],
                             len_opt3, q->options[2],
                             len_opt4, q->options[3]);
        
        if (n_written < 0 || n_written >= remaining_buffer_size) {
            snprintf(response_message, max_response_len, "Buffer insuficiente para formatar pergunta %d.", q->id);
            return -1;
        }
        used_buffer_size += n_written;
        remaining_buffer_size -= n_written;
    }

    snprintf(response_message, max_response_len, "Perguntas do questionário %d obtidas com sucesso.", quiz_id);
    return 0;
}

int student_submit_quiz_answers(int student_id, int quiz_id, const char* answers_str, char* response_message, int max_response_len) {
    if (max_response_len <= 0 || !response_message) return -1;
    response_message[0] = '\0'; // Clear response message buffer

    // 1. Find Student
    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(response_message, max_response_len, "Aluno com ID %d não encontrado.", student_id);
        return -1;
    }

    // 2. Find Quiz
    Quiz* quiz = NULL;
    for (int i = 0; i < global_data.num_quizzes; ++i) {
        if (global_data.quizzes[i].id == quiz_id) {
            quiz = &global_data.quizzes[i];
            break;
        }
    }
    if (!quiz) {
        snprintf(response_message, max_response_len, "Questionário com ID %d não encontrado.", quiz_id);
        return -1;
    }

    if (quiz->num_questions == 0) {
        snprintf(response_message, max_response_len, "Questionário %d não possui perguntas para serem respondidas.", quiz_id);
        return -1;
    }

    // Parse answers_str: "Q_ID:Answer_Option,Q_ID:Answer_Option,..."
    char temp_answers_str[MAX_DATA_LEN];
    strncpy(temp_answers_str, answers_str, MAX_DATA_LEN - 1);
    temp_answers_str[MAX_DATA_LEN - 1] = '\0';

    int correct_answers_count = 0;
    int total_questions_answered = 0;

    char* answer_pair_token = strtok(temp_answers_str, ",");
    while (answer_pair_token != NULL) {
        int q_id, student_answer;
        if (sscanf(answer_pair_token, "%d:%d", &q_id, &student_answer) == 2) {
            total_questions_answered++;
            // Find the question in the quiz
            Question* current_question = NULL;
            for (int i = 0; i < quiz->num_questions; ++i) {
                if (quiz->questions[i].id == q_id) {
                    current_question = &quiz->questions[i];
                    break;
                }
            }

            if (current_question) {
                if (student_answer == current_question->correct_option) {
                    correct_answers_count++;
                }
            }
        }
        answer_pair_token = strtok(NULL, ",");
    }

    if (total_questions_answered == 0) {
        snprintf(response_message, max_response_len, "Nenhuma resposta válida foi fornecida para o questionário %d.", quiz_id);
        return -1;
    }

    // Calculate score (simple percentage for now)
    float score = (float)correct_answers_count / quiz->num_questions * 100.0f;

    // Store Quiz Result (simplified - just adds to a global array for now)
    if (global_data.num_quiz_results < MAX_QUIZ_RESULTS) {
        QuizResult new_result;
        new_result.id = generate_next_id(); // Assumes a function to generate unique IDs
        new_result.student_id = student_id;
        new_result.quiz_id = quiz_id;
        new_result.score = score;
        time(&new_result.timestamp); // Set current timestamp

        global_data.quiz_results[global_data.num_quiz_results++] = new_result;
        if (save_data("system_data.dat") != 0) {
            snprintf(response_message, max_response_len, "Falha ao salvar o resultado do questionário.");
            return -1;
        }
    } else {
        snprintf(response_message, max_response_len, "Limite de resultados de questionário atingido. Resultado não salvo.");
        return -1;
    }

    snprintf(response_message, max_response_len, "Questionário \"%s\" (ID: %d) finalizado. Você acertou %d de %d perguntas. Sua nota: %.2f.",
             quiz->title, quiz_id, correct_answers_count, quiz->num_questions, score);
    return 0;
}

void student_view_absences(int student_id, char* data_buffer, int buffer_size) {
    if (buffer_size > 0 && data_buffer) data_buffer[0] = '\0';
    if (!data_buffer || buffer_size <= 0) return;

    Student* student = NULL;
    for (int i = 0; i < global_data.num_students; ++i) {
        if (global_data.students[i].id == student_id) {
            student = &global_data.students[i];
            break;
        }
    }
    if (!student) {
        snprintf(data_buffer, buffer_size, "Erro: Aluno com ID %d não encontrado.", student_id);
        return;
    }

    int used = 0;
    char temp_entry_buffer[MAX_STR_LEN * 3]; // Buffer maior para detalhes da falta
    char header_buffer[MAX_STR_LEN * 2];     // Buffer para o cabeçalho

    // Primeiro, verifica se há alguma falta para o aluno
    int found_absences = 0;
    for (int i = 0; i < global_data.num_absences; ++i) {
        if (global_data.absences[i].student_id == student_id) {
            found_absences = 1;
            break;
        }
    }

    if (!found_absences) {
        snprintf(data_buffer, buffer_size, "Nenhuma falta registrada para o aluno com ID %d.\n", student_id);
        return;
    }

    // Se encontrou faltas, formata o cabeçalho primeiro no data_buffer
    int n_header = snprintf(header_buffer, sizeof(header_buffer), "\n--- FALTAS DO ALUNO %d (%s) ---\n", student->id, student->name);
    if (n_header < 0 || (size_t)n_header >= sizeof(header_buffer)) {
        snprintf(data_buffer, buffer_size, "Erro ao formatar cabeçalho de faltas.");
        return;
    }
    // Copia o cabeçalho para o início do data_buffer
    strncpy(data_buffer, header_buffer, buffer_size);
    data_buffer[buffer_size - 1] = '\0'; // Garante terminação nula
    used += n_header;

    for (int i = 0; i < global_data.num_absences; ++i) {
        Absence* a = &global_data.absences[i];
        if (a->student_id != student_id) continue;

        const char* subject_name = "(sem nome)";
        Subject* subject = NULL;
        for (int s = 0; s < global_data.num_subjects; ++s) {
            if (global_data.subjects[s].id == a->subject_id) {
                subject = &global_data.subjects[s];
                if (strlen(subject->name) > 0) subject_name = subject->name;
                break;
            }
        }

        const char* justification = "Nenhuma";
        if (strlen(a->justification) > 0) {
            justification = a->justification;
        }

        int remaining_for_entry = sizeof(temp_entry_buffer);
        int n_entry = snprintf(temp_entry_buffer, remaining_for_entry, "Matéria: %s (ID: %d) | Data: %s | Justificativa: %s\n",
                                 subject_name, a->subject_id, a->date, justification);
        if (n_entry < 0 || (size_t)n_entry >= remaining_for_entry) {
            continue;
        }

        int remaining_in_data_buffer = buffer_size - used;
        if (remaining_in_data_buffer <= 1) {
            const char ell[] = "... (lista truncada)\n";
            size_t ell_len = strlen(ell);
            if (buffer_size > used + ell_len + 1) {
                strncat(data_buffer + used, ell, remaining_in_data_buffer); // Usar strncat para segurança
                used += ell_len;
            }
            break;
        }

        int n_concat = snprintf(data_buffer + used, remaining_in_data_buffer, "%s", temp_entry_buffer);
        if (n_concat < 0 || n_concat >= remaining_in_data_buffer) {
            const char ell[] = "... (lista truncada)\n";
            size_t ell_len = strlen(ell);
            if (buffer_size > used + ell_len + 1) {
                strncat(data_buffer + used, ell, remaining_in_data_buffer); // Usar strncat para segurança
                used += ell_len;
            }
            break;
        }
        used += n_concat;
    }
}
