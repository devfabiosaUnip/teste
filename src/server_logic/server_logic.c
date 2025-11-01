#include "server_logic/server_logic.h"
#include "models.h"
#include "protocol.h" // Inclui definições de protocolo (ClientMessage, ServerResponse, CommandType)
#include "admin/admin.h" // Inclui declarações das funções de administração
#include "storage/storage.h" // Inclui declarações de funções de armazenamento
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "professor/professor.h"
#include "student/student.h" // Incluir cabeçalho do aluno

// Função para autenticar usuários (agora no lado do servidor)
User* authenticate_user(const char* email, const char* password, UserRole* role) {
    static User logged_in_user; // Retorna uma cópia do usuário logado

    // Tenta autenticar como Admin
    for (int i = 0; i < global_data.num_admins; i++) {
        if (strcmp(global_data.admins[i].email, email) == 0 && strcmp(global_data.admins[i].password, password) == 0) {
            logged_in_user.id = global_data.admins[i].id;
            strcpy(logged_in_user.email, global_data.admins[i].email);
            strcpy(logged_in_user.password, global_data.admins[i].password);
            logged_in_user.role = ROLE_ADMIN;
            *role = ROLE_ADMIN;
            return &logged_in_user;
        }
    }

    // Tenta autenticar como Professor
    for (int i = 0; i < global_data.num_professors; i++) {
        if (strcmp(global_data.professors[i].email, email) == 0 && strcmp(global_data.professors[i].password, password) == 0) {
            logged_in_user.id = global_data.professors[i].id;
            strcpy(logged_in_user.email, global_data.professors[i].email);
            strcpy(logged_in_user.password, global_data.professors[i].password);
            logged_in_user.role = ROLE_PROFESSOR;
            *role = ROLE_PROFESSOR;
            return &logged_in_user;
        }
    }

    // Tenta autenticar como Aluno
    for (int i = 0; i < global_data.num_students; i++) {
        if (strcmp(global_data.students[i].email, email) == 0 && strcmp(global_data.students[i].password, password) == 0) {
            logged_in_user.id = global_data.students[i].id;
            strcpy(logged_in_user.email, global_data.students[i].email);
            strcpy(logged_in_user.password, global_data.students[i].password);
            logged_in_user.role = ROLE_STUDENT;
            *role = ROLE_STUDENT;
            return &logged_in_user;
        }
    }

    return NULL; // Autenticação falhou
}

// Nova função para lidar com o comando de login
ServerResponse handle_login_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro de autenticação desconhecido.");

    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        strncpy(email, token, MAX_EMAIL_LEN - 1);
        email[MAX_EMAIL_LEN - 1] = '\0';
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(password, token, MAX_PASSWORD_LEN - 1);
            password[MAX_PASSWORD_LEN - 1] = '\0';
        } else {
            strcpy(server_resp.message, "Formato de login inválido: senha ausente.");
            return server_resp;
        }
    } else {
        strcpy(server_resp.message, "Formato de login inválido: email ausente.");
        return server_resp;
    }

    UserRole role;
    User* user = authenticate_user(email, password, &role);

    if (user != NULL) {
        server_resp.status = 0;
        server_resp.user_id = user->id;
        strcpy(server_resp.message, "Login bem-sucedido.");
        switch (role) {
            case ROLE_ADMIN: strcpy(server_resp.data, "ROLE_ADMIN"); break;
            case ROLE_PROFESSOR: strcpy(server_resp.data, "ROLE_PROFESSOR"); break;
            case ROLE_STUDENT: strcpy(server_resp.data, "ROLE_STUDENT"); break;
            default: strcpy(server_resp.data, "UNKNOWN_ROLE"); break;
        }
    } else {
        strcpy(server_resp.message, "Email ou senha incorretos.");
    }
    return server_resp;
}

// Nova função para lidar com o comando de adicionar professor
ServerResponse handle_admin_add_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao adicionar professor.");

    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        strncpy(name, token, MAX_STR_LEN - 1);
        name[MAX_STR_LEN - 1] = '\0';
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(email, token, MAX_EMAIL_LEN - 1);
            email[MAX_EMAIL_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(password, token, MAX_PASSWORD_LEN - 1);
                password[MAX_PASSWORD_LEN - 1] = '\0';
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar professor: senha ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar professor: email ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar professor: nome ausente.");
        return server_resp;
    }

    int result = add_professor(name, email, password, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de editar professor
ServerResponse handle_admin_edit_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar professor.");

    int id;
    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(name, token, MAX_STR_LEN - 1);
            name[MAX_STR_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(email, token, MAX_EMAIL_LEN - 1);
                email[MAX_EMAIL_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(password, token, MAX_PASSWORD_LEN - 1);
                    password[MAX_PASSWORD_LEN - 1] = '\0';
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar professor: senha ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar professor: email ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar professor: nome ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar professor: ID ausente.");
        return server_resp;
    }

    int result = edit_professor(id, name, email, password, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de listar professores
ServerResponse handle_admin_list_professors_command() {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = 0; // Assume sucesso, a lista pode ser vazia
    strcpy(server_resp.message, "Lista de professores enviada.");

    list_professors(server_resp.data, MAX_DATA_LEN); // Preenche server_resp.data com a lista
    return server_resp;
}

// Nova função para lidar com o comando de remover professor
ServerResponse handle_admin_remove_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover professor.");

    int id;
    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para remover professor: ID ausente.");
        return server_resp;
    }
    int result = remove_professor(id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de adicionar turma
ServerResponse handle_admin_add_class_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao adicionar turma.");

    char name[MAX_STR_LEN];
    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        strncpy(name, token, MAX_STR_LEN - 1);
        name[MAX_STR_LEN - 1] = '\0';
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar turma: nome ausente.");
        return server_resp;
    }
    int result = add_class(name, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de listar turmas
ServerResponse handle_admin_list_classes_command() {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = 0; // Assume sucesso, a lista pode ser vazia
    strcpy(server_resp.message, "Lista de turmas enviada.");

    list_classes(server_resp.data, MAX_DATA_LEN);
    return server_resp;
}

// Nova função para lidar com o comando de editar turma
ServerResponse handle_admin_edit_class_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar turma.");

    int id;
    char name[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(name, token, MAX_STR_LEN - 1);
            name[MAX_STR_LEN - 1] = '\0';
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar turma: nome ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar turma: ID ausente.");
        return server_resp;
    }
    int result = edit_class(id, name, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de remover turma
ServerResponse handle_admin_remove_class_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover turma.");

    int id;
    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para remover turma: ID ausente.");
        return server_resp;
    }
    int result = remove_class(id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de adicionar aluno
ServerResponse handle_admin_add_student_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao adicionar aluno.");

    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        strncpy(name, token, MAX_STR_LEN - 1);
        name[MAX_STR_LEN - 1] = '\0';
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(email, token, MAX_EMAIL_LEN - 1);
            email[MAX_EMAIL_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(password, token, MAX_PASSWORD_LEN - 1);
                password[MAX_PASSWORD_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    class_id = atoi(token);
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar aluno: ID da turma ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar aluno: senha ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar aluno: email ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para adicionar aluno: nome ausente.");
        return server_resp;
    }

    int result = add_student(name, email, password, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de listar alunos
ServerResponse handle_admin_list_students_command() {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = 0; // Assume sucesso, a lista pode ser vazia
    strcpy(server_resp.message, "Lista de alunos enviada.");

    list_students(server_resp.data, MAX_DATA_LEN);
    return server_resp;
}

// Nova função para lidar com o comando de editar aluno
ServerResponse handle_admin_edit_student_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar aluno.");

    int id;
    char name[MAX_STR_LEN];
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(name, token, MAX_STR_LEN - 1);
            name[MAX_STR_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(email, token, MAX_EMAIL_LEN - 1);
                email[MAX_EMAIL_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(password, token, MAX_PASSWORD_LEN - 1);
                    password[MAX_PASSWORD_LEN - 1] = '\0';
                    token = strtok(NULL, ",");
                    if (token != NULL) {
                        class_id = atoi(token);
                    } else {
                        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar aluno: ID da turma ausente.");
                        return server_resp;
                    }
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar aluno: senha ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar aluno: email ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar aluno: nome ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para editar aluno: ID ausente.");
        return server_resp;
    }

    int result = edit_student(id, name, email, password, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de remover aluno
ServerResponse handle_admin_remove_student_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover aluno.");

    int id;
    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para remover aluno: ID ausente.");
        return server_resp;
    }
    int result = remove_student(id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de atribuir matéria ao professor (ADMIN)
ServerResponse handle_admin_assign_subject_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao atribuir matéria.");

    int professor_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para atribuir matéria: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para atribuir matéria: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_assign_subject_to_professor(professor_id, subject_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de desatribuir matéria do professor (ADMIN)
ServerResponse handle_admin_unassign_subject_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao desatribuir matéria.");

    int professor_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para desatribuir matéria: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para desatribuir matéria: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_unassign_subject_from_professor(professor_id, subject_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de atribuir turma ao professor (ADMIN)
ServerResponse handle_admin_assign_class_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao atribuir turma.");

    int professor_id;
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            class_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para atribuir turma: ID da turma ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para atribuir turma: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_assign_class_to_professor(professor_id, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de desatribuir turma do professor (ADMIN)
ServerResponse handle_admin_unassign_class_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao desatribuir turma.");

    int professor_id;
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            class_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para desatribuir turma: ID da turma ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para desatribuir turma: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_unassign_class_from_professor(professor_id, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de listar turmas atribuídas ao professor
ServerResponse handle_professor_list_assigned_classes_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = 0;
    strcpy(server_resp.message, "Lista de turmas atribuídas enviada.");

    int professor_id = client_msg.user_id;
    list_professor_assigned_classes(professor_id, server_resp.data, MAX_DATA_LEN);
    return server_resp;
}

// Nova função para lidar com o comando de listar matérias atribuídas ao professor
ServerResponse handle_professor_list_assigned_subjects_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = 0;
    strcpy(server_resp.message, "Lista de matérias atribuídas enviada.");

    int professor_id = client_msg.user_id;
    list_professor_assigned_subjects(professor_id, server_resp.data, MAX_DATA_LEN);
    return server_resp;
}

// Nova função para lidar com o comando de lançar nota
ServerResponse handle_professor_post_grade_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao lançar nota.");

    int professor_id = client_msg.user_id;
    int student_id;
    int subject_id;
    float grade_value;
    char description[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                grade_value = atof(token);
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(description, token, MAX_STR_LEN - 1);
                    description[MAX_STR_LEN - 1] = '\0';
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para lançar nota: descrição ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para lançar nota: valor da nota ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para lançar nota: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para lançar nota: ID do aluno ausente.");
        return server_resp;
    }
    int result = post_grade(professor_id, student_id, subject_id, grade_value, description, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de registrar falta
ServerResponse handle_professor_register_absence_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao registrar falta.");

    int professor_id = client_msg.user_id;
    int student_id;
    int subject_id;
    char date[MAX_STR_LEN];
    char justification[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(date, token, MAX_STR_LEN - 1);
                date[MAX_STR_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(justification, token, MAX_STR_LEN - 1);
                    justification[MAX_STR_LEN - 1] = '\0';
                } else {
                    justification[0] = '\0'; // Justificativa pode ser opcional
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para registrar falta: data ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para registrar falta: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para registrar falta: ID do aluno ausente.");
        return server_resp;
    }
    int result = register_absence(professor_id, student_id, subject_id, date, justification, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

// Nova função para lidar com o comando de visualizar notas/faltas
ServerResponse handle_professor_view_grades_absences_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao visualizar notas/faltas.");

    int professor_id = client_msg.user_id;
    int student_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para visualizar notas/faltas: ID do aluno ausente.");
        return server_resp;
    }
    view_student_grades_absences(professor_id, student_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Dados de notas e faltas do aluno enviados.");
    return server_resp;
}

// Função para processar comandos do cliente no servidor
ServerResponse process_client_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1; // Padrão de erro
    strcpy(server_resp.message, "Comando desconhecido ou não implementado.");

    switch (client_msg.command) {
        case CMD_LOGIN: {
            server_resp = handle_login_command(client_msg);
            break;
        }
        case CMD_ADMIN_ADD_PROFESSOR: {
            server_resp = handle_admin_add_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_EDIT_PROFESSOR: {
            server_resp = handle_admin_edit_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_LIST_PROFESSORS: {
            server_resp = handle_admin_list_professors_command();
            break;
        }
        case CMD_ADMIN_REMOVE_PROFESSOR: {
            server_resp = handle_admin_remove_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_ADD_CLASS: {
            server_resp = handle_admin_add_class_command(client_msg);
            break;
        }
        case CMD_ADMIN_LIST_CLASSES: {
            server_resp = handle_admin_list_classes_command();
            break;
        }
        case CMD_ADMIN_EDIT_CLASS: {
            server_resp = handle_admin_edit_class_command(client_msg);
            break;
        }
        case CMD_ADMIN_REMOVE_CLASS: {
            server_resp = handle_admin_remove_class_command(client_msg);
            break;
        }
        case CMD_ADMIN_ADD_STUDENT: {
            server_resp = handle_admin_add_student_command(client_msg);
            break;
        }
        case CMD_ADMIN_LIST_STUDENTS: {
            server_resp = handle_admin_list_students_command();
            break;
        }
        case CMD_ADMIN_EDIT_STUDENT: {
            server_resp = handle_admin_edit_student_command(client_msg);
            break;
        }
        case CMD_ADMIN_REMOVE_STUDENT: {
            server_resp = handle_admin_remove_student_command(client_msg);
            break;
        }
        case CMD_ADMIN_ASSIGN_SUBJECT: {
            server_resp = handle_admin_assign_subject_command(client_msg);
            break;
        }
        case CMD_ADMIN_UNASSIGN_SUBJECT: {
            server_resp = handle_admin_unassign_subject_command(client_msg);
            break;
        }
        case CMD_ADMIN_ASSIGN_CLASS: {
            server_resp = handle_admin_assign_class_command(client_msg);
            break;
        }
        case CMD_ADMIN_UNASSIGN_CLASS: {
            server_resp = handle_admin_unassign_class_command(client_msg);
            break;
        }
        case CMD_ADMIN_ASSIGN_SUBJECT_TO_PROFESSOR: {
            server_resp = handle_admin_assign_subject_to_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_UNASSIGN_SUBJECT_FROM_PROFESSOR: {
            server_resp = handle_admin_unassign_subject_from_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_ASSIGN_CLASS_TO_PROFESSOR: {
            server_resp = handle_admin_assign_class_to_professor_command(client_msg);
            break;
        }
        case CMD_ADMIN_UNASSIGN_CLASS_FROM_PROFESSOR: {
            server_resp = handle_admin_unassign_class_from_professor_command(client_msg);
            break;
        }
        // --- Comandos de Professor ---
        case CMD_PROFESSOR_LIST_ASSIGNED_CLASSES: {
            server_resp = handle_professor_list_assigned_classes_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_LIST_ASSIGNED_SUBJECTS: {
            server_resp = handle_professor_list_assigned_subjects_command(client_msg);
            break;
        }
        // --- Comandos de Professor - Notas e Faltas ---
        case CMD_PROFESSOR_POST_GRADE: {
            server_resp = handle_professor_post_grade_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_REGISTER_ABSENCE: {
            server_resp = handle_professor_register_absence_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_VIEW_GRADES_ABSENCES: {
            server_resp = handle_professor_view_grades_absences_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_CREATE_SUBJECT_MODULE: {
            server_resp = handle_professor_create_subject_module_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_LIST_SUBJECT_MODULES: {
            server_resp = handle_professor_list_subject_modules_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_EDIT_MODULE: {
            server_resp = handle_professor_edit_module_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_REMOVE_MODULE: {
            server_resp = handle_professor_remove_module_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_ADD_LESSON_TO_MODULE: {
            server_resp = handle_professor_add_lesson_to_module_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_EDIT_LESSON: {
            server_resp = handle_professor_edit_lesson_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_REMOVE_LESSON: {
            server_resp = handle_professor_remove_lesson_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_LIST_MODULE_LESSONS: {
            server_resp = handle_professor_list_module_lessons_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_CREATE_MODULE_QUIZ: {
            server_resp = handle_professor_create_module_quiz_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_EDIT_QUIZ: {
            server_resp = handle_professor_edit_quiz_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_REMOVE_QUIZ: {
            server_resp = handle_professor_remove_quiz_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_ADD_QUESTION_TO_QUIZ: {
            server_resp = handle_professor_add_question_to_quiz_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_EDIT_QUESTION: {
            server_resp = handle_professor_edit_question_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_REMOVE_QUESTION: {
            server_resp = handle_professor_remove_question_command(client_msg);
            break;
        }
        case CMD_PROFESSOR_LIST_MODULE_QUIZZES: {
            server_resp = handle_professor_list_module_quizzes_command(client_msg);
            break;
        }

        // --- Comandos de Aluno ---
        case CMD_STUDENT_ENROLL_SUBJECT: {
            server_resp.status = -1;
            strcpy(server_resp.message, "Funcionalidade de matrícula/desmatrícula de matéria movida para o Administrador.");
            break;
        }
        case CMD_STUDENT_UNENROLL_SUBJECT: {
            server_resp.status = -1;
            strcpy(server_resp.message, "Funcionalidade de matrícula/desmatrícula de matéria movida para o Administrador.");
            break;
        }
        case CMD_STUDENT_LIST_ENROLLED_SUBJECTS: {
            server_resp.status = -1;
            strcpy(server_resp.message, "Funcionalidade de listar matérias matriculadas movida para o Administrador.");
            break;
        }
        case CMD_STUDENT_LIST_MODULES_IN_SUBJECT: {
            server_resp = handle_student_list_modules_in_subject_command(client_msg);
            break;
        }
        case CMD_STUDENT_VIEW_LESSONS_IN_MODULE: {
            server_resp = handle_student_view_lessons_in_module_command(client_msg);
            break;
        }
        case CMD_STUDENT_VIEW_QUIZZES_IN_MODULE: {
            server_resp = handle_student_view_quizzes_in_module_command(client_msg);
            break;
        }
        case CMD_STUDENT_TAKE_QUIZ: {
            server_resp = handle_student_take_quiz_command(client_msg);
            break;
        }
        case CMD_STUDENT_REQUEST_QUIZ_QUESTIONS: {
            server_resp = handle_student_request_quiz_questions_command(client_msg);
            break;
        }
        case CMD_STUDENT_VIEW_GRADES: {
            server_resp = handle_student_view_grades_command(client_msg);
            break;
        }
        case CMD_STUDENT_VIEW_ABSENCES: {
            server_resp = handle_student_view_absences_command(client_msg);
            break;
        }

        default:
            // Mensagem padrão de erro já definida
            break;
    }

    return server_resp;
}

// Funções de tratamento de comandos de Aluno

// As funções handle_student_enroll_subject_command, handle_student_unenroll_subject_command e handle_student_list_enrolled_subjects_command
// foram movidas para o módulo de administração e, portanto, não são mais necessárias aqui.
// Os comandos correspondentes em process_client_command agora retornam uma mensagem informativa.

ServerResponse handle_student_list_modules_in_subject_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao listar módulos da matéria.");

    int student_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para listar módulos da matéria: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para listar módulos da matéria: ID do aluno ausente.");
        return server_resp;
    }

    student_list_modules_in_subject(student_id, subject_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Módulos da matéria listados com sucesso!");
    return server_resp;
}

ServerResponse handle_student_view_lessons_in_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao visualizar aulas do módulo.");

    int student_id;
    int module_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            module_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para visualizar aulas do módulo: ID do módulo ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para visualizar aulas do módulo: ID do aluno ausente.");
        return server_resp;
    }

    student_view_lessons_in_module(student_id, module_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Aulas do módulo listadas com sucesso!");
    return server_resp;
}

ServerResponse handle_student_view_quizzes_in_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao visualizar questionários do módulo.");

    int student_id;
    int module_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        student_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            module_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para visualizar questionários do módulo: ID do módulo ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para visualizar questionários do módulo: ID do aluno ausente.");
        return server_resp;
    }

    student_view_quizzes_in_module(student_id, module_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Questionários do módulo listados com sucesso!");
    return server_resp;
}

ServerResponse handle_student_take_quiz_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao fazer questionário.");

    int student_id;
    int quiz_id;
    // int answers[MAX_QUESTIONS_PER_QUIZ]; // As respostas serão tratadas via string
    // int num_answers = 0; // Não é mais relevante aqui

    char temp_data[MAX_DATA_LEN];
    strncpy(temp_data, client_msg.data, MAX_DATA_LEN - 1);
    temp_data[MAX_DATA_LEN - 1] = '\0';

    char* token = strtok(temp_data, ",");
    if (token != NULL) {
        quiz_id = atoi(token); // O primeiro token é o quiz_id
        
        token = strtok(NULL, ""); // O resto da string são as respostas no formato Q_ID:Answer_Option,...
        char* answers_str = token;

        if (answers_str == NULL || strlen(answers_str) == 0) {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para fazer questionário: respostas ausentes.");
            return server_resp;
        }

        student_id = client_msg.user_id; // student_id vem de client_msg.user_id
        
        // Chamar nova função para processar as respostas
        int result = student_submit_quiz_answers(student_id, quiz_id, answers_str, server_resp.message, MAX_MESSAGE_LEN);
        server_resp.status = (result == 0) ? 0 : -1;

    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido para fazer questionário: ID do questionário ausente.");
        return server_resp;
    }

    return server_resp;
}

ServerResponse handle_student_view_grades_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao visualizar notas.");

    int student_id = client_msg.user_id;

    student_view_grades(student_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Notas listadas com sucesso!");
    return server_resp;
}

ServerResponse handle_student_view_absences_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao visualizar faltas.");

    int student_id = client_msg.user_id;

    student_view_absences(student_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Faltas listadas com sucesso!");
    return server_resp;
}

ServerResponse handle_admin_assign_subject_to_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao atribuir matéria ao professor.");

    int professor_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_assign_subject_to_professor(professor_id, subject_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_admin_unassign_subject_from_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao desatribuir matéria do professor.");

    int professor_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            subject_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da matéria ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_unassign_subject_from_professor(professor_id, subject_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_admin_assign_class_to_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao atribuir turma ao professor.");

    int professor_id;
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            class_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da turma ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_assign_class_to_professor(professor_id, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_admin_unassign_class_from_professor_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao desatribuir turma do professor.");

    int professor_id;
    int class_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        professor_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            class_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da turma ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do professor ausente.");
        return server_resp;
    }

    int result = admin_unassign_class_from_professor(professor_id, class_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_create_subject_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao criar módulo em matéria.");

    int professor_id = client_msg.user_id;
    int subject_id;
    char module_name[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        subject_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(module_name, token, MAX_STR_LEN - 1);
            module_name[MAX_STR_LEN - 1] = '\0';
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nome do módulo ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da matéria ausente.");
        return server_resp;
    }

    int result = create_subject_module(professor_id, subject_id, module_name, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_list_subject_modules_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao listar módulos da matéria.");

    int professor_id = client_msg.user_id;
    int subject_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        subject_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da matéria ausente.");
        return server_resp;
    }

    // A função list_subject_modules preenche server_resp.data diretamente
    list_subject_modules(professor_id, subject_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0; // Assume sucesso, a lista pode ser vazia
    strcpy(server_resp.message, "Módulos da matéria enviados.");
    return server_resp;
}

ServerResponse handle_professor_edit_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar módulo.");

    int professor_id = client_msg.user_id;
    int module_id;
    char new_name[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(new_name, token, MAX_STR_LEN - 1);
            new_name[MAX_STR_LEN - 1] = '\0';
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: novo nome do módulo ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    int result = edit_module(professor_id, module_id, new_name, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_remove_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover módulo.");

    int professor_id = client_msg.user_id;
    int module_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    int result = remove_module(professor_id, module_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_add_lesson_to_module_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao adicionar aula ao módulo.");

    int professor_id = client_msg.user_id;
    int module_id;
    char lesson_title[MAX_STR_LEN];
    char lesson_content[MAX_DATA_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(lesson_title, token, MAX_STR_LEN - 1);
            lesson_title[MAX_STR_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(lesson_content, token, MAX_DATA_LEN - 1);
                lesson_content[MAX_DATA_LEN - 1] = '\0';
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: conteúdo da aula ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: título da aula ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    int result = add_lesson_to_module(professor_id, module_id, lesson_title, lesson_content, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_edit_lesson_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar aula.");

    int professor_id = client_msg.user_id;
    int lesson_id;
    char new_title[MAX_STR_LEN];
    char new_content[MAX_DATA_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        lesson_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(new_title, token, MAX_STR_LEN - 1);
            new_title[MAX_STR_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(new_content, token, MAX_DATA_LEN - 1);
                new_content[MAX_DATA_LEN - 1] = '\0';
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: novo conteúdo da aula ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: novo título da aula ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da aula ausente.");
        return server_resp;
    }

    int result = edit_lesson(professor_id, lesson_id, new_title, new_content, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_remove_lesson_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover aula.");

    int professor_id = client_msg.user_id;
    int lesson_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        lesson_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da aula ausente.");
        return server_resp;
    }

    int result = remove_lesson(professor_id, lesson_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_list_module_lessons_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao listar aulas do módulo.");

    int professor_id = client_msg.user_id;
    int module_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    list_module_lessons(professor_id, module_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Aulas do módulo enviadas.");
    return server_resp;
}

ServerResponse handle_professor_create_module_quiz_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao criar questionário em módulo.");

    int professor_id = client_msg.user_id;
    int module_id;
    char quiz_title[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(quiz_title, token, MAX_STR_LEN - 1);
            quiz_title[MAX_STR_LEN - 1] = '\0';
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: título do questionário ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    int result = create_module_quiz(professor_id, module_id, quiz_title, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_edit_quiz_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar questionário.");

    int professor_id = client_msg.user_id;
    int quiz_id;
    char new_title[MAX_STR_LEN];

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(new_title, token, MAX_STR_LEN - 1);
            new_title[MAX_STR_LEN - 1] = '\0';
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: novo título do questionário ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    int result = edit_quiz(professor_id, quiz_id, new_title, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_remove_quiz_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover questionário.");

    int professor_id = client_msg.user_id;
    int quiz_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    int result = remove_quiz(professor_id, quiz_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_add_question_to_quiz_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao adicionar pergunta ao questionário.");

    int professor_id = client_msg.user_id;
    int quiz_id;
    char question_text[MAX_STR_LEN];
    char option1[MAX_STR_LEN], option2[MAX_STR_LEN], option3[MAX_STR_LEN], option4[MAX_STR_LEN];
    int correct_option;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(question_text, token, MAX_STR_LEN - 1);
            question_text[MAX_STR_LEN - 1] = '\0';
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(option1, token, MAX_STR_LEN - 1);
                option1[MAX_STR_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(option2, token, MAX_STR_LEN - 1);
                    option2[MAX_STR_LEN - 1] = '\0';
                    token = strtok(NULL, ",");
                    if (token != NULL) {
                        strncpy(option3, token, MAX_STR_LEN - 1);
                        option3[MAX_STR_LEN - 1] = '\0';
                        token = strtok(NULL, ",");
                        if (token != NULL) {
                            strncpy(option4, token, MAX_STR_LEN - 1);
                            option4[MAX_STR_LEN - 1] = '\0';
                            token = strtok(NULL, ",");
                            if (token != NULL) {
                                correct_option = atoi(token);
                            } else {
                                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: opção correta ausente.");
                                return server_resp;
                            }
                        } else {
                            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: opção 4 ausente.");
                            return server_resp;
                        }
                    } else {
                        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: opção 3 ausente.");
                        return server_resp;
                    }
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: opção 2 ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: opção 1 ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: texto da pergunta ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    int result = add_question_to_quiz(professor_id, quiz_id, question_text, option1, option2, option3, option4, correct_option, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_edit_question_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao editar pergunta do questionário.");

    int professor_id = client_msg.user_id;
    int quiz_id;
    int question_id;
    char new_text[MAX_STR_LEN];
    char new_option1[MAX_STR_LEN], new_option2[MAX_STR_LEN], new_option3[MAX_STR_LEN], new_option4[MAX_STR_LEN];
    int new_correct_option;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            question_id = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                strncpy(new_text, token, MAX_STR_LEN - 1);
                new_text[MAX_STR_LEN - 1] = '\0';
                token = strtok(NULL, ",");
                if (token != NULL) {
                    strncpy(new_option1, token, MAX_STR_LEN - 1);
                    new_option1[MAX_STR_LEN - 1] = '\0';
                    token = strtok(NULL, ",");
                    if (token != NULL) {
                        strncpy(new_option2, token, MAX_STR_LEN - 1);
                        new_option2[MAX_STR_LEN - 1] = '\0';
                        token = strtok(NULL, ",");
                        if (token != NULL) {
                            strncpy(new_option3, token, MAX_STR_LEN - 1);
                            new_option3[MAX_STR_LEN - 1] = '\0';
                            token = strtok(NULL, ",");
                            if (token != NULL) {
                                strncpy(new_option4, token, MAX_STR_LEN - 1);
                                new_option4[MAX_STR_LEN - 1] = '\0';
                                token = strtok(NULL, ",");
                                if (token != NULL) {
                                    new_correct_option = atoi(token);
                                } else {
                                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nova opção correta ausente.");
                                    return server_resp;
                                }
                            } else {
                                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nova opção 4 ausente.");
                                return server_resp;
                            }
                        } else {
                            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nova opção 3 ausente.");
                            return server_resp;
                        }
                    } else {
                        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nova opção 2 ausente.");
                        return server_resp;
                    }
                } else {
                    snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: nova opção 1 ausente.");
                    return server_resp;
                }
            } else {
                snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: novo texto da pergunta ausente.");
                return server_resp;
            }
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da pergunta ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    int result = edit_question(professor_id, quiz_id, question_id, new_text, new_option1, new_option2, new_option3, new_option4, new_correct_option, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_remove_question_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao remover pergunta do questionário.");

    int professor_id = client_msg.user_id;
    int quiz_id;
    int question_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
        token = strtok(NULL, ",");
        if (token != NULL) {
            question_id = atoi(token);
        } else {
            snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID da pergunta ausente.");
            return server_resp;
        }
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    int result = remove_question(professor_id, quiz_id, question_id, server_resp.message, MAX_MESSAGE_LEN);
    server_resp.status = (result == 0) ? 0 : -1;
    return server_resp;
}

ServerResponse handle_professor_list_module_quizzes_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao listar questionários do módulo.");

    int professor_id = client_msg.user_id;
    int module_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        module_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do módulo ausente.");
        return server_resp;
    }

    list_module_quizzes(professor_id, module_id, server_resp.data, MAX_DATA_LEN);
    server_resp.status = 0;
    strcpy(server_resp.message, "Questionários do módulo enviados.");
    return server_resp;
}

ServerResponse handle_student_request_quiz_questions_command(ClientMessage client_msg) {
    ServerResponse server_resp;
    memset(&server_resp, 0, sizeof(ServerResponse));
    server_resp.status = -1;
    strcpy(server_resp.message, "Erro ao solicitar perguntas do questionário.");

    int student_id = client_msg.user_id;
    int quiz_id;

    char* token = strtok(client_msg.data, ",");
    if (token != NULL) {
        quiz_id = atoi(token);
    } else {
        snprintf(server_resp.message, MAX_MESSAGE_LEN, "Formato de dados inválido: ID do questionário ausente.");
        return server_resp;
    }

    // Aqui, vamos chamar uma nova função em student.c para obter as perguntas
    // e formatá-las no server_resp.data
    char questions_data[MAX_DATA_LEN];
    int result = student_get_quiz_questions(student_id, quiz_id, questions_data, MAX_DATA_LEN, server_resp.message, MAX_MESSAGE_LEN);

    if (result == 0) {
        server_resp.status = 0;
        strcpy(server_resp.message, "Perguntas do questionário enviadas.");
        strncpy(server_resp.data, questions_data, MAX_DATA_LEN - 1);
        server_resp.data[MAX_DATA_LEN - 1] = '\0';
    } else {
        // server_resp.message já foi preenchido por student_get_quiz_questions em caso de erro
    }

    return server_resp;
}