#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ_USUARIO   "usuarios.csv"   /* arquivo usuario */
#define ARQ_AUTOR   "autores.csv"   /* arquivo autor */
#define ARQ_LIVRO   "livros.csv"   /* arquivo livros */
#define ARQ_TEMP  "temp.csv"     /* arquivo auxiliar */
#define MAX_NOME  80
#define COD_INICIAL  1000
#define TAM_BUFFER 512


typedef struct {
    int cod;
    char nome[MAX_NOME];
    int status; /*0 = ATIVO, 1 = INATIVO, 2 = SUSPENSO*/
}Usuario;
typedef struct {
    int cod;
    char nome[MAX_NOME]; //Nome completo + Espaços
}Autor;
typedef struct {
    int cod;
    char titulo[MAX_NOME]; //Nome completo + Espaços
    int codAutores[3]; //Nome completo + Espaços
    int estoque;
}Livro;

void limpar_tela(void);
void limpar_buffer(void);
int prox_codigo(int tipo);
void excluirArquivo(int tipo);
int listar(int tipo);
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
    FILE *arqAutor = fopen(ARQ_AUTOR, "r");
    FILE *arqLivro = fopen(ARQ_LIVRO, "r");

    if (arqUser == NULL) {
        arqUser = fopen(ARQ_USUARIO, "w");
        if (arqUser != NULL) {
            fprintf(arqUser, "cod;nome;status\n");
            fclose(arqUser);
        }
    } else {
        fclose(arqUser);
    }
    if (arqAutor == NULL) {
        arqAutor = fopen(ARQ_AUTOR, "w");
        if (arqAutor != NULL) {
            fprintf(arqAutor, "cod;nome\n");
            fclose(arqAutor);
        }
    } else {
        fclose(arqAutor);
    }
    if (arqLivro == NULL) {
        arqLivro = fopen(ARQ_LIVRO, "w");
        if (arqLivro != NULL) {
            fprintf(arqLivro, "cod;título;Estoque;codAutores\n");
            fclose(arqLivro);
        }
    } else {
        fclose(arqLivro);
    }
}
int prox_codigo(int tipo){//Esse Sistema Serve para Vasculhar o Arquivo e Atribuir o Valor de Forma Dinâmica

    char linha[TAM_BUFFER];
    FILE *arq = NULL; 
      
    switch (tipo){
        case 1:
        arq =  fopen(ARQ_USUARIO, "r");
        break;
        case 2:
            arq =  fopen(ARQ_AUTOR, "r");
            break;
        case 3:
            arq =  fopen(ARQ_LIVRO, "r");
            break;
        default:
            return 0;
            break;
    }
    //Case de Arquivo usuario
    
    
    //Programa Geralzão
    //Percorre todo o arquivo a procura do maior valor de Código Registrado
    int cod;
    int maior = COD_INICIAL; //1000
    if (arq == NULL) return COD_INICIAL;
    char *token;
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        token = strtok(linha, ";");
        if (token != NULL) cod = atoi(token);
        if (cod > maior) maior = cod;
    }
    fclose(arq);
    return maior + 1; //Descobrindo o Maior ele retorna o próximo número 
}
void excluirArquivo(int tipo){
    //Tipo 01: Usuario | 02:Autor | 03:Livro 
    switch (tipo){
        case 1:
                remove(ARQ_USUARIO);
                rename(ARQ_TEMP, ARQ_USUARIO);
            break;
        case 2:
                remove(ARQ_AUTOR);
                rename(ARQ_TEMP, ARQ_AUTOR);
            break;
        case 3:
                remove(ARQ_LIVRO);
                rename(ARQ_TEMP, ARQ_LIVRO);
            break;
        
        default:
            break;
    }
    //Após apagar o arquivo temporário, é preciso recria-lo, nessa etapa  
    garantir_arquivo();
    
};

//Crud do Objeto Usuário
void cadastrar(int tipo) {
    char linha[TAM_BUFFER];
    FILE *arq = NULL;

    switch (tipo){ //Tipo 01: Usuario | 02:Autor | 03:Livro 
        case 1:{ 
            arq = fopen(ARQ_USUARIO, "a");   /* modo append */
            Usuario a;
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                return;
            }
        
            a.cod = prox_codigo(1);
            if (a.cod == 0)
            {
                printf("Error, Tente de novo\n");
                fclose(arq);
                return;
            }
            //Atribuição do Nome, com limitador
            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin);
            a.nome[strcspn(a.nome, "\n")] = '\0';
            a.status = 0;
            fprintf(arq, "\n%d;%s;%d\n", a.cod, a.nome, a.status);

            fclose(arq);
            printf("Usuário cadastrado com sucesso!\n");
            break;
        }
        case 2:{
            arq = fopen(ARQ_AUTOR, "a");   /* modo append, adiciona um novo objeto no final do arquivo */
            Autor a;
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_AUTOR);
                return;
            }
            a.cod = prox_codigo(2); //Tipo 01: Usuario | 02:Autor | 03:Livro 
            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin); //Atribuição do Nome, com limitador
            a.nome[strcspn(a.nome, "\n")] = '\0';
            fprintf(arq, "\n%d;%s;\n", a.cod, a.nome);

            fclose(arq);
            printf("Autor cadastrado com sucesso!\n");
            break;}
        
        case 3:{
            arq = fopen(ARQ_LIVRO, "a");   /* modo append, adiciona um novo objeto no final do arquivo */
            Livro a;

            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_LIVRO);
                return;
            }

            a.cod = prox_codigo(3); //Tipo 01: Usuario | 02:Autor | 03:Livro 
            printf("Titulo do Livro: ");
            fgets(a.titulo, MAX_NOME, stdin); //Atribuição do Nome, com limitador de 80 caracteres
            a.titulo[strcspn(a.titulo, "\n")] = '\0';

            printf("Defina o Estoque do livro %s:\n", a.titulo);
            scanf("%d", &a.estoque);
            limpar_buffer();
            
            //Aqui começa o registro
            
            a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;
            for ( int i = 0; i < 3; i++)
            {   
                listar(2); //Além de Printar a lista de Autores, ele retorna a quantidade de autores, temos registrados
                int autores[1000], op = 0, autorEscolhido, autorExiste = 0;
                char operator;
                //Função pra criar um array com todos os códigos dos autores

                FILE *arqAutor = fopen(ARQ_AUTOR,"r");
                 if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_AUTOR);
                return;
                }

                

                char *token;
                fgets(linha, sizeof(linha), arqAutor);//Pula o Cabeçalho
                while (op < 1000 && fgets(linha, sizeof(linha), arqAutor) != NULL) {
                    token = strtok(linha, ";");//Pego o primeiro valor do arquivo Autor(Código), e coloco dentro do meu array
                    if (token != NULL) autores[op++] = atoi(token);
                }
                fclose(arqAutor);

                
                printf("Escreva o Cod do Autor: \nSe não foi registrado, digite 0\n");
                scanf("%d", &autorEscolhido);
                limpar_buffer();
                    for (int j = 0; j < op; j++){
                        if (autorEscolhido == autores[j]) autorExiste = 1;
                    }
                if (autorExiste){
                    printf("Autor Adicionado com sucesso!");
                }
                else{
                    printf("Cadastre o Novo Autor\n");
                    cadastrar(2);
                    autorEscolhido = prox_codigo(2) - 1; //Por meio desse método, adciono o código do último autor registrado
                }

                a.codAutores[i] = autorEscolhido;//No final, recebo o valor de autorEscolhido como item da minha lista de autores

                
                if (i < 2) {
                    printf("Pretende listar mais algum autor?(S/N)\n");
                    scanf(" %c", &operator);
                    limpar_buffer();
                    if (operator == 'N' || operator == 'n') break;
                    limpar_tela();
                }
            }
            
            
            fprintf(arq, "\n%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo,a.estoque,a.codAutores[0],a.codAutores[1],a.codAutores[2]);
            fclose(arq);
            printf("Livro cadastrado com sucesso!\n");
            break;
        }
        
        default:
            break;
    }
    
    
}

int listar(int tipo) {
    
    char linha[TAM_BUFFER];
    FILE *arq = NULL; 
    int total = 0;//Mede a Quantidade de Objetos dentro de um arquivo
    switch (tipo){
        case 1:{
        
            Usuario a;        
            arq = fopen(ARQ_USUARIO, "r");
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                return 0;
            }
            fgets(linha, sizeof(linha), arq);  /* pula o cabecalho */
            printf("\n%-5s | %-5s | %s\n", "CODIGO", "NOME", "STATUS");
            printf("----------------------------------------------------\n");

            char conversorStatus[10];
            while (fscanf(arq, "%d;%[^;];%d", &a.cod, a.nome, &a.status) == 3) { 
                (a.status == 0)? strcpy(conversorStatus, "ATIVO") : (a.status == 1)? strcpy(conversorStatus, "INATIVO"): strcpy(conversorStatus, "SUSPENSO");
                printf("%-5d | %s | %s\n", a.cod, a.nome, conversorStatus);
                total++;
            }
            break;
            }
        case 2:{
            arq = fopen(ARQ_AUTOR, "r");   
            Autor a;
            
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_AUTOR);
                return 0;
            }
            fgets(linha, sizeof(linha), arq);  /* pula o cabecalho */
            printf("\n%-6s | %s\n", "CODIGO", "NOME");
            printf("----------------------------------------------------\n");

            while (fgets(linha, sizeof(linha), arq) != NULL) {
                linha[strcspn(linha, "\r\n")] = '\0';

                if (sscanf(linha, "%d;%99[^;]", &a.cod, a.nome) != 2)
                    continue;

                printf("%-6d | %s\n", a.cod, a.nome);
                total++;
            }
            break;
        }
        case 3:{
            FILE *arqAutor = fopen(ARQ_AUTOR, "r"); 
            arq = fopen(ARQ_LIVRO, "r"); 
            Livro a;
            char linhaAutor[TAM_BUFFER]; // Buffer separado para autores
            
            if (arq == NULL || arqAutor == NULL) {
                printf("Erro ao abrir o arquivo, Autor e Livro\n");
                if (arq) fclose(arq);
                if (arqAutor) fclose(arqAutor);
                return 0;
            }


            fgets(linha, sizeof(linha), arq);  /* pula o cabecalho */
            printf("\n%-6s | %-30s | %-7s | %s\n", "CODIGO", "TITULO", "ESTOQUE", "AUTOR(ES)");
            printf("----------------------------------------------------\n");
            
            
             while (fgets(linha, sizeof(linha), arq) != NULL) {
                linha[strcspn(linha, "\r\n")] = '\0';

                a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;

                int lidos = sscanf(linha, "%d;%99[^;];%d;%d;%d;%d",
                                   &a.cod, a.titulo, &a.estoque,
                                   &a.codAutores[0], &a.codAutores[1], &a.codAutores[2]);
                if (lidos < 3)
                    continue;

                printf("%-6d | %-30s | %-7d |", a.cod, a.titulo, a.estoque);

                /*loop, para transformar códigos em nomes de autores*/
                for (int i = 0; i < 3; i++) {
                    if (a.codAutores[i] <= 0)
                        continue;

                    rewind(arqAutor);                              /* volta ao início */
                    fgets(linhaAutor, sizeof(linhaAutor), arqAutor); /* pula cabecalho */

                    while (fgets(linhaAutor, sizeof(linhaAutor), arqAutor) != NULL) {
                        linhaAutor[strcspn(linhaAutor, "\r\n")] = '\0';

                        char *token = strtok(linhaAutor, ";");
                        if (token != NULL && atoi(token) == a.codAutores[i]) {
                            token = strtok(NULL, ";");
                            if (token != NULL)
                                printf(" %s;", token);
                            break;
                        }
                    }
                }
                printf("\n");
                total++;
            }
            fclose(arqAutor);
            break;
        }
        default:
            return 0;
    }   
    if (arq) fclose(arq);
    return total;
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
            int tipo = 1;
            break;
                
        }
    
        fclose(arq);
        fclose(temp);

        if (atualizado) {
            excluirArquivo(tipo);
        } else {
            remove(ARQ_TEMP);
            printf("Cod:%d nao encontrado.\n", mat);
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
    int mat,  excluido = 0;
    FILE *arq = NULL; 
    FILE *temp = fopen(ARQ_TEMP, "w");

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
            printf("Error, na Etapa de Exclusão");
            break;
        }
    
    
}

int main(void){
    limpar_tela();
    garantir_arquivo();
    listar(3);
    
}