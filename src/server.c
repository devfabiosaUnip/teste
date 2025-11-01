#include <stdio.h>
#include <stdlib.h>
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
#include "models.h"
#include "storage/storage.h"
#include "server_logic/server_logic.h"
#include "server.h"

#define MAX_CLIENTS 5

// Função para lidar com um cliente individual
void handle_client(int client_socket) {
    ClientMessage client_msg;
    ssize_t bytes_received;

    printf("Cliente conectado (socket %d).\n", client_socket);

    // Loop para receber e responder mensagens do cliente
    while ((bytes_received = recv(client_socket, (char*)&client_msg, sizeof(ClientMessage), 0)) > 0) {
        printf("Mensagem recebida do cliente (socket %d): Comando %d, User ID %d, Data: %s\n", 
               client_socket, client_msg.command, client_msg.user_id, client_msg.data);

        ServerResponse server_resp = process_client_command(client_msg);

        send(client_socket, (char*)&server_resp, sizeof(ServerResponse), 0);
        printf("Resposta enviada ao cliente (socket %d): Status %d, Mensagem: %s\n", 
               client_socket, server_resp.status, server_resp.message);
    }

    if (bytes_received == 0) {
        printf("Cliente desconectado (socket %d).\n", client_socket);
    } else if (bytes_received == -1) {
        perror("Erro ao receber dados do cliente");
    }

#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
}

void start_server(int port) {
#ifdef _WIN32
    WSADATA wsa;
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Erro. Código de erro: %d\n", WSAGetLastError());
        return;
    }
    printf("Winsock inicializado.\n");
#endif

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Carrega dados do sistema (apenas uma vez no servidor)
    load_data("system_data.dat");

    // Cria o socket do servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar socket do servidor");
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Escuta em todas as interfaces
    server_addr.sin_port = htons(port);

    // Bind do socket ao endereço
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao fazer bind do socket");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#endif
        return;
    }

    // Começa a escutar por conexões
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Erro ao escutar no socket");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#endif
        return;
    }

    printf("Servidor escutando na porta %d...\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Erro ao aceitar conexão");
            continue;
        }
        printf("Conexão aceita do %s:%d (socket %d)\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_socket);

        handle_client(client_socket);
    }

#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif
}
