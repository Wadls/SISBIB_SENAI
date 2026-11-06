#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ_USUARIO   "usuarios.csv"   /* arquivo usuario */
#define ARQ_AUTOR   "autores.csv"   /* arquivo autor */
#define ARQ_LIVRO   "livros.csv"   /* arquivo livros */
#define ARQ_EMP   "emprestimos.csv"   /* arquivo empréstimos */
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
    char titulo[MAX_NOME];
    int codAutores[3]; 
    int estoque;
}Livro;
typedef struct {
    int cod;
    int codAluno;
    int codLivro[3]; //Cada usuário pode pegar no máximo 3 livros
    int status;
}Emprestimo;

void limpar_tela(void);
void limpar_buffer(void);
int prox_codigo(int tipo);
void excluirArquivo(int tipo);
void conversorCodAutor(char nomes[], int codAutores[]);
void escolherAutor(int codAutores[]);
int buscar_registro(const char *arquivo, int cod, char *linhaOut, size_t tam);
int alterar_estoque(int codLivro, int delta);
void garantir_arquivo(void);

//CRUD de Objetos
void cadastrar(int tipo);
int listar(int tipo);
void devolver(void);
void atualizar(int tipo);
void excluir(int tipo);
/*Essas variáveis servem, para que tanto a função de garantir arquivo quanto os cabeçalhos estejam otimizados*/
const char *ARQUIVOS[]   = {NULL, ARQ_USUARIO, ARQ_AUTOR, ARQ_LIVRO, ARQ_EMP};
const char *CABECALHOS[] = {NULL, "cod;nome;status", "cod;nome",
                                   "cod;titulo;estoque;a1;a2;a3",
                                   "cod;codAluno;livro1;livro2;livro3;status"};
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
    for (int i = 1; i <= 4; i++) {
        FILE *f = fopen(ARQUIVOS[i], "r");
        if (f) { fclose(f); continue; }
        if ((f = fopen(ARQUIVOS[i], "w"))) { fprintf(f, "%s\n", CABECALHOS[i]); fclose(f);}
    }
}
int prox_codigo(int tipo){//Esse Sistema Serve para Vasculhar o Arquivo e Atribuir o Valor de Forma Dinâmica

    char linha[TAM_BUFFER]; 
    if (tipo < 1 || tipo > 4) return 0;
    FILE *arq = fopen(ARQUIVOS[tipo], "r"); // 1 = Aluno | 2 = Autor | 3 = Livro | 4 = Empréstimo 
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
void conversorCodAutor(char nomes[], int codAutores[]){
    FILE *arqAutor = fopen(ARQ_AUTOR, "r");
    char linhaAutor[TAM_BUFFER];
    nomes[0] = '\0';
    if (arqAutor == NULL) {
                printf("Erro ao abrir o arquivo, Autor\n");
                return;
            }
    for (int i = 0; i < 3; i++) {
                    if (codAutores[i] <= 0)
                        continue;
                    rewind(arqAutor);/* volta ao início */
                    fgets(linhaAutor, sizeof(linhaAutor), arqAutor); /* pula cabecalho */

                    while (fgets(linhaAutor, sizeof(linhaAutor), arqAutor) != NULL) {
                        
                        linhaAutor[strcspn(linhaAutor, "\r\n")] = '\0';
                        char *token = strtok(linhaAutor, ";");
                        if (token != NULL && atoi(token) == codAutores[i]) { // Se o código escritor for igual o código dentro do arq autor
                            token = strtok(NULL, ";");
                            if (token != NULL){
                                strncat(nomes,token,strlen(nomes) -1);
                                strncat(nomes,"; ",strlen(nomes) -1);
                            }
                            break;
                        }
                    }

    }
    fclose(arqAutor);
}
void escolherAutor(int codAutores[]){
    char linha[TAM_BUFFER];

    for ( int i = 0; i < 3; i++)
            {   
                listar(2); //Além de Printar a lista de Autores, ele retorna a quantidade de autores, temos registrados
                int  autorEscolhido, autorExiste = 0;
                char operator;
                //Função pra criar um array com todos os códigos dos autores

                FILE *arqAutor = fopen(ARQ_AUTOR,"r");
                 if (arqAutor == NULL) {
                    printf("Erro ao abrir o arquivo %s.\n", ARQ_AUTOR);
                    return;
                }

                printf("Escreva o Cod do Autor: \nSe nao foi registrado, digite 0\n");
                scanf("%d", &autorEscolhido);
                limpar_buffer();
                char *token;
                fgets(linha, sizeof(linha), arqAutor);//Pula o Cabeçalho
                while (fgets(linha, sizeof(linha), arqAutor) != NULL) {
                    token = strtok(linha, ";");//Pego o primeiro valor do arquivo Autor(Código), e coloco dentro do meu array
                    if (token != NULL && autorEscolhido == atoi(token)){
                        autorExiste = 1;
                        codAutores[i] = autorEscolhido;

                    };
                }
                fclose(arqAutor);
                if (autorExiste){
                    printf("Autor Adicionado com sucesso!");
                }
                else{
                    printf("Cadastre o Novo Autor\n");
                    cadastrar(2);
                    autorEscolhido = prox_codigo(2) - 1; //Por meio desse método, adciono o código do último autor registrado
                    codAutores[i] = autorEscolhido;
                }

                if (i < 2) {
                    printf("Pretende listar mais algum autor?(S/N)\n");
                    scanf(" %c", &operator);
                    limpar_buffer();
                    if (operator == 'N' || operator == 'n') break;
                    limpar_tela();
                }
            }
}

int buscar_registro(const char *arquivo, int cod, char *linhaOut, size_t tam) {
    FILE *f = fopen(arquivo, "r");
    if (f == NULL) return 0;

    char linha[TAM_BUFFER];
    int c;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (sscanf(linha, "%d", &c) == 1 && c == cod) {
            snprintf(linhaOut, tam, "%s", linha);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void devolver(void) {
    char linha[TAM_BUFFER];
    Emprestimo a;
    int cod, devolvido = 0, encontrado = 0;
    int livrosDevolvidos[3] = {0, 0, 0};

    FILE *arq = fopen(ARQ_EMP, "r");
    FILE *temp = fopen(ARQ_TEMP, "w");
    if (arq == NULL || temp == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        if (arq) fclose(arq);
        if (temp) fclose(temp);
        return;
    }

    listar(4);
    printf("Cod do Emprestimo a devolver: ");
    scanf("%d", &cod);
    limpar_buffer();

    fgets(linha, sizeof(linha), arq);   /* cabeçalho */
    fputs(linha, temp);

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';

        a.codLivro[0] = a.codLivro[1] = a.codLivro[2] = 0;
        if (sscanf(linha, "%d;%d;%d;%d;%d;%d", &a.cod, &a.codAluno,
                   &a.codLivro[0], &a.codLivro[1], &a.codLivro[2], &a.status) != 6)
            continue;

        if (a.cod == cod) {
            encontrado = 1;
            if (a.status == 1) {
                printf("Esse emprestimo ja foi devolvido.\n");
            } else {
                a.status = 1;
                devolvido = 1;
                for (int i = 0; i < 3; i++)          /* guarda os livros */
                    livrosDevolvidos[i] = a.codLivro[i];
            }
        }
        fprintf(temp, "%d;%d;%d;%d;%d;%d\n", a.cod, a.codAluno,
                a.codLivro[0], a.codLivro[1], a.codLivro[2], a.status);
    }
    fclose(arq);
    fclose(temp);

    if (devolvido) {
        remove(ARQ_EMP);
        rename(ARQ_TEMP, ARQ_EMP);

        for (int i = 0; i < 3; i++)
            if (livrosDevolvidos[i] > 0)
                alterar_estoque(livrosDevolvidos[i], +1);

        printf("Devolucao registrada com sucesso!\n");
    } else {
        remove(ARQ_TEMP);
        if (!encontrado) printf("Emprestimo %d nao encontrado.\n", cod);
    }
}
/* Soma 'delta' ao estoque do livro (use -1 ao emprestar, +1 ao devolver). Retorna 1 se deu certo */

int alterar_estoque(int codLivro, int delta) {
    FILE *arq = fopen(ARQ_LIVRO, "r");
    FILE *temp = fopen(ARQ_TEMP, "w");
    if (arq == NULL || temp == NULL) {
        if (arq) fclose(arq);
        if (temp) fclose(temp);
        return 0;
    }

    char linha[TAM_BUFFER];
    Livro a;
    int alterado = 0;

    fgets(linha, sizeof(linha), arq);   /* cabeçalho */
    fputs(linha, temp);

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;
        if (sscanf(linha, "%d;%79[^;];%d;%d;%d;%d", &a.cod, a.titulo, &a.estoque,
                   &a.codAutores[0], &a.codAutores[1], &a.codAutores[2]) < 3)
            continue;

        if (a.cod == codLivro && a.estoque + delta >= 0) {
            a.estoque += delta;
            alterado = 1;
        }
        fprintf(temp, "%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo, a.estoque,
                a.codAutores[0], a.codAutores[1], a.codAutores[2]);
    }
    fclose(arq);
    fclose(temp);

    if (alterado) excluirArquivo(3);
    else remove(ARQ_TEMP);
    return alterado;
}



//Crud do Objeto Usuário
void cadastrar(int tipo) {
   
    FILE *arq = NULL;

    switch (tipo){ //Tipo 01: Usuario | 02:Autor | 03:Livro | 04:Emprestimo
        case 1:{ 
            arq = fopen(ARQ_USUARIO, "a");   /* modo append */
            Usuario a;
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                return;
            }
        
            a.cod = prox_codigo(1);
            if (a.cod == 0){
                printf("Error, Tente de novo\n");
                fclose(arq);
                return;
            }
            //Atribuição do Nome, com limitador
            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin);
            a.nome[strcspn(a.nome, "\n")] = '\0';

            a.status = 0; // 0 = ATIVO | 1 = INATIVO | 2 = SUSPENSO

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
            a.cod = prox_codigo(2); //Tipo 01: Usuario | 02:Autor | 03:Livro | 04 Emprestimo

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
            escolherAutor(a.codAutores);
            fprintf(arq, "\n%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo,a.estoque,a.codAutores[0],a.codAutores[1],a.codAutores[2]);
            fclose(arq);
            printf("Livro cadastrado com sucesso!\n");
            break;
        }
        case 4:{
            Emprestimo a;
            char linha[TAM_BUFFER], nome[MAX_NOME], titulo[MAX_NOME];
            int codUser, statusUser, estoque, qtd = 0;

            a.codLivro[0] = a.codLivro[1] = a.codLivro[2] = 0;
            a.status = 0;

            /* 1) Aluno */
            listar(1);
            printf("Cod do Usuario: ");
            scanf("%d", &a.codAluno);
            limpar_buffer();

            if (!buscar_registro(ARQ_USUARIO, a.codAluno, linha, sizeof(linha)) ||
                sscanf(linha, "%d;%79[^;];%d", &codUser, nome, &statusUser) != 3) {
                printf("Usuario nao encontrado.\n");
                return;
            }
            if (statusUser != 0) {
                printf("Usuario nao esta ATIVO, emprestimo negado.\n");
                return;
            }

            /* 2) Livros (até 3) */
            for (int i = 0; i < 3; i++) {
                int codEscolhido, repetido = 0;

                listar(3);
                printf("Cod do Livro: ");
                scanf("%d", &codEscolhido);
                limpar_buffer();

                for (int j = 0; j < i; j++)
                    if (a.codLivro[j] == codEscolhido) repetido = 1;

                if (repetido) {
                    printf("Esse livro ja esta neste emprestimo.\n"); //Não permito emprestar dois livros iguais
                    i--;                                  
                    continue;
                }

                if (!buscar_registro(ARQ_LIVRO, codEscolhido, linha, sizeof(linha)) ||
                    sscanf(linha, "%d;%79[^;];%d", &codEscolhido, titulo, &estoque) != 3) {
                    printf("Livro nao encontrado.\n");
                    i--;
                    continue;
                }
                if (estoque <= 0) {
                    printf("Livro '%s' sem estoque.\n", titulo);
                    i--;
                    continue;
                }
                if (!alterar_estoque(codEscolhido, -1)) {
                    printf("Erro ao atualizar o estoque.\n");
                    break;
                }

                a.codLivro[qtd++] = codEscolhido;
                printf("Livro '%s' adicionado!\n", titulo);

                if (i < 2) {
                    char operator;
                    printf("Pretende emprestar mais algum livro?(S/N)\n");
                    scanf(" %c", &operator);
                    limpar_buffer();
                    if (operator == 'N' || operator == 'n') break;
                    limpar_tela();
                }
            }

            if (qtd == 0) {
                printf("Nenhum livro selecionado, emprestimo cancelado.\n");
                return;
            }

            /* 3) Grava */
            arq = fopen(ARQ_EMP, "a");
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_EMP);
                for (int i = 0; i < qtd; i++) alterar_estoque(a.codLivro[i], +1);  /* desfaz */
                return;
            }
            a.cod = prox_codigo(4); // 1 Aluno | 2 = Autor | 3 = Livro | 4 = Empréstimo
            fprintf(arq, "%d;%d;%d;%d;%d;%d\n", a.cod, a.codAluno,
                    a.codLivro[0], a.codLivro[1], a.codLivro[2], a.status);
            fclose(arq);
            printf("Emprestimo %d cadastrado com sucesso!\n", a.cod);
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
            arq = fopen(ARQ_LIVRO, "r"); 
            Livro a;
            char nomesAutores[TAM_BUFFER];
            
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo Livro\n");
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

                conversorCodAutor(nomesAutores,a.codAutores);
                printf("%s\n", nomesAutores);
                total++;
            }
            break;
        }
        case 4: {
            arq = fopen(ARQ_EMP, "r");
            Emprestimo a;
            char nomeAluno[MAX_NOME], titulos[TAM_BUFFER], tituloLivro[MAX_NOME];
            char linhaBusca[TAM_BUFFER];

            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_EMP);
                return 0;
            }

            fgets(linha, sizeof(linha), arq);  /* pula o cabecalho */
            printf("\n%-6s | %-20s | %-35s | %s\n", "CODIGO", "ALUNO", "LIVRO(S)", "STATUS");
            printf("----------------------------------------------------------------------------\n");

            while (fgets(linha, sizeof(linha), arq) != NULL) {
                linha[strcspn(linha, "\r\n")] = '\0';

                a.codLivro[0] = a.codLivro[1] = a.codLivro[2] = 0;
                if (sscanf(linha, "%d;%d;%d;%d;%d;%d", &a.cod, &a.codAluno,
                        &a.codLivro[0], &a.codLivro[1], &a.codLivro[2], &a.status) != 6)
                    continue;

                /* código do aluno -> nome */
                strcpy(nomeAluno, "(nao encontrado)");
                if (buscar_registro(ARQ_USUARIO, a.codAluno, linhaBusca, sizeof(linhaBusca))) {
                    int c;
                    sscanf(linhaBusca, "%d;%79[^;]", &c, nomeAluno);
                }

                /* códigos dos livros -> títulos */
                titulos[0] = '\0';
                for (int i = 0; i < 3; i++) {
                    if (a.codLivro[i] <= 0) continue;

                    strcpy(tituloLivro, "(nao encontrado)");
                    if (buscar_registro(ARQ_LIVRO, a.codLivro[i], linhaBusca, sizeof(linhaBusca))) {
                        int c;
                        sscanf(linhaBusca, "%d;%79[^;]", &c, tituloLivro);
                    }
                    strncat(titulos, tituloLivro, sizeof(titulos) - strlen(titulos) - 1);
                    strncat(titulos, "; ",        sizeof(titulos) - strlen(titulos) - 1);
                }

                printf("%-6d | %-20s | %-35s | %s\n", a.cod, nomeAluno, titulos,
                    a.status == 0 ? "EM ABERTO" : "DEVOLVIDO");
                total++;
            }
            break;
        }
        default:
            return 0;
    }   
    if (arq) fclose(arq);
    return total;
}

void atualizar(int tipo) {
    
    char linha[TAM_BUFFER];
    int mat,escolha;

    FILE *arq = NULL; 
    FILE *temp = fopen(ARQ_TEMP, "w");
    int atualizado = 0;
    switch (tipo){
    case 1:{
        Usuario a;
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
                printf(">>Cod:%d(Imutavel)\n", a.cod);
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
            else{
                fprintf(temp,"%d;%s;%d\n", a.cod, a.nome, a.status);
            }     
        }
        
            break;
        }
    case 2:{
        Autor a;
        arq  = fopen(ARQ_AUTOR, "r");
        if (arq == NULL || temp == NULL) {
            printf("Erro ao abrir os arquivos.\n");
            if (arq) fclose(arq);
            if (temp) fclose(temp);
            return;
        }
        listar(2);
        printf("Cod Autor para atualizar: ");
        scanf(" %d", &mat);
        limpar_buffer();

        fgets(linha, sizeof(linha), arq);
        fputs(linha, temp);

        while (fgets(linha, sizeof(linha), arq) != NULL) {
            linha[strcspn(linha, "\r\n")] = '\0';
            if (sscanf(linha, "%d;%79[^;]", &a.cod, a.nome) != 2) continue;   /* ignora linhas em branco */
            if (a.cod == mat){   
                limpar_tela();
                printf("Pretende atualizar oq?\n");
                printf("+----------------------+\n");
                printf(">>Cod:%d(Imutavel)\n", a.cod);
                printf("1- Nome: %s\n", a.nome);
                printf("2- Nada(Sair)\n");
                printf("+----------------------+\n");
                scanf("%d",&escolha);
                limpar_buffer();
                
                if (escolha == 1){
                    printf("Novo nome: ");
                    fgets(a.nome, MAX_NOME, stdin);
                    a.nome[strcspn(a.nome, "\n")] = '\0';
                }
                else{
                    printf("Tchau, Tchau!\n");
                }
                fprintf(temp,"%d;%s\n", a.cod, a.nome);
                printf("Autor Atualizado com sucesso Atualizado com sucesso\n");
                atualizado = 1;
            }
            else{
                fprintf(temp,"%d;%s\n", a.cod, a.nome);
            }     
        }
        
            break;
        }
    case 3:{
        Livro a;
        arq  = fopen(ARQ_LIVRO, "r");
        if (arq == NULL || temp == NULL) {
            printf("Erro ao abrir os arquivos.\n");
            if (arq) fclose(arq);
            if (temp) fclose(temp);
            return;
        }
        listar(3);
        printf("Cod Livro para atualizar: ");
        scanf("%d", &mat);
        limpar_buffer();

        fgets(linha, sizeof(linha), arq);
        fputs(linha, temp);

        while (fgets(linha, sizeof(linha), arq) != NULL) {
            linha[strcspn(linha, "\r\n")] = '\0';
            a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;
            if (sscanf(linha,"%d;%79[^;];%d;%d;%d;%d",&a.cod, a.titulo, &a.estoque,&a.codAutores[0], &a.codAutores[1], &a.codAutores[2]) < 3) continue;

            if (a.cod == mat){   
                limpar_tela();
                char nomes[TAM_BUFFER];
                conversorCodAutor(nomes,a.codAutores);
                printf("Pretende atualizar oq?\n");
                printf("+----------------------+\n");
                printf(">>Cod:%d(Imutavel)\n", a.cod);
                printf("1- Titulo %s\n", a.titulo);
                printf("2- Estoque %d\n", a.estoque);
                printf("3- Autores: %s\n", nomes);
                printf("4- Nada(Sair)\n");
                printf("+----------------------+\n");
                scanf("%d",&escolha);
                limpar_buffer();
                switch (escolha){
                    case 1:
                        printf("Novo Titulo: ");
                        fgets(a.titulo, MAX_NOME, stdin);
                        a.titulo[strcspn(a.titulo, "\n")] = '\0';
                        break;
                    case 2:
                        printf("Novo Estoque: ");
                        scanf("%d",&a.estoque);
                        limpar_buffer();
                        break;
                    case 3:
                        a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;
                        escolherAutor(a.codAutores);
                        break;   
                    default:
                        printf("Tchau, Tchau!\n");
                        fclose(arq);
                        fclose(temp);
                        remove(ARQ_TEMP);
                        return;

                    }

                fprintf(temp, "%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo, a.estoque, a.codAutores[0], a.codAutores[1], a.codAutores[2]);
                printf("Livro Atualizado com sucesso\n");
                atualizado = 1;
            }
            else{
                fprintf(temp, "%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo, a.estoque,a.codAutores[0], a.codAutores[1], a.codAutores[2]);
            }     
        }
        break;
        }
        
        default:
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
    
    
}

void excluir(int tipo) {
    char linha[TAM_BUFFER], lv[TAM_BUFFER];
    int cod, c, aluno, l[3], st, a[3], pend = 0, achou = 0;

    if (tipo < 1 || tipo > 3) return;
    listar(tipo);
    printf("Cod a excluir: ");
    scanf("%d", &cod);
    limpar_buffer();

    /* bloqueia se houver emprestimo em aberto ligado ao aluno/livro/autor */
    FILE *f = fopen(ARQ_EMP, "r");
    if (f) {
        fgets(linha, sizeof(linha), f);
        while (!pend && fgets(linha, sizeof(linha), f)) {
            if (sscanf(linha, "%d;%d;%d;%d;%d;%d", &c, &aluno, &l[0], &l[1], &l[2], &st) != 6 || st)
                continue;
            if (tipo == 1) pend = (aluno == cod);
            for (int k = 0; k < 3; k++) {
                if (tipo == 3) pend |= (l[k] == cod);
                if (tipo == 2 && buscar_registro(ARQ_LIVRO, l[k], lv, sizeof(lv))) {
                    a[0] = a[1] = a[2] = 0;
                    sscanf(lv, "%*d;%*[^;];%*d;%d;%d;%d", &a[0], &a[1], &a[2]);
                    pend |= (a[0] == cod || a[1] == cod || a[2] == cod);
                }
            }
        }
        fclose(f);
    }
    if (pend) { printf("Exclusao negada: emprestimo pendente.\n"); return; }

    /* copia tudo, menos a linha do codigo */
    f = fopen(ARQUIVOS[tipo], "r");
    FILE *t = fopen(ARQ_TEMP, "w");
    if (!f || !t) {
        printf("Erro ao abrir os arquivos.\n");
        if (f) fclose(f);
        if (t) fclose(t);
        return;
    }
    fgets(linha, sizeof(linha), f);
    fputs(linha, t);
    while (fgets(linha, sizeof(linha), f)) {
        if (atoi(linha) == cod) achou = 1;
        else if (linha[0] != '\n') fputs(linha, t);
    }
    fclose(f);
    fclose(t);

    if (achou) { excluirArquivo(tipo); printf("Excluido com sucesso.\n"); }
    else { remove(ARQ_TEMP); printf("Cod %d nao encontrado.\n", cod); }
}

int main(void){
    garantir_arquivo();
    limpar_tela();
    limpar_buffer();
    char opcao;
    int op = 0;
    do{
      limpar_buffer();
      printf("==================================\n");
      printf(" > >   Bem Vindo Ao SISBIB    < < \n");
      printf("==================================\n");
      printf("| 1 - Alunos:                    |\n");
      printf("| 2 - Autores:                   |\n");
      printf("| 3 - Livros:                    |\n");
      printf("| 4 - Emprestimos:               |\n");
      printf("| 5 - Sair                       |\n");
      printf("==================================\n");
      scanf("%c", &opcao);
      limpar_buffer();

      switch (opcao){
          case '1':
            do{
            limpar_tela();
            printf("==================================\n");
            printf("   > >   GESTAO DE ALUNOS   < <   \n");
            printf("==================================\n");
            printf("| 1 - Cadastrar Aluno:           |\n");
            printf("| 2 - Listar Alunos:             |\n");
            printf("| 3 - Atualizar Aluno:           |\n");
            printf("| 4 - Excluir ALuno              |\n");
            printf("| 5 - Sair                       |\n");
            printf("==================================\n");
            scanf("%d", &op);
            limpar_buffer();
            limpar_tela();
            switch (op){
                case 1:
                    cadastrar(1);
                    break;
                case 2:
                    listar(1);
                    break;
                case 3:
                    atualizar(1);
                    break;
                case 4:
                    excluir(1);
                    break;
                case 5:
                    break;;
                default:
                    printf("Valor Invalido, Tente Novamente, ");
                    break;
            }
            system("pause");
                } while (op < 1 || op > 5);
            break;
        case '2':
            do{
            limpar_tela();
            printf("==================================\n");
            printf("   > >   GESTAO DE AUTORES   < <   \n");
            printf("==================================\n");
            printf("| 1 - Cadastrar Autor:           |\n");
            printf("| 2 - Listar Autores:            |\n");
            printf("| 3 - Atualizar Autor:           |\n");
            printf("| 4 - Excluir Autor              |\n");
            printf("| 5 - Sair                       |\n");
            printf("==================================\n");
            scanf("%d", &op);
            limpar_buffer();
            limpar_tela();
            switch (op){
                case 1:
                    cadastrar(2);
                    break;
                case 2:
                    listar(2);
                    break;
                case 3:
                    atualizar(2);
                    break;
                case 4:
                    excluir(2);
                    break;
                case 5:
                    break;;
                default:
                    printf("Valor Invalido, Tente Novamente, ");
                    break;
            }
            system("pause");
                } while (op < 1 || op > 5);
            break;
        case '3':
            do{
            limpar_tela();
            printf("==================================\n");
            printf("   > >   GESTAO DE LIVROS   < <   \n");
            printf("==================================\n");
            printf("| 1 - Cadastrar Livro:           |\n");
            printf("| 2 - Listar Livros:             |\n");
            printf("| 3 - Atualizar Livro:           |\n");
            printf("| 4 - Excluir Livro              |\n");
            printf("| 5 - Sair                       |\n");
            printf("==================================\n");
            scanf("%d", &op);
            limpar_buffer();
            limpar_tela();
            switch (op){
                case 1:
                    cadastrar(3);
                    break;
                case 2:
                    listar(3);
                    break;
                case 3:
                    atualizar(3);
                    break;
                case 4:
                    excluir(3);
                    break;
                case 5:
                    break;;
                default:
                    printf("Valor Invalido, Tente Novamente, ");
                    break;
            }
            system("pause");
                } while (op < 1 || op > 5);
            break;
            
        case '4':
            do{
            limpar_tela();
            printf("==================================\n");
            printf("  > >   GESTAO DE EMPRESTIMOS  < < \n");
            printf("==================================\n");
            printf("| 1 - Novo Emprestimo:           |\n");
            printf("| 2 - Listar Emprestimos:        |\n");
            printf("| 3 - Devolver Livro:            |\n");
            printf("| 4 - Sair                       |\n");
            printf("==================================\n");
            scanf("%d", &op);
            limpar_buffer();
            limpar_tela();
            switch (op){
                case 1:
                    cadastrar(4);
                    break;
                case 2:
                    listar(4);
                    break;
                case 3:
                    devolver();
                    break;
                case 4:
                    break;
                default:
                    printf("Valor Invalido, Tente Novamente, ");
                    break;
            }
            system("pause");
                } while (op < 1 || op > 4);
            break;
      default:
        break;
      } 
    } while (opcao != '5');
}