#include <stdio.h>
#include <stdlib.h>
#include "server.h" // Inclui o cabeçalho do servidor

#define DEFAULT_PORT 8080

int main() {
    printf("Iniciando o servidor de sistema acadêmico...\n");
    start_server(DEFAULT_PORT);
    return 0;
}
