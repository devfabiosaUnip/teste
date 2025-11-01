# Documentação do Projeto PIM

Este documento tem como objetivo fornecer um mapa para entender a estrutura de pastas e a funcionalidade de cada arquivo neste projeto.

## Estrutura de Pastas

```
PIM/
├── build/                        # Arquivos de objeto e executáveis gerados pela compilação
├── src/                          # Código fonte do projeto
│   ├── admin/                    # Módulo para funcionalidades administrativas (ex: CRUD de usuários)
│   │   ├── admin.c               # Implementação das funções administrativas
│   │   └── admin.h               # Definições de estruturas e protótipos para o módulo admin
│   ├── client/                   # Módulo do cliente (interface do usuário)
│   │   ├── client_main.c         # Ponto de entrada principal do cliente
│   │   ├── client.c              # Funções de comunicação e lógica do cliente
│   │   └── client.h              # Definições e protótipos para o módulo cliente
│   ├── common_utils/             # Funções utilitárias comuns usadas por cliente e servidor
│   │   ├── common_utils.c        # Implementação das funções utilitárias
│   │   └── common_utils.h        # Definições e protótipos para as utilidades comuns
│   ├── models.h                  # Definições de estruturas de dados para as entidades (ex: Aluno, Professor, Disciplina)
│   ├── professor/                # Módulo para funcionalidades do professor
│   │   ├── professor.c           # Implementação das funções do professor
│   │   └── professor.h           # Definições e protótipos para o módulo professor
│   ├── protocol.h                # Definições do protocolo de comunicação entre cliente e servidor
│   ├── server_logic/             # Lógica de negócio do servidor
│   │   ├── server_logic.c        # Implementação da lógica do servidor
│   │   └── server_logic.h        # Definições e protótipos para a lógica do servidor
│   ├── server.c                  # Implementação do servidor (gerenciamento de conexões)
│   ├── server.h                  # Definições e protótipos para o módulo do servidor
│   ├── storage/                  # Módulo de persistência de dados
│   │   ├── storage.c             # Implementação das funções de armazenamento (leitura/escrita em arquivo)
│   │   └── storage.h             # Definições e protótipos para o módulo de armazenamento
│   └── main.c                    # Ponto de entrada principal para o servidor
├── Makefile                      # Arquivo para automatizar a compilação do projeto
├── sistema_academico_client.exe  # Executável do cliente
├── sistema_academico_server.exe  # Executável do servidor
├── system_data.dat               # Arquivo de dados persistente do sistema
└── ... outros arquivos gerados ...
```

## Visão Geral dos Arquivos e Módulos

Esta seção detalha a função de cada arquivo e módulo dentro do diretório `src/`.

### Módulo `admin`
*   `src/admin/admin.c`: Contém as implementações das funções relacionadas à administração do sistema, como o gerenciamento de usuários (alunos, professores, administradores), disciplinas e turmas.
*   `src/admin/admin.h`: Define as estruturas de dados específicas para o módulo administrativo e declara os protótipos das funções implementadas em `admin.c`.

### Módulo `client`
*   `src/client_main.c`: É o arquivo principal que inicia a aplicação cliente. Ele lida com a inicialização da interface do usuário e o fluxo de controle inicial da aplicação cliente.
*   `src/client.c`: Contém as funções que gerenciam a interação do usuário, como exibição de menus, coleta de entrada de dados e a comunicação efetiva com o servidor.
*   `src/client.h`: Declara as estruturas e protótipos de funções usados pelo cliente, facilitando a organização e reuso do código em todo o módulo.

### Módulo `common_utils`
*   `src/common_utils.c`: Implementa funções utilitárias gerais que podem ser usadas tanto pelo cliente quanto pelo servidor, como manipulação de strings, validações de entrada ou outras operações comuns.
*   `src/common_utils.h`: Contém as declarações para as funções utilitárias comuns, permitindo que outros módulos as incluam e usem em seus respectivos códigos.

### Arquivos de Definição Global
*   `src/models.h`: Define as estruturas de dados para as principais entidades do sistema, como `Aluno`, `Professor`, `Disciplina`, `Turma`, etc. Essas estruturas são fundamentais e usadas em todo o projeto para representar os dados.
*   `src/protocol.h`: Define o formato das mensagens e comandos utilizados na comunicação entre o cliente e o servidor. Isso é crucial para garantir que ambos os lados entendam as informações que estão sendo trocadas de forma padronizada.

### Módulo `professor`
*   `src/professor/professor.c`: Contém as implementações das funções específicas para o perfil de professor, como lançamento de notas, gerenciamento de turmas associadas e visualização de informações de alunos.
*   `src/professor/professor.h`: Define as estruturas e protótipos de funções para o módulo do professor, organizando o código e facilitando sua inclusão em outras partes do sistema.

### Módulo `server`
*   `src/server.c`: Implementa a lógica principal do servidor, incluindo o gerenciamento de conexões de clientes, a escuta por novas requisições e a coordenação geral da comunicação.
*   `src/server.h`: Contém as declarações de funções e estruturas relacionadas ao servidor, servindo como uma interface para o módulo do servidor.

### Módulo `server_logic`
*   `src/server_logic/server_logic.c`: Implementa a lógica de negócios central do servidor. É responsável por processar as requisições dos clientes, interagir com o módulo de armazenamento e aplicar as regras de negócio do sistema.
*   `src/server_logic/server_logic.h`: Define os protótipos de funções e estruturas utilizadas pela lógica do servidor, garantindo uma interface clara para a comunicação com outras partes do servidor.

### Módulo `storage`
*   `src/storage/storage.c`: Implementa as funções de persistência de dados, como leitura e escrita de informações em arquivos (ex: `system_data.dat`). É responsável por carregar o estado do sistema na inicialização e salvá-lo durante a execução.
*   `src/storage/storage.h`: Declara as funções para o módulo de armazenamento de dados, permitindo que outros módulos solicitem operações de persistência de forma abstrata.

### Ponto de Entrada do Servidor
*   `src/main.c`: O ponto de entrada principal para a aplicação do servidor. Ele é responsável por inicializar o servidor, configurar os ouvintes de conexão e iniciar o ciclo de processamento de requisições de clientes.

## Outros Arquivos Importantes

*   `Makefile`: Um script que automatiza o processo de compilação do projeto. Ele define como os arquivos `.c` devem ser compilados em arquivos de objeto `.o` e, em seguida, linkados para criar os executáveis do cliente e do servidor.
*   `system_data.dat`: Este arquivo é usado para armazenar os dados persistentes do sistema (alunos, professores, disciplinas, etc.) entre as execuções do programa. Ele é manipulado pelo módulo `storage`.
