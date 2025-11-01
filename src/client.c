#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "client.h"
#include "protocol.h" // Necessário para ServerResponse e CommandType
#include "common_utils.h" // Para funções de utilidade de entrada de usuário e menus
#include "models.h" // Necessário para UserRole

// Protótipos das funções de manipulação do professor
void handle_professor_actions(int professor_id, int client_socket);
void handle_professor_grades_absences_menu(int professor_id, int client_socket);
void handle_professor_modules_lessons_menu(int professor_id, int client_socket);
void handle_professor_quizzes_menu(int professor_id, int client_socket);

// Função para enviar uma mensagem ao servidor e receber a resposta
ServerResponse send_command_to_server(int client_socket, CommandType command, int user_id, const char* data) {
    ClientMessage client_msg;
    ServerResponse server_resp;

    client_msg.command = command;
    client_msg.user_id = user_id;
    strncpy(client_msg.data, data, sizeof(client_msg.data) - 1);
    client_msg.data[sizeof(client_msg.data) - 1] = '\0';

    send(client_socket, (char*)&client_msg, sizeof(ClientMessage), 0);
    recv(client_socket, (char*)&server_resp, sizeof(ServerResponse), 0);

    return server_resp;
}

void start_client(const char* server_ip, int port) {
#ifdef _WIN32
    WSADATA wsa;
    printf("Inicializando Winsock para cliente...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Erro. Código de erro: %d\n", WSAGetLastError());
        return;
    }
    printf("Winsock inicializado para cliente.\n");
#endif

    int client_socket;
    struct sockaddr_in server_addr;

    // Cria o socket do cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erro ao criar socket do cliente");
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Endereço IP inválido/não suportado");
#ifdef _WIN32
        closesocket(client_socket);
        WSACleanup();
#else
        close(client_socket);
#endif
        return;
    }

    // Conecta ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao conectar ao servidor");
#ifdef _WIN32
        closesocket(client_socket);
        WSACleanup();
#else
        close(client_socket);
#endif
        return;
    }

    printf("Conectado ao servidor %s:%d\n", server_ip, port);

    // Lógica do cliente (menu de login, admin, professor, aluno)
    int choice;
    char email[MAX_EMAIL_LEN];
    char password[MAX_PASSWORD_LEN];
    int logged_in_user_id = 0;
    UserRole current_user_role = (UserRole)CMD_UNKNOWN; // Usar CMD_UNKNOWN como um papel inicial inválido

    do {
        display_main_menu(); // Menu principal para login/saída
        choice = get_int_input("Escolha uma opção: ");

        switch (choice) {
            case 1: { // Login
                get_string_input("Email: ", email, MAX_EMAIL_LEN);
                get_string_input("Senha: ", password, MAX_PASSWORD_LEN);
                char login_data[MAX_EMAIL_LEN + MAX_PASSWORD_LEN + 2]; // email,senha
                sprintf(login_data, "%s,%s", email, password);

                ServerResponse resp = send_command_to_server(client_socket, CMD_LOGIN, 0, login_data);

                if (resp.status == 0) {
                    printf("Login bem-sucedido! Bem-vindo(a)!\n");
                    logged_in_user_id = resp.user_id; // logged_in_user_id agora é utilizado
                    // Converter resp.data (string) para UserRole
                    if (strcmp(resp.data, "ROLE_ADMIN") == 0) {
                        current_user_role = ROLE_ADMIN;
                    } else if (strcmp(resp.data, "ROLE_PROFESSOR") == 0) {
                        current_user_role = ROLE_PROFESSOR;
                    } else if (strcmp(resp.data, "ROLE_STUDENT") == 0) {
                        current_user_role = ROLE_STUDENT;
                    }

                    // Entrar no menu de acordo com o papel
                    switch (current_user_role) {
                        case ROLE_ADMIN: {
                            int admin_choice;
                            do {
                                display_admin_main_menu();
                                admin_choice = get_int_input("Escolha uma opção: ");

                                switch (admin_choice) {
                                    case 1: { // Gerenciar Professores
                                        int prof_choice;
                                        do {
                                            display_admin_professor_menu();
                                            prof_choice = get_int_input("Escolha uma opção: ");
                                            switch (prof_choice) {
                                                case 1: { // Adicionar Professor
                                                    char name[MAX_STR_LEN];
                                                    char email[MAX_EMAIL_LEN];
                                                    char password[MAX_PASSWORD_LEN];
                                                    get_string_input("Nome do Professor: ", name, MAX_STR_LEN);
                                                    get_string_input("Email do Professor: ", email, MAX_EMAIL_LEN);
                                                    get_string_input("Senha do Professor: ", password, MAX_PASSWORD_LEN);
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%s,%s,%s", name, email, password);
                                                    ServerResponse add_prof_resp = send_command_to_server(client_socket, CMD_ADMIN_ADD_PROFESSOR, logged_in_user_id, data_to_send);
                                                    if (add_prof_resp.status == 0) {
                                                        printf("Sucesso: %s\n", add_prof_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", add_prof_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 2: { // Listar Professores
                                                    ServerResponse list_prof_resp = send_command_to_server(client_socket, CMD_ADMIN_LIST_PROFESSORS, logged_in_user_id, "");
                                                    if (list_prof_resp.status == 0) {
                                                        printf("\n--- LISTA DE PROFESSORES ---\n%s-----------------------------\n", list_prof_resp.data);
                                                    } else {
                                                        printf("Erro: %s\n", list_prof_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 3: { // Editar Professor
                                                    int id = get_int_input("ID do Professor a ser editado: ");
                                                    char name[MAX_STR_LEN];
                                                    char email[MAX_EMAIL_LEN];
                                                    char password[MAX_PASSWORD_LEN];
                                                    get_string_input("Novo Nome: ", name, MAX_STR_LEN);
                                                    get_string_input("Novo Email: ", email, MAX_EMAIL_LEN);
                                                    get_string_input("Nova Senha: ", password, MAX_PASSWORD_LEN);
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d,%s,%s,%s", id, name, email, password);
                                                    ServerResponse edit_prof_resp = send_command_to_server(client_socket, CMD_ADMIN_EDIT_PROFESSOR, logged_in_user_id, data_to_send);
                                                    if (edit_prof_resp.status == 0) {
                                                        printf("Sucesso: %s\n", edit_prof_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", edit_prof_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 4: { // Remover Professor
                                                    int id = get_int_input("ID do Professor a ser removido: ");
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d", id);
                                                    ServerResponse remove_prof_resp = send_command_to_server(client_socket, CMD_ADMIN_REMOVE_PROFESSOR, logged_in_user_id, data_to_send);
                                                    if (remove_prof_resp.status == 0) {
                                                        printf("Sucesso: %s\n", remove_prof_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", remove_prof_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 0: break;
                                                default: printf("Opção inválida. Tente novamente.\n"); break;
                                            }
                                        } while (prof_choice != 0);
                                        break;
                                    }
                                    case 2: { // Gerenciar Turmas
                                        int class_choice;
                                        do {
                                            display_admin_class_menu();
                                            class_choice = get_int_input("Escolha uma opção: ");
                                            switch (class_choice) {
                                                case 1: { // Adicionar Turma
                                                    char name[MAX_STR_LEN];
                                                    get_string_input("Nome da Turma: ", name, MAX_STR_LEN);
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%s", name);
                                                    ServerResponse add_class_resp = send_command_to_server(client_socket, CMD_ADMIN_ADD_CLASS, logged_in_user_id, data_to_send);
                                                    if (add_class_resp.status == 0) {
                                                        printf("Sucesso: %s\n", add_class_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", add_class_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 2: { // Listar Turmas
                                                    ServerResponse list_class_resp = send_command_to_server(client_socket, CMD_ADMIN_LIST_CLASSES, logged_in_user_id, "");
                                                    if (list_class_resp.status == 0) {
                                                        printf("\n--- LISTA DE TURMAS ---\n%s-------------------------\n", list_class_resp.data);
                                                    } else {
                                                        printf("Erro: %s\n", list_class_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 3: { // Editar Turma
                                                    int id = get_int_input("ID da Turma a ser editada: ");
                                                    char name[MAX_STR_LEN];
                                                    get_string_input("Novo Nome: ", name, MAX_STR_LEN);
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d,%s", id, name);
                                                    ServerResponse edit_class_resp = send_command_to_server(client_socket, CMD_ADMIN_EDIT_CLASS, logged_in_user_id, data_to_send);
                                                    if (edit_class_resp.status == 0) {
                                                        printf("Sucesso: %s\n", edit_class_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", edit_class_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 4: { // Remover Turma
                                                    int id = get_int_input("ID da Turma a ser removida: ");
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d", id);
                                                    ServerResponse remove_class_resp = send_command_to_server(client_socket, CMD_ADMIN_REMOVE_CLASS, logged_in_user_id, data_to_send);
                                                    if (remove_class_resp.status == 0) {
                                                        printf("Sucesso: %s\n", remove_class_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", remove_class_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 0: break;
                                                default: printf("Opção inválida. Tente novamente.\n"); break;
                                            }
                                        } while (class_choice != 0);
                                        break;
                                    }
                                    case 3: { // Gerenciar Alunos
                                        int student_choice;
                                        do {
                                            display_admin_student_menu();
                                            student_choice = get_int_input("Escolha uma opção: ");
                                            switch (student_choice) {
                                                case 1: { // Adicionar Aluno
                                                    char name[MAX_STR_LEN];
                                                    char email[MAX_EMAIL_LEN];
                                                    char password[MAX_PASSWORD_LEN];
                                                    int class_id;
                                                    get_string_input("Nome do Aluno: ", name, MAX_STR_LEN);
                                                    get_string_input("Email do Aluno: ", email, MAX_EMAIL_LEN);
                                                    get_string_input("Senha do Aluno: ", password, MAX_PASSWORD_LEN);
                                                    class_id = get_int_input("ID da Turma: ");
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%s,%s,%s,%d", name, email, password, class_id);
                                                    ServerResponse add_student_resp = send_command_to_server(client_socket, CMD_ADMIN_ADD_STUDENT, logged_in_user_id, data_to_send);
                                                    if (add_student_resp.status == 0) {
                                                        printf("Sucesso: %s\n", add_student_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", add_student_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 2: { // Listar Alunos
                                                    ServerResponse list_student_resp = send_command_to_server(client_socket, CMD_ADMIN_LIST_STUDENTS, logged_in_user_id, "");
                                                    if (list_student_resp.status == 0) {
                                                        printf("\n--- LISTA DE ALUNOS ---\n%s---------------------------\n", list_student_resp.data);
                                                    } else {
                                                        printf("Erro: %s\n", list_student_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 3: { // Editar Aluno
                                                    int id = get_int_input("ID do Aluno a ser editado: ");
                                                    char name[MAX_STR_LEN];
                                                    char email[MAX_EMAIL_LEN];
                                                    char password[MAX_PASSWORD_LEN];
                                                    int new_class_id;
                                                    get_string_input("Novo Nome: ", name, MAX_STR_LEN);
                                                    get_string_input("Novo Email: ", email, MAX_EMAIL_LEN);
                                                    get_string_input("Nova Senha: ", password, MAX_PASSWORD_LEN);
                                                    new_class_id = get_int_input("Novo ID da Turma (0 para manter): ");
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d,%s,%s,%s,%d", id, name, email, password, new_class_id);
                                                    ServerResponse edit_student_resp = send_command_to_server(client_socket, CMD_ADMIN_EDIT_STUDENT, logged_in_user_id, data_to_send);
                                                    if (edit_student_resp.status == 0) {
                                                        printf("Sucesso: %s\n", edit_student_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", edit_student_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 4: { // Remover Aluno
                                                    int id = get_int_input("ID do Aluno a ser removido: ");
                                                    char data_to_send[MAX_DATA_LEN];
                                                    snprintf(data_to_send, MAX_DATA_LEN, "%d", id);
                                                    ServerResponse remove_student_resp = send_command_to_server(client_socket, CMD_ADMIN_REMOVE_STUDENT, logged_in_user_id, data_to_send);
                                                    if (remove_student_resp.status == 0) {
                                                        printf("Sucesso: %s\n", remove_student_resp.message);
                                                    } else {
                                                        printf("Erro: %s\n", remove_student_resp.message);
                                                    }
                                                    break;
                                                }
                                                case 0: break;
                                                default: printf("Opção inválida. Tente novamente.\n"); break;
                                            }
                                        } while (student_choice != 0);
                                        break;
                                    }
                                    case 4: { // Gerenciar Atribuições Professor-Matéria/Turma
                                        handle_admin_professor_subject_class_assignment_menu(logged_in_user_id, client_socket);
                                        break;
                                    }
                                    case 0: break;
                                    default: printf("Opção inválida. Tente novamente.\n"); break;
                                }
                            } while (admin_choice != 0);
                            break;
                        }
                        case ROLE_PROFESSOR:
                            printf("Login bem-sucedido! Bem-vindo(a)! Logado como Professor.\n");
                            handle_professor_actions(logged_in_user_id, client_socket);
                            break;
                        case ROLE_STUDENT:
                            printf("Login bem-sucedido! Bem-vindo(a)! Logado como Aluno.\n");
                            handle_student_actions(logged_in_user_id, client_socket);
                            break;
                        default:
                            printf("Erro: Papel de usuário desconhecido.\n");
                            break;
                    }

                } else {
                    printf("Erro de login: %s\n", resp.message);
                }
                break;
            }
            case 0: // Sair
                printf("Desconectando do servidor. Até mais!\n");
                // Enviar comando de logout ao servidor (opcional)
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (choice != 0);

#ifdef _WIN32
    closesocket(client_socket);
    WSACleanup();
#else
    close(client_socket);
#endif
}

// main para o cliente (temporário, pode ser movido para um arquivo cliente_main.c dedicado)
/*
int main() {
    start_client("127.0.0.1", 8080);
    return 0;
}
*/

void handle_professor_actions(int professor_id, int client_socket) {
    int choice;
    int running = 1;

    while (running) {
        display_professor_main_menu();
        choice = get_int_input("");
        clear_input_buffer();

        switch (choice) {
            case 1: // Gerenciar Matérias e Turmas Atribuídas
                printf("\nFuncionalidade movida para o menu do Administrador. Por favor, entre em contato com o administrador para gerenciar atribuições.\n");
                break;
            case 2: // Gerenciar Notas e Faltas de Alunos
                handle_professor_grades_absences_menu(professor_id, client_socket);
                break;
            case 3: // Gerenciar Módulos e Aulas
                handle_professor_modules_lessons_menu(professor_id, client_socket);
                break;
            case 4: // Gerenciar Questionários
                handle_professor_quizzes_menu(professor_id, client_socket);
                break;
            case 0: // Logout
                running = 0;
                printf("Voltando ao Menu Principal.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

void handle_professor_grades_absences_menu(int professor_id, int client_socket) {
    int choice;
    int running = 1;
    char input_buffer[MAX_DATA_LEN];
    ServerResponse server_resp;

    while (running) {
        display_professor_grades_absences_menu();
        choice = get_int_input("");
        clear_input_buffer();

        switch (choice) {
            case 1: { // Lançar Nota
                int student_id, subject_id;
                float grade_value;
                char description[MAX_STR_LEN];
                printf("ID do Aluno: ");
                student_id = get_int_input("");
                printf("ID da Matéria: ");
                subject_id = get_int_input("");
                printf("Valor da Nota: ");
                scanf("%f", &grade_value);
                clear_input_buffer();
                printf("Descrição (ex: 'Prova 1'): ");
                get_string_input("Descrição (ex: 'Prova 1'): ", description, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d,%f,%s", student_id, subject_id, grade_value, description);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_POST_GRADE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 2: { // Registrar Falta
                int student_id, subject_id;
                char date[MAX_STR_LEN];
                char justification[MAX_STR_LEN];
                printf("ID do Aluno: ");
                student_id = get_int_input("");
                printf("ID da Matéria: ");
                subject_id = get_int_input("");
                printf("Data (DD/MM/AAAA): ");
                get_string_input("Data (DD/MM/AAAA): ", date, MAX_STR_LEN);
                printf("Justificativa (opcional): ");
                get_string_input("Justificativa (opcional): ", justification, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d,%s,%s", student_id, subject_id, date, justification);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_REGISTER_ABSENCE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 3: { // Visualizar Notas e Faltas de Aluno
                int student_id;
                printf("ID do Aluno para visualizar: ");
                student_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", student_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_VIEW_GRADES_ABSENCES, professor_id, input_buffer);
                printf("\n--- NOTAS E FALTAS DO ALUNO ---\n");
                printf("%s\n", server_resp.data);
                printf("----------------------------------\n");
                break;
            }
            case 0: // Voltar
                running = 0;
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

void handle_professor_modules_lessons_menu(int professor_id, int client_socket) {
    int choice;
    int running = 1;
    char input_buffer[MAX_DATA_LEN];
    ServerResponse server_resp;

    while (running) {
        display_professor_modules_lessons_menu();
        choice = get_int_input("");
        clear_input_buffer();

        switch (choice) {
            case 1: { // Criar Módulo em Matéria
                int subject_id;
                char module_name[MAX_STR_LEN];
                printf("ID da Matéria para criar módulo: ");
                subject_id = get_int_input("");
                printf("Nome do Módulo: ");
                get_string_input("Nome do Módulo: ", module_name, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s", subject_id, module_name);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_CREATE_SUBJECT_MODULE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 2: { // Listar Módulos de Matéria
                int subject_id;
                printf("ID da Matéria para listar módulos: ");
                subject_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", subject_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_LIST_SUBJECT_MODULES, professor_id, input_buffer);
                printf("\n--- MÓDULOS DA MATÉRIA ---\n");
                printf("%s\n", server_resp.data);
                printf("--------------------------\n");
                break;
            }
            case 3: { // Editar Módulo
                int module_id;
                char new_name[MAX_STR_LEN];
                printf("ID do Módulo a editar: ");
                module_id = get_int_input("");
                printf("Novo Nome do Módulo: ");
                get_string_input("Novo Nome do Módulo: ", new_name, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s", module_id, new_name);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_EDIT_MODULE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 4: { // Remover Módulo
                int module_id;
                printf("ID do Módulo a remover: ");
                module_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", module_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_REMOVE_MODULE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 5: { // Adicionar Aula a Módulo
                int module_id;
                char lesson_title[MAX_STR_LEN];
                char youtube_link[MAX_STR_LEN];
                printf("ID do Módulo para adicionar aula: ");
                module_id = get_int_input("");
                printf("Título da Aula: ");
                get_string_input("Título da Aula: ", lesson_title, MAX_STR_LEN);
                printf("Link do YouTube: ");
                get_string_input("Link do YouTube: ", youtube_link, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s,%s", module_id, lesson_title, youtube_link);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_ADD_LESSON_TO_MODULE, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 6: { // Editar Aula
                int lesson_id;
                char new_title[MAX_STR_LEN];
                char new_youtube_link[MAX_STR_LEN];
                printf("ID da Aula a editar: ");
                lesson_id = get_int_input("");
                printf("Novo Título da Aula: ");
                get_string_input("Novo Título da Aula: ", new_title, MAX_STR_LEN);
                printf("Novo Link do YouTube: ");
                get_string_input("Novo Link do YouTube: ", new_youtube_link, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s,%s", lesson_id, new_title, new_youtube_link);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_EDIT_LESSON, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 7: { // Remover Aula
                int lesson_id;
                printf("ID da Aula a remover: ");
                lesson_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", lesson_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_REMOVE_LESSON, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 8: { // Listar Aulas de Módulo
                int module_id;
                printf("ID do Módulo para listar aulas: ");
                module_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", module_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_LIST_MODULE_LESSONS, professor_id, input_buffer);
                printf("\n--- AULAS DO MÓDULO ---\n");
                printf("%s\n", server_resp.data);
                printf("-------------------------\n");
                break;
            }
            case 0: // Voltar
                running = 0;
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

void handle_professor_quizzes_menu(int professor_id, int client_socket) {
    int choice;
    int running = 1;
    char input_buffer[MAX_DATA_LEN];
    ServerResponse server_resp;

    while (running) {
        display_professor_quizzes_menu();
        choice = get_int_input("");
        clear_input_buffer();

        switch (choice) {
            case 1: { // Criar Questionário em Módulo
                int module_id;
                char quiz_title[MAX_STR_LEN];
                printf("ID do Módulo para criar questionário: ");
                module_id = get_int_input("");
                printf("Título do Questionário: ");
                get_string_input("Título do Questionário: ", quiz_title, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s", module_id, quiz_title);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_CREATE_MODULE_QUIZ, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 2: { // Editar Questionário
                int quiz_id;
                char new_title[MAX_STR_LEN];
                printf("ID do Questionário a editar: ");
                quiz_id = get_int_input("");
                printf("Novo Título do Questionário: ");
                get_string_input("Novo Título do Questionário: ", new_title, MAX_STR_LEN);
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s", quiz_id, new_title);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_EDIT_QUIZ, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 3: { // Remover Questionário
                int quiz_id;
                printf("ID do Questionário a remover: ");
                quiz_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", quiz_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_REMOVE_QUIZ, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 4: { // Adicionar Pergunta a Questionário
                int quiz_id, correct_option;
                char question_text[MAX_STR_LEN];
                char option1[MAX_STR_LEN], option2[MAX_STR_LEN], option3[MAX_STR_LEN], option4[MAX_STR_LEN];
                printf("ID do Questionário para adicionar pergunta: ");
                quiz_id = get_int_input("");
                printf("Texto da Pergunta: ");
                get_string_input("Texto da Pergunta: ", question_text, MAX_STR_LEN);
                printf("Opção 1: ");
                get_string_input("Opção 1: ", option1, MAX_STR_LEN);
                printf("Opção 2: ");
                get_string_input("Opção 2: ", option2, MAX_STR_LEN);
                printf("Opção 3: ");
                get_string_input("Opção 3: ", option3, MAX_STR_LEN);
                printf("Opção 4: ");
                get_string_input("Opção 4: ", option4, MAX_STR_LEN);
                printf("Opção Correta (1-4): ");
                correct_option = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%s,%s,%s,%s,%s,%d", 
                         quiz_id, question_text, option1, option2, option3, option4, correct_option);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_ADD_QUESTION_TO_QUIZ, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 5: { // Editar Pergunta de Questionário
                int quiz_id, question_id, new_correct_option;
                char new_text[MAX_STR_LEN];
                char new_option1[MAX_STR_LEN], new_option2[MAX_STR_LEN], new_option3[MAX_STR_LEN], new_option4[MAX_STR_LEN];
                printf("ID do Questionário da pergunta: ");
                quiz_id = get_int_input("");
                printf("ID da Pergunta a editar: ");
                question_id = get_int_input("");
                printf("Novo Texto da Pergunta: ");
                get_string_input("Novo Texto da Pergunta: ", new_text, MAX_STR_LEN);
                printf("Nova Opção 1: ");
                get_string_input("Nova Opção 1: ", new_option1, MAX_STR_LEN);
                printf("Nova Opção 2: ");
                get_string_input("Nova Opção 2: ", new_option2, MAX_STR_LEN);
                printf("Nova Opção 3: ");
                get_string_input("Nova Opção 3: ", new_option3, MAX_STR_LEN);
                printf("Nova Opção 4: ");
                get_string_input("Nova Opção 4: ", new_option4, MAX_STR_LEN);
                printf("Nova Opção Correta (1-4): ");
                new_correct_option = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d,%s,%s,%s,%s,%s,%d", 
                         quiz_id, question_id, new_text, new_option1, new_option2, new_option3, new_option4, new_correct_option);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_EDIT_QUESTION, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 6: { // Remover Pergunta de Questionário
                int quiz_id, question_id;
                printf("ID do Questionário da pergunta: ");
                quiz_id = get_int_input("");
                printf("ID da Pergunta a remover: ");
                question_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", quiz_id, question_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_REMOVE_QUESTION, professor_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 7: { // Listar Questionários de Módulo
                int module_id;
                printf("ID do Módulo para listar questionários: ");
                module_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d", module_id);
                server_resp = send_command_to_server(client_socket, CMD_PROFESSOR_LIST_MODULE_QUIZZES, professor_id, input_buffer);
                printf("\n--- QUESTIONÁRIOS DO MÓDULO ---\n");
                printf("%s\n", server_resp.data);
                printf("----------------------------------\n");
                break;
            }
            case 0: // Voltar
                running = 0;
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

// Funções de menu para o aluno
void display_student_main_menu() {
    printf("\n--- MENU DO ALUNO ---\n");
    printf("1. Gerenciar Matérias (Matricular/Desmatricular/Listar)\n");
    printf("2. Visualizar Conteúdo (Módulos, Aulas, Questionários)\n");
    printf("3. Fazer Questionário\n");
    printf("4. Visualizar Notas e Faltas\n");
    printf("0. Logout\n");
    printf("---------------------\n");
}

void display_student_subject_menu() {
    printf("\n--- GERENCIAR MATÉRIAS ---\n");
    printf("1. Matricular-se em Matéria\n");
    printf("2. Desmatricular-se de Matéria\n");
    printf("3. Listar Matérias Matriculadas\n");
    printf("0. Voltar\n");
    printf("--------------------------\n");
}

void display_student_module_menu() {
    printf("\n--- VISUALIZAR CONTEÚDO ---\n");
    printf("1. Listar Módulos de uma Matéria\n");
    printf("2. Visualizar Aulas de um Módulo\n");
    printf("3. Visualizar Questionários de um Módulo\n");
    printf("0. Voltar\n");
    printf("---------------------------\n");
}

void display_student_quiz_menu() {
    printf("\n--- FAZER QUESTIONÁRIO ---\n");
    printf("1. Listar Questionários de um Módulo\n");
    printf("2. Iniciar Questionário\n");
    printf("0. Voltar\n");
    printf("---------------------------\n");
}

void handle_student_actions(int student_id, int client_socket) {
    int choice;
    int running = 1;
    char input_buffer[MAX_DATA_LEN];
    ServerResponse server_resp;

    while (running) {
        display_student_main_menu();
        choice = get_int_input("Escolha uma opção: ");
        clear_input_buffer();

        switch (choice) {
            case 1: { // Gerenciar Matérias
                int subject_menu_choice;
                do {
                    display_student_subject_menu();
                    subject_menu_choice = get_int_input("Escolha uma opção: ");
                    clear_input_buffer();
                    switch (subject_menu_choice) {
                        case 1: { // Matricular-se em Matéria
                            int subject_id = get_int_input("ID da Matéria para matricular-se: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, subject_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_ENROLL_SUBJECT, student_id, input_buffer);
                            printf("Resposta: %s\n", server_resp.message);
                            break;
                        }
                        case 2: { // Desmatricular-se de Matéria
                            int subject_id = get_int_input("ID da Matéria para desmatricular-se: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, subject_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_UNENROLL_SUBJECT, student_id, input_buffer);
                            printf("Resposta: %s\n", server_resp.message);
                            break;
                        }
                        case 3: { // Listar Matérias Matriculadas
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_LIST_ENROLLED_SUBJECTS, student_id, "");
                            printf("\n--- SUAS MATÉRIAS MATRICULADAS ---\n");
                            printf("%s\n", server_resp.data);
                            printf("------------------------------------\n");
                            break;
                        }
                        case 0: break;
                        default: printf("Opção inválida. Tente novamente.\n"); break;
                    }
                } while (subject_menu_choice != 0);
                break;
            }
            case 2: { // Visualizar Conteúdo (Módulos, Aulas, Questionários)
                int module_view_choice;
                do {
                    display_student_module_menu();
                    module_view_choice = get_int_input("Escolha uma opção: ");
                    clear_input_buffer();
                    switch (module_view_choice) {
                        case 1: { // Listar Módulos de uma Matéria
                            int subject_id = get_int_input("ID da Matéria para listar módulos: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, subject_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_LIST_MODULES_IN_SUBJECT, student_id, input_buffer);
                            printf("\n--- MÓDULOS DA MATÉRIA ---\n");
                            printf("%s\n", server_resp.data);
                            printf("--------------------------\n");
                            break;
                        }
                        case 2: { // Visualizar Aulas de um Módulo
                            int module_id = get_int_input("ID do Módulo para visualizar aulas: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, module_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_VIEW_LESSONS_IN_MODULE, student_id, input_buffer);
                            printf("\n--- AULAS DO MÓDULO ---\n");
                            printf("%s\n", server_resp.data);
                            printf("-------------------------\n");
                            break;
                        }
                        case 3: { // Visualizar Questionários de um Módulo
                            int module_id = get_int_input("ID do Módulo para visualizar questionários: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, module_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_VIEW_QUIZZES_IN_MODULE, student_id, input_buffer);
                            printf("\n--- QUESTIONÁRIOS DO MÓDULO ---\n");
                            printf("%s\n", server_resp.data);
                            printf("----------------------------------\n");
                            break;
                        }
                        case 0: break;
                        default: printf("Opção inválida. Tente novamente.\n"); break;
                    }
                } while (module_view_choice != 0);
                break;
            }
            case 3: { // Fazer Questionário
                int quiz_menu_choice;
                do {
                    display_student_quiz_menu();
                    quiz_menu_choice = get_int_input("Escolha uma opção: ");
                    clear_input_buffer();
                    switch (quiz_menu_choice) {
                        case 1: { // Listar Questionários de um Módulo
                            int module_id = get_int_input("ID do Módulo para listar questionários: ");
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", student_id, module_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_VIEW_QUIZZES_IN_MODULE, student_id, input_buffer);
                            printf("\n--- QUESTIONÁRIOS DO MÓDULO ---\n");
                            printf("%s\n", server_resp.data);
                            printf("----------------------------------\n");
                            break;
                        }
                        case 2: { // Iniciar Questionário
                            int quiz_id = get_int_input("ID do Questionário para fazer: ");
                            
                            // 1. Solicitar perguntas do questionário ao servidor
                            snprintf(input_buffer, MAX_DATA_LEN, "%d", quiz_id);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_REQUEST_QUIZ_QUESTIONS, student_id, input_buffer);

                            if (server_resp.status != 0) {
                                printf("Erro ao obter perguntas do questionário: %s\n", server_resp.message);
                                break;
                            }
                            
                            if (strlen(server_resp.data) == 0) {
                                printf("Nenhuma pergunta encontrada para o questionário %d.\n", quiz_id);
                                break;
                            }

                            // 2. Analisar as perguntas e coletar respostas
                            char temp_data[MAX_DATA_LEN];
                            strncpy(temp_data, server_resp.data, MAX_DATA_LEN - 1);
                            temp_data[MAX_DATA_LEN - 1] = '\0';

                            char* line = strtok(temp_data, "\n");
                            if (line == NULL) {
                                printf("Formato de perguntas inválido.\n");
                                break;
                            }
                            int num_questions_from_server = atoi(line); // Primeira linha é o número de questões
                            
                            if (num_questions_from_server <= 0) {
                                printf("Nenhuma pergunta válida para o questionário %d.\n", quiz_id);
                                break;
                            }

                            char answers_buffer[MAX_DATA_LEN] = ""; // Buffer para armazenar as respostas do aluno
                            char answer_item[50]; // Para cada par Q_ID:Answer
                            int current_question_idx = 0;

                            line = strtok(NULL, "\n"); // Pular para a primeira pergunta
                            while (line != NULL && current_question_idx < num_questions_from_server) {
                                char q_text[MAX_STR_LEN], opt1[MAX_STR_LEN], opt2[MAX_STR_LEN], opt3[MAX_STR_LEN], opt4[MAX_STR_LEN];
                                int q_id;
                                int len_text, len_opt1, len_opt2, len_opt3, len_opt4;
                                int chosen_option;
                                char* current_pos = line;

                                // Parse o ID da pergunta
                                sscanf(current_pos, "%d|", &q_id);
                                current_pos = strchr(current_pos, '|') + 1; // Move para após o primeiro '|'

                                // Parse o comprimento do texto da pergunta e o texto
                                sscanf(current_pos, "%d|", &len_text);
                                current_pos = strchr(current_pos, '|') + 1;
                                strncpy(q_text, current_pos, len_text);
                                q_text[len_text] = '\0';
                                current_pos += len_text + 1; // Move para após o texto e o próximo '|'

                                // Parse o comprimento e o texto da opção 1
                                sscanf(current_pos, "%d|", &len_opt1);
                                current_pos = strchr(current_pos, '|') + 1;
                                strncpy(opt1, current_pos, len_opt1);
                                opt1[len_opt1] = '\0';
                                current_pos += len_opt1 + 1;

                                // Parse o comprimento e o texto da opção 2
                                sscanf(current_pos, "%d|", &len_opt2);
                                current_pos = strchr(current_pos, '|') + 1;
                                strncpy(opt2, current_pos, len_opt2);
                                opt2[len_opt2] = '\0';
                                current_pos += len_opt2 + 1;

                                // Parse o comprimento e o texto da opção 3
                                sscanf(current_pos, "%d|", &len_opt3);
                                current_pos = strchr(current_pos, '|') + 1;
                                strncpy(opt3, current_pos, len_opt3);
                                opt3[len_opt3] = '\0';
                                current_pos += len_opt3 + 1;

                                // Parse o comprimento e o texto da opção 4
                                sscanf(current_pos, "%d|", &len_opt4);
                                current_pos = strchr(current_pos, '|') + 1;
                                strncpy(opt4, current_pos, len_opt4);
                                opt4[len_opt4] = '\0';
                                // current_pos += len_opt4 + 1; // Não é necessário mover para o próximo '|' pois é o final da linha
                                
                                printf("\nPergunta %d (ID: %d): %s\n", current_question_idx + 1, q_id, q_text);
                                printf("1. %s\n", opt1);
                                printf("2. %s\n", opt2);
                                printf("3. %s\n", opt3);
                                printf("4. %s\n", opt4);
                                
                                do {
                                    chosen_option = get_int_input("Sua resposta (1-4): ");
                                    clear_input_buffer();
                                    if (chosen_option < 1 || chosen_option > 4) {
                                        printf("Opção inválida. Escolha entre 1 e 4.\n");
                                    }
                                } while (chosen_option < 1 || chosen_option > 4);

                                // Formatar a resposta e adicionar ao buffer de respostas
                                if (strlen(answers_buffer) > 0) {
                                    strcat(answers_buffer, ",");
                                }
                                snprintf(answer_item, sizeof(answer_item), "%d:%d", q_id, chosen_option);
                                strcat(answers_buffer, answer_item);

                                current_question_idx++;
                                line = strtok(NULL, "\n"); // Próxima pergunta
                            }
                            
                            // 3. Enviar as respostas do aluno ao servidor
                            // O input_buffer conterá "quiz_id,Q_ID:Answer_Option,Q_ID:Answer_Option,..."
                            snprintf(input_buffer, MAX_DATA_LEN, "%d,%s", quiz_id, answers_buffer);
                            server_resp = send_command_to_server(client_socket, CMD_STUDENT_TAKE_QUIZ, student_id, input_buffer);
                            printf("Resposta: %s\n", server_resp.message);
                            break;
                        }
                        case 0: break;
                        default: printf("Opção inválida. Tente novamente.\n"); break;
                    }
                } while (quiz_menu_choice != 0);
                break;
            }
            case 4: { // Visualizar Notas e Faltas
                server_resp = send_command_to_server(client_socket, CMD_STUDENT_VIEW_GRADES, student_id, "");
                printf("\n--- SUAS NOTAS ---\n");
                printf("%s\n", server_resp.data);
                printf("------------------\n");

                server_resp = send_command_to_server(client_socket, CMD_STUDENT_VIEW_ABSENCES, student_id, "");
                printf("\n--- SUAS FALTAS ---\n");
                printf("%s\n", server_resp.data);
                printf("-------------------\n");
                break;
            }
            case 0: // Logout
                running = 0;
                printf("Voltando ao Menu Principal.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

// Novo: Lógica para o Menu de Gerenciamento de Matérias e Turmas do Administrador
void handle_admin_professor_subject_class_assignment_menu(int admin_id, int client_socket) {
    int choice;
    int running = 1;
    char input_buffer[MAX_DATA_LEN];
    ServerResponse server_resp;

    while (running) {
        display_admin_professor_subject_class_assignment_menu();
        choice = get_int_input("");
        clear_input_buffer();

        switch (choice) {
            case 1: { // Atribuir Matéria a Professor
                int professor_id, subject_id;
                printf("ID do Professor: ");
                professor_id = get_int_input("");
                printf("ID da Matéria a atribuir: ");
                subject_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", professor_id, subject_id);
                server_resp = send_command_to_server(client_socket, CMD_ADMIN_ASSIGN_SUBJECT_TO_PROFESSOR, admin_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 2: { // Desatribuir Matéria de Professor
                int professor_id, subject_id;
                printf("ID do Professor: ");
                professor_id = get_int_input("");
                printf("ID da Matéria a desatribuir: ");
                subject_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", professor_id, subject_id);
                server_resp = send_command_to_server(client_socket, CMD_ADMIN_UNASSIGN_SUBJECT_FROM_PROFESSOR, admin_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 3: { // Atribuir Turma a Professor
                int professor_id, class_id;
                printf("ID do Professor: ");
                professor_id = get_int_input("");
                printf("ID da Turma a atribuir: ");
                class_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", professor_id, class_id);
                server_resp = send_command_to_server(client_socket, CMD_ADMIN_ASSIGN_CLASS_TO_PROFESSOR, admin_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 4: { // Desatribuir Turma de Professor
                int professor_id, class_id;
                printf("ID do Professor: ");
                professor_id = get_int_input("");
                printf("ID da Turma a desatribuir: ");
                class_id = get_int_input("");
                snprintf(input_buffer, MAX_DATA_LEN, "%d,%d", professor_id, class_id);
                server_resp = send_command_to_server(client_socket, CMD_ADMIN_UNASSIGN_CLASS_FROM_PROFESSOR, admin_id, input_buffer);
                printf("Resposta: %s\n", server_resp.message);
                break;
            }
            case 0: // Voltar
                running = 0;
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}
