#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ_USUARIO   "usuarios.csv"   /* arquivo principal */
#define ARQ_TEMP  "temp.csv"     /* arquivo auxiliar */
#define MAX_NOME  80
#define COD_INICIAL  1000
#define TAM_BUFFER 512


typedef struct {
    int cod;
    char nome[MAX_NOME];
    int status; /*0 = ATIVO, 1 = INATIVO, 2 = SUSPENSO*/
}Usuario;

void limpar_tela(void);
void limpar_buffer(void);
int prox_codigo(int tipo);
void excluirArquivo(int tipo);

void listar(int tipo);
void cadastrar(int tipo);
//Funções que Otimizam o Código
void limpar_tela(void) {
    printf("\033[2J\033[H");  /* limpa a tela e posiciona o cursor no topo */
}
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;  
}
void garantir_arquivo(void) {
    FILE *arqUser = fopen(ARQ_USUARIO, "r");

    if (arqUser == NULL) {
        arqUser = fopen(ARQ_USUARIO, "w");
        if (arqUser != NULL) {
            fprintf(arqUser, "cod;nome;status\n");
            fclose(arqUser);
        }
    } else {
        fclose(arqUser);
    }
}
int prox_codigo(int tipo){//Esse Sistema Serve para Vasculhar o Arquivo e Atribuir o Valor de Forma Dinâmica

    char linha[TAM_BUFFER];
    FILE *arq = NULL; 
      
    switch (tipo){
    case 1:
       arq =  fopen(ARQ_USUARIO, "r");
       break;
        
    default:
        return 0;
        break;
    
    
    }
    //Case de Arquivo usuario
    
    
    //Programa Geralzão
    int cod;
    int maior = COD_INICIAL; //999
    if (arq == NULL) return COD_INICIAL;
    char *token;
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        token = strtok(linha, ";");
        if (token != NULL) cod = atoi(token);
        if (cod > maior) maior = cod;
    }

   
 
    fclose(arq);
    return maior + 1;
}
void excluirArquivo(int tipo){
    switch (tipo){
        case 1:
                remove(ARQ_USUARIO);
                rename(ARQ_TEMP, ARQ_USUARIO);
            break;
        
        default:
            break;
    }

    garantir_arquivo();
    
};

//Crud do Objeto Usuário
void cadastrar(int tipo) {
    Usuario a;
    FILE *arq = NULL;

    switch (tipo){
        case 1:
            arq = fopen(ARQ_USUARIO, "a");   /* modo append */

            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                return;
            }
        
            a.cod = prox_codigo(1);
            if (a.cod == 0)
            {
                printf("Error, Tente de novo\n");
                return;
            }
            //Atribuição do Nome, com limitador
            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin);
            a.nome[strcspn(a.nome, "\n")] = '\0';
            a.status = 0;
            fprintf(arq, "%d;%s;%d\n", a.cod, a.nome, a.status);

            fclose(arq);
            printf("Aluno cadastrado com sucesso!\n");
            break;
        
        default:
            break;
    }
    
    
}

void listar(int tipo) {
    Usuario a;
    char linha[TAM_BUFFER];
    FILE *arq = NULL; 
    switch (tipo){
        case 1:
                arq = fopen(ARQ_USUARIO, "r");
                if (arq == NULL) {
                    printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                    return;
                }
                fgets(linha, sizeof(linha), arq);  /* pula o cabecalho */
                printf("\n%-5s | %-5s | %s\n", "CODIGO", "NOME", "STATUS");
                printf("----------------------------------------------------\n");

                char conversorStatus[10];
                while (fscanf(arq, "%d;%[^;];%d", &a.cod, a.nome, &a.status) == 3) { 
                    (a.status == 0)? strcpy(conversorStatus, "ATIVO") : (a.status == 1)? strcpy(conversorStatus, "INATIVO"): strcpy(conversorStatus, "SUSPENSO");
                    printf("%-5d | %s | %s\n", a.cod, a.nome, conversorStatus);
                }
    fclose(arq);
            
            break;
        
        default:
            break;
    }

}

void atualizar(int tipo) {
    Usuario a;
    char linha[TAM_BUFFER];
    int mat,escolha;

    FILE *arq = NULL; 
    FILE *temp = fopen(ARQ_TEMP, "w");
    int atualizado = 0;
    switch (tipo){
    case 1:
        arq  = fopen(ARQ_USUARIO, "r");
        if (arq == NULL || temp == NULL) {
            printf("Erro ao abrir os arquivos.\n");
            if (arq) fclose(arq);
            if (temp) fclose(temp);
            return;
            }
        listar(1);
        printf("Cod Usuario a atualizar: ");
        scanf("%d", &mat);
        limpar_buffer();

        fgets(linha, sizeof(linha), arq);
        fputs(linha, temp);

        while (fscanf(arq, "%d;%[^;];%d", &a.cod, a.nome, &a.status) == 3) {
            if (a.cod == mat){   
                char conversorStatus[20];
                (a.status == 0)? strcpy(conversorStatus, "ATIVO") : (a.status == 1)? strcpy(conversorStatus, "INATIVO"): strcpy(conversorStatus, "SUSPENSO");
                limpar_tela();
                printf("Usuario encontrado: %s\n", a.nome);
                
                printf("Pretende atualizar oq?\n");
                printf("+----------------------+\n");
                printf(">>Cod:%d(Imutável)\n", a.cod);
                printf("1- Nome: %s\n", a.nome);
                printf("2- Status: %s\n", conversorStatus);
                printf("3- Nada(Sair)\n");
                printf("+----------------------+\n");
                scanf("%d",&escolha);
                limpar_buffer();
                
                if (escolha == 1){
                    printf("Novo nome: ");
                    fgets(a.nome, MAX_NOME, stdin);
                    a.nome[strcspn(a.nome, "\n")] = '\0';
                }
                else if (escolha == 2){   
                    do{
                        printf("0-ATIVO\n1-INATIVO\n2-SUSPENSO\n");
                        printf("Novo Status: ");
                        scanf("%d", &a.status);
                        if (a.status > 2 || a.status < 0)
                        {   limpar_tela();
                            printf("Valor incorreto, tente de novo\n");
                        }
                        limpar_buffer();
                    } while(a.status > 2 || a.status < 0);
                }
                else{
                    printf("Tchau, Tchau!\n");
                }
                fprintf(temp,"%d;%s;%d\n", a.cod, a.nome, a.status);
                printf("Usuario Atualizado com sucesso\n");
                atualizado = 1;
            }
            else {
                fprintf(temp,"%d;%s;%d\n", a.cod, a.nome, a.status);
            }
                
        }
    
        fclose(arq);
        fclose(temp);

        if (atualizado) {
            excluirArquivo(1);
        } else {
            remove(ARQ_TEMP);
            printf("Usuário de Cod:%d nao encontrado.\n", mat);
            garantir_arquivo();
        }
            break;
        
        default:
            break;
        }
    
    
}


void excluir(int tipo) {
    Usuario a;
    char linha[TAM_BUFFER];
    int mat;
    FILE *arq = NULL; 
    FILE *temp = fopen(ARQ_TEMP, "w");
    int excluido = 0;

    switch (tipo){
    case 1:
        arq  = fopen(ARQ_USUARIO, "r");
        if (arq == NULL || temp == NULL) {
            printf("Erro ao abrir os arquivos.\n");
            if (arq) fclose(arq);
            if (temp) fclose(temp);
            return;
            }
        listar(1);
        printf("Cod Usuario a Deletar: ");
        scanf("%d", &mat);
        limpar_buffer();

        fgets(linha, sizeof(linha), arq);
        fputs(linha, temp);

        while (fscanf(arq, "%d;%[^;];%d", &a.cod, a.nome, &a.status) == 3) {
            if (a.cod != mat){   
                fprintf(temp,"%d;%s;%d\n", a.cod, a.nome, a.status);
                
            }
            else {
                printf("Usuario Excluido com sucesso\n");
                excluido = 1;
            }
                
        }
    
        fclose(arq);
        fclose(temp);

        if (excluido) {
            excluirArquivo(1);
        } else {
            remove(ARQ_TEMP);
            printf("Usuário de Cod:%d nao encontrado.\n", mat);
            garantir_arquivo();
        }
            break;
        
        default:
            break;
        }
    
    
}


int main(void){
    limpar_tela();
    garantir_arquivo();
    excluir(1);
    listar(1);

}