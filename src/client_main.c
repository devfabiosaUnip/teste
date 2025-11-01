#include <stdio.h>
#include <stdlib.h>
#include "client.h"

#ifdef _WIN32
#include <windows.h> // Necessário para SetConsoleOutputCP
#endif

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
#ifdef _WIN32
    // Configura a página de código do console para UTF-8 no Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    printf("Iniciando o cliente de sistema acadêmico...\n");
    start_client(SERVER_IP, SERVER_PORT);
    return 0;
}
