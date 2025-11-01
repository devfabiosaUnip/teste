#include "storage/storage.h"
#include "models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SystemData global_data; // Inicializa a variável global

int save_data(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Erro ao abrir arquivo para salvar");
        return -1;
    }
    fwrite(&global_data, sizeof(SystemData), 1, file);
    fclose(file);
    printf("Dados salvos com sucesso em %s\n", filename);
    return 0;
}

void load_data(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir arquivo para carregar. Criando novos dados.\n");
        // Inicializa global_data com valores padrão se o arquivo não existe
        memset(&global_data, 0, sizeof(SystemData));
        // Adicionar um administrador padrão se não houver um
        if (global_data.num_admins == 0) {
            global_data.admins[0].id = 1;
            strcpy(global_data.admins[0].name, "Admin Master");
            strcpy(global_data.admins[0].email, "admin@admin.com");
            strcpy(global_data.admins[0].password, "admin123");
            global_data.num_admins = 1;
        }

        return;
    }
    fread(&global_data, sizeof(SystemData), 1, file);
    fclose(file);
    printf("Dados carregados com sucesso de %s\n", filename);

    // Garante que haja pelo menos um admin se o arquivo estava vazio ou corrompido sem admins
    if (global_data.num_admins == 0) {
        global_data.admins[0].id = 1;
        strcpy(global_data.admins[0].name, "Admin Master");
        strcpy(global_data.admins[0].email, "admin@admin.com");
        strcpy(global_data.admins[0].password, "admin123");
        global_data.num_admins = 1;
    }
}
