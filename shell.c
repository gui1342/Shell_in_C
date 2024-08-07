#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#define TRUE 1
#define COMAND_MAX_SIZE 1024

void parse_command(char *linha_de_comando, char **parametros) {
    char *ptr = linha_de_comando;
    int i = 0;
    int em_aspas = 0;
    char tipo_aspa;

    // Lê os parâmetros
    while (*ptr) {
        // Salta espaços
        while (isspace(*ptr)) ptr++;

        if (*ptr == '\0') break;

        // Checa se estamos dentro de aspas
        if (*ptr == '\'' || *ptr == '\"') {
            em_aspas = 1;
            tipo_aspa = *ptr;
            ptr++;
            char *start = ptr; // início do parâmetro
            while (*ptr && (*ptr != tipo_aspa)) {
                ptr++;
            }
            // Armazena o parâmetro
            int len = ptr - start;
            parametros[i] = (char *)malloc(len + 1);
            strncpy(parametros[i], start, len);
            parametros[i][len] = '\0';
            i++;

            // Avança o ponteiro além das aspas de fechamento
            if (*ptr == tipo_aspa) {
                ptr++;
            }
        } else {
            char *start = ptr; // início do parâmetro
            while (*ptr && !isspace(*ptr) && *ptr != '\'' && *ptr != '\"') {
                ptr++;
            }
            // Armazena o parâmetro
            int len = ptr - start;
            parametros[i] = (char *)malloc(len + 1);
            strncpy(parametros[i], start, len);
            parametros[i][len] = '\0';
            i++;
        }
    }

    // Define o último parâmetro como NULL para indicar o fim
    parametros[i] = NULL;
}


int main()
{
    char *linha_de_comando;
    char *comando;
    char **parametros;
    int status;
    char buffercwd[COMAND_MAX_SIZE];

    if ((comando = (char *)malloc(40 * sizeof(char))) == NULL) {
      printf ("Ocorreu um erro ao tentar alocar memória ao ponteiro ""comando""\n");
      exit (1);
    }
    if ((parametros = (char **)malloc(COMAND_MAX_SIZE * sizeof(char *))) == NULL) {
      printf ("Ocorreu um erro ao tentar alocar memória ao ponteiro ""parametros""\n");
      exit (1);
    }
     if (((linha_de_comando = (char *)malloc(COMAND_MAX_SIZE * sizeof(char)))) == NULL) {
      printf ("Ocorreu um erro ao tentar alocar memória ao ponteiro ""linha_de_comando""\n");
      exit (1);
    }


    while (TRUE)
    {   
        printf("Shell_do_Gui> %s > ", getcwd(buffercwd, COMAND_MAX_SIZE));

        if(fgets(linha_de_comando, COMAND_MAX_SIZE, stdin) == NULL){
            perror("Descupe, algo deu errado");
            exit(1);
        }
        linha_de_comando[strcspn(linha_de_comando, "\n")] = '\0'; // Remove a nova linha no final da entrada

        parse_command(linha_de_comando, parametros);
        comando = parametros[0];

        
        if (strcmp(comando, "cd") == 0){
            if (parametros[1] == NULL)
            {
                printf("Esse comando espera um argumento para ser executado\n");

            } else if (chdir(parametros[1]) != 0)
            {
                printf("Ocorreu um erro: o diretório \"%s\" não foi encontrado\n", parametros[1]);
            }
            continue;
        }

        if (strcmp(linha_de_comando, "exit") == 0) {
            break;
        }


        if (fork() != 0) {
            waitpid(-1, &status, 0);
            
        } else {
            char bin_path[50] = "/bin/";
            
            strcat(bin_path, comando);
            printf("bin: %s\n",bin_path);
            execve(bin_path, parametros, NULL);
        }
    }
    free(linha_de_comando);
    free(comando);
    free(parametros);
    return 0;
}
