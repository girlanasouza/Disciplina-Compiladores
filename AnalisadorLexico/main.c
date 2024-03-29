
/*
Trata-se de um programa escrito na Linguagem C que implementa as funções de um Analisador Léxico para uma ferramenta de Compilação de Códigos
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tokens.h"
#include "classificador.h"


//ORGANIZACAO DO ANALISADOR LEXICO:

//PARTE I - ESTRUTURAS E VARIAVEIS GLOBAIS E SUAS RESPECTIVAS FUNCOES DE MANIPULACAO

//ESTRUTURAS DE DADOS PARA GUARDA E ORGANIZACAO DOS LEXEMAS

//lexemas
typedef struct lexem_t { 
    char* lexema;
    int linha;
    int token;
    int error;
    struct lexem_t* prox;
} lexem_type;

//lista encadeada de lexemas
typedef struct list_lexem_t {
    lexem_type* dado; 
    struct list_lexem_t *prox;
} list_lexem;

//hash - tabela de simbolos
typedef struct symbol_hash_table_t {
    int tam;
    list_lexem** listas; //vetor de listas 
} symbol_hash_table;


//ASSINATURAS DAS FUNCOES DE MANIPULACAO DAS ESTRUTURAS DE ARMAZENAMENTO DOS LEXEMAS
lexem_type* lexem_create (char* lexem, int token, int linha);  //aloca memoria e inicializa um lexema
bool add_list_lexem (lexem_type* lexem, list_lexem** list);    //adiciona um lexema numa lista encadeada
void print_lexem_list (list_lexem* lista);                     //printa uma lista de lexemas
int calculate_prime_number (int inicial);                      //calcula numero primo (para definir tamanho da hash)
symbol_hash_table* symbol_hash_table_create(int linhas);       //cria a tabela hash (tabela de simbolos) com listas encadeadas nas entradas
unsigned int symbol_hash_table_function (lexem_type* lexem, int tam); //funcao hash para encontrar entrada na tabela
bool add_symbol_hash_table (lexem_type* lexem, symbol_hash_table* symbol_table); //adiciona identificadores e literais na hash - tabela de simbolos
void print_symbol_table (symbol_hash_table* symbol_table);     //printa o conteudo das entradas ocupadas da tabela de simbolos



//ESTRUTURA DA VARIAVEL GLOBAL QUE VAI MAPEAR DADOS GERAIS DA ENTRADA (CODIGO-FONTE)
typedef struct TipoArquivo {
    char* nome;         //para guardar nome do arquivo de entrada 
    int numLinhas;      //para guardar numero de linhas do arquivo
    long* vetorPosicao; //para indicar posicao de inicio de cada linha (mapeamento)
    bool reading;       //informa se o processo de leitura do arquivo ainda esta em curso
} TipoArquivo;


//VARIAVEIS GLOBAIS
TipoArquivo codigoFonte;          //para guardar dados do arquivo de entrada
symbol_hash_table* symbol_table;  //para guardar ponteiro para tabela de simbolos



//PARTE II - FUNCOES DA ESPECIFICACAO DO TRABALHO CHAMADAS PELA MAIN E POR ELAS PROPRIAS PARA EXECUCAO DA ANALISE LEXICA COMPLETA DO ARQUIVO-FONTE

//assinaturas:
void file_mapping(FILE* entrada, char* nome_arquivo);      //funcao que faz o mapeamento inicial do codigo-fonte
int getLineNumber(FILE* entrada);                          //funcao que retorna a linha em que cada lexema aparece no codigo
bool isRunning();                                          //funcao que informa se chegou ou nao ao fim da leitura do arquivo
char prox_char(FILE* entrada);                             //funcao que resgata um caracter do arquivo-fonte por vez
void erro(lexem_type* token, FILE* entrada, FILE* saida);  //funcao que retorna o tipo de erro de analise lexica encontrado
void grava_token(lexem_type* token, FILE* saida);          //funcao que grava o lexema devidamente tokenizado no arquivo de saida
lexem_type* analex(char* ch, char* buffer, FILE* entrada); //funcao principal que coordena a analise lexica



//PARTE III - FUNCOES AUXILIARES DO ANALEX() PARA CONSTRUCAO DOS LEXEMAS CONFORME A NATUREZA

//assinaturas:
lexem_type* build_word (char* ch, char* buffer, FILE* entrada);      //construcao dos lexemas do tipo: identificadores e palavras reservadas
lexem_type* build_number (char* ch, char* buffer, FILE* entrada);    //construcao dos lexemas do tipo literais numericos
lexem_type* build_operator (char* ch, char* buffer, FILE* entrada);  //construcao dos lexemas do tipo: operadores compostos e de atribuicao
lexem_type* build_string(char* ch, char* buffer, FILE* entrada);     //construcao dos lexemas do tipo literais lexograficos
lexem_type* discard_comments(char* ch, char* buffer, FILE* entrada); //para descartar os comentarios



//FUNCAO MAIN
int main(int argc, char *argv[]) {
    
    FILE* entrada = fopen(argv[1],"r");
    if( entrada == NULL ) {printf("Arquivo de entrada nao foi acessado...\n");}
    
    //mapeamento do arquivo de entrada (codigo-fonte)
    file_mapping(entrada,argv[1]);
    
    //criacao da tabela de simbolos (alocacao de memoria)
    symbol_table = symbol_hash_table_create(codigoFonte.numLinhas);
    

    FILE* saida = fopen(argv[2],"w");
    if( saida == NULL ) {printf("Arquivo nao foi acessado...\n");}
        
    
    //variaveis locais para entrada e retorno das funcoes chamadas na Main
    lexem_type* token;      //para receber os tokens formados pelo analisador lexico
    char buffer[1024] = ""; //buffer de montagem dos lexemas
    char ch[2] = "";        //para receber o caracter atual lido do arquivo
    

    //loop de execucao do analisador lexico ate chegar ao fim da leitura do arquivo de entrada
    ch[0] = prox_char(entrada);                   
    while (isRunning()) {  
        token = analex(ch,buffer,entrada);         //analisador lexico devolve lexema tokenizado
        grava_token(token,saida);                  //faz-se a gravacao do token e lexema no arquivo de saida
        erro(token,entrada,saida);                 //identifica se houve erro, devolvendo o tipo e forcando parada da leitura
        add_symbol_hash_table(token,symbol_table); //adiciona token na tabela de simbolos conforme o caso
        ch[0] = prox_char(entrada);                //le novo caracter
    }
    
    fclose(entrada);
    fclose(saida);
    
    print_symbol_table(symbol_table);   //funcao chamada para mostrar preenchimento da tabela de simbolos

    return 0;
}


//IMPLEMENTACAO DAS FUNCOES ACIMA ESPECIFICADAS:

//FUNCOES DA PARTE I:

lexem_type* lexem_create (char* lexem, int token, int linha) {
    lexem_type* novo = malloc(sizeof(lexem_type));
    novo->lexema = malloc(sizeof(char)*(strlen(lexem)+1));
    strcpy(novo->lexema,lexem);
    novo->linha = linha;
    novo->token = token;
    novo->error = 0;
    novo->prox = NULL;
    return novo;
}


bool add_list_lexem (lexem_type* lexem, list_lexem** list) {
    
    list_lexem* novo = malloc(sizeof(list_lexem));
    if (novo==NULL) return false;
    novo->dado = lexem;
    novo->prox = NULL;
    
    list_lexem* atual = *list;
    if (*list==NULL) *list = novo;
    
    else {
    
        if (strcmp(atual->dado->lexema,lexem->lexema)==0) {
            lexem_type* aux = atual->dado;
            while(aux->prox) {aux = aux->prox;}
            aux->prox = lexem;
            free(novo);
            return true;
        }       
        while (atual->prox != NULL) {
            
            atual = atual->prox;
            if (strcmp(atual->dado->lexema,lexem->lexema)==0) {
                lexem_type* aux = atual->dado;
                while(aux->prox) {aux = aux->prox;}
                aux->prox = lexem;
                free(novo);
                return true;
            }
        }
        atual->prox = novo;
    }
    return true;
}


void print_lexem_list (list_lexem* lista) {
    
    list_lexem* id = lista;
    while(id) {

        lexem_type* id2 = id->dado;
        int token = id2->token;

        if (id2->token < 255) 
            printf("( %d, %s ) - aparece nas linha(s):",token,id2->lexema);
        else
            printf("( %s, %s ) - aparece nas linha(s):",tokenDescription(token),id2->lexema);

        while(id2) {
            printf(" %d", id2->linha);
            id2 = id2->prox;
        }
        id = id->prox;
        printf("\n");
    }
}


void print_symbol_table (symbol_hash_table* symbol_table) {
    
    int tam = symbol_table->tam;
    printf("\nTABELA DE SIMBOLOS:\n");

    for (int i=0; i<tam;i++) {

        list_lexem* lista = symbol_table->listas[i];        
        if (lista!=NULL) {
            printf("Entrada %d:\n",i);
            print_lexem_list(lista);
            printf("\n");
        }
    }
}


int calculate_prime_number (int inicial) {
    int prime_number = inicial;
    int divisor = 3;
    if (prime_number%2 == 0) {prime_number++;}
    while (divisor <= prime_number) {
        if ( (prime_number%divisor == 0) && (prime_number != divisor) ) {
            divisor = 3;
            prime_number += 2;
        }
        else {divisor++;}
    }
    return prime_number;
}


symbol_hash_table* symbol_hash_table_create(int linhas) {
       
    symbol_hash_table* nova = malloc(sizeof(symbol_hash_table));
    nova->tam = calculate_prime_number(linhas);
    nova->listas = malloc(sizeof(list_lexem*)*nova->tam);
    for (int i=0;i<nova->tam;i++) {
        nova->listas[i] = NULL;
    }
    return nova;
}


unsigned int symbol_hash_table_function (lexem_type* lexem, int tam) {
    unsigned int hash_value = 0;
    int i = 0;
    char ch;
    ch = lexem->lexema[i];
    while (lexem->lexema[i]!='\0') {
        hash_value = (hash_value*31) + lexem->lexema[i];
        i++;
    }
    return (hash_value % tam);
}


bool add_symbol_hash_table (lexem_type* lexem, symbol_hash_table* symbol_table) {

    if (lexem && ehIdentificadorOuLiteral(lexem->token)){
        
        int index = symbol_hash_table_function (lexem,symbol_table->tam);
        list_lexem** lista = &symbol_table->listas[index];
        add_list_lexem (lexem,lista);
        return true;
    }
    return false;
}


//FUNCOES DA PARTE II:

bool isRunning() {
    return codigoFonte.reading;
}


int getLineNumber(FILE* entrada) {
    long posicao = ftell(entrada);
    int inicio = 0;
    int fim = codigoFonte.numLinhas;
    int meio;
    while ( (fim-inicio) > 1 ) {
        meio = (inicio+fim)/2;
        if (codigoFonte.vetorPosicao[meio] > posicao) {fim = meio;}
        else {inicio = meio;}
    }
    return fim;
}


void file_mapping(FILE* entrada, char* nome_arquivo) {
                
    codigoFonte.reading = true;
    codigoFonte.nome = nome_arquivo;
    printf("\nArquivo-fonte: %s\n",codigoFonte.nome);
    
    int numLines = 0;
    char ch;
    while( (ch = fgetc(entrada)) != EOF ) {
        if (ch == '\n') {numLines++;}
    }
    if (ch != '\n') {numLines++;}

    fseek(entrada, 0, SEEK_SET);

    codigoFonte.numLinhas = numLines;

    printf("Numero de linhas: %d\n",numLines);
    
    int i = 0;
    codigoFonte.vetorPosicao = malloc(sizeof(long)*(numLines));
    codigoFonte.vetorPosicao[i] = 0;
    while( (ch = fgetc(entrada)) != EOF ) {
        if (ch == '\n') {
            i++;
            if (i<numLines) {codigoFonte.vetorPosicao[i] = ftell(entrada);}
        }
    }
    fseek(entrada, 0, SEEK_SET);
}


void erro(lexem_type* token, FILE* entrada, FILE* saida) {
    
    if (token && token->token == TOKEN_ERROR) {
        fprintf(saida,"%s\n", errorDescription(token->error));
        fseek(entrada,0,SEEK_END);
    }
}


void grava_token(lexem_type* token, FILE* saida) { 

    if (token) {
        if (ftell(saida)==0) {fprintf(saida,"%-10s%-20s%s\n\n","LINHA","TOKEN","LEXEMA");}
        if (token->token >= 256)
            fprintf(saida,"%-10d%-20s%s\n",token->linha,tokenDescription(token->token),token->lexema);
        else
            fprintf(saida,"%-10d%-20d%s\n",token->linha,token->token,token->lexema);            
    }
}


lexem_type* analex(char* ch, char* buffer, FILE* entrada) {
    
    lexem_type* lexema;
    
    if (ehSeparador(ch[0])) {lexema = NULL;}
    
    else if (ehCaracterInvalido(ch[0])) {
        lexema = lexem_create(ch,TOKEN_ERROR,getLineNumber(entrada));
        if (ch[0] == '.') {lexema->error = 3;}
        else if (ch[0] == '_') {lexema->error = 2;}
        else {lexema->error = 1;}
    }
    else if (ehAlfabetico(ch[0])) {lexema = build_word(ch,buffer,entrada);}
    else if (ehAlgarismo(ch[0]))  {lexema = build_number(ch,buffer,entrada);}
    else if (ehRelacional(ch[0])) {lexema = build_operator(ch,buffer,entrada);}
    else if (ch[0] == '"' || ch[0] == 39) {lexema = build_string(ch,buffer,entrada);}
    else if (ch[0] == 92) {lexema = discard_comments(ch,buffer,entrada);}
    
    else {
        int token = (ch[0] == ';') ? SG_SEMICOLON : ch[0];
        lexema = lexem_create(ch,token,getLineNumber(entrada));
    }
    
    buffer[0] = '\0';

    return lexema;
} 


char prox_char(FILE* entrada) {
    char ch;
    int leu = fscanf(entrada,"%c",&ch);
    
    if (!leu || leu==-1) {
        codigoFonte.reading = false; //setando a variavel global para parada do loop de execucao quando EOF
        return 0;
    }
    else
        return ch;
}



//FUNCOES DA PARTE III:

lexem_type* build_word (char* ch, char* buffer, FILE* entrada) {
    strcat(buffer,ch);
    ch[0] = prox_char(entrada);
    while ( ch[0] && ehAlfabetico(ch[0]) ) {
        strcat(buffer,ch);
        ch[0] = prox_char(entrada);
    }
    if ( !(feof(entrada) || ehSeparador(ch[0])) )
        fseek(entrada,-1,SEEK_CUR);

    int token = classificador_word(buffer);
    lexem_type* novo = lexem_create(buffer,token,getLineNumber(entrada));
    
    if (ehAlgarismo(ch[0])) {
        novo->token = TOKEN_ERROR;
        novo->error = 7;
        strcat(novo->lexema,ch);
    }

    return novo;
}


lexem_type* build_number (char* ch, char* buffer, FILE* entrada) {
    strcat(buffer,ch);
    bool ponto = false;
    
    ch[0] = prox_char(entrada);
    while ( ch[0] && (ehAlgarismo(ch[0]) || (ch[0] == '.' && !ponto)) ) {
        if (ch[0] == '.') {ponto = true;}
        strcat(buffer,ch);
        ch[0] = prox_char(entrada);
    }
    
    if ( !(feof(entrada) || ehSeparador(ch[0])) )
        fseek(entrada,-1,SEEK_CUR);
    
    int token = (ponto) ? LIT_REAL : LIT_INT;
    lexem_type* novo = lexem_create(buffer,token,getLineNumber(entrada));
    
    if (ehAlfabetico(ch[0])) {
        novo->token = TOKEN_ERROR;
        novo->error = 7;
        strcat(novo->lexema,ch);
    }
    
    if (ehSeparador(ch[0]) && buffer[strlen(buffer)-1]=='.') {
        novo->token = TOKEN_ERROR;
        novo->error = 8;
    }
    
    return novo;
}


lexem_type* build_operator (char* ch, char* buffer, FILE* entrada) {
    strcat(buffer,ch);
    ch[0] = prox_char(entrada);
    if ( ch[0] && (ch[0] == '=') ) {
        strcat(buffer,ch);
    }
    if ( !(feof(entrada) || ehSeparador(ch[0])) )
        fseek(entrada,strlen(buffer)-2,SEEK_CUR);
    
    int token = classificador_operator(buffer);
    lexem_type* novo = lexem_create(buffer,token,getLineNumber(entrada));
    if (token==TOKEN_ERROR) {novo->error = 4;}
    return novo;
}


lexem_type* build_string(char* ch, char* buffer, FILE* entrada) {
    strcat(buffer,ch);
    if (ch[0]==39) {
        ch[0] = prox_char(entrada);
        if (ch[0]) {strcat(buffer,ch);}
        ch[0] = prox_char(entrada);
        if (ch[0]) {strcat(buffer,ch);}
    }
    else {
        ch[0] = prox_char(entrada);
        while ( ch[0] && (ch[0] != '"') ) {
            strcat(buffer,ch);
            ch[0] = prox_char(entrada);
        }
        if (ch[0] == '"') {strcat(buffer,ch);}
    }
    int token = classificador_string(buffer);
    if (token==TOKEN_ERROR && strlen(buffer)>100) {buffer[100]='\0';}
    lexem_type* novo = lexem_create(buffer,token,getLineNumber(entrada));
    if (token==TOKEN_ERROR) {novo->error = 6;}
    return novo;
}


lexem_type* discard_comments(char* ch, char* buffer, FILE* entrada) {
      
    strcat(buffer,ch);
    lexem_type* novo = lexem_create(buffer,TOKEN_ERROR,getLineNumber(entrada));
    novo->error = 5; // seta inicialmente o "\" como erro (caracter invalido) enquanto nao confirmado se tratar de inicio de comentario
    
    short contra_barras = 1;
    ch[0] = prox_char(entrada);
    
    if (ch[0] && ch[0]==92) {
        contra_barras++;
        
        ch[0] = prox_char(entrada);    
        if (ch[0] && ch[0]==92) {contra_barras++;}
        
        //trata comentarios de uma linha
        if (contra_barras==2) {
            while (ch[0]) {
                if (ch[0]=='\n') {break;}
                ch[0] = prox_char(entrada);
            }
        }
        //trata comentarios de mais de uma linha
        else {
            ch[0] = prox_char(entrada);
            while ( (contra_barras) && (ch[0]) ) {
            
                while (contra_barras && ch[0]=='/') { //verifica se aparecem tres barras seguidas finalizando comentario
                    contra_barras--;
                    ch[0] = prox_char(entrada);
                }

                if (contra_barras>0) { // enquanto nao aparecerem tres barras seguidas
                    contra_barras = 3; // reinicia a contagem de barras para reconhecer fim de comentario
                    ch[0] = prox_char(entrada);
                }
            }            
        }
        novo = NULL; //descarta o comentario
    }
    
    return novo;
}
