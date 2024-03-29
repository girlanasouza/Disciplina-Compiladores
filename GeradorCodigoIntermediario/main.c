/*
Este programa é um gerador de código intermediário para a linguagem FIRST2023. Ele analisa um arquivo de entrada contendo código-fonte e produz uma saída com o código intermediário gerado a partir do código-fonte.
*/


#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "estruturas.h"

struct typeToken{
    int tipo, value_token;
    char* lexema;
    struct typeToken* anterior, * proximo;
};

FILE *inputFile;
FILE *outputFile;
FILE *arqLido;

typeToken** vetor_tokens;
int ocupVetor = 0;

extern int yylineno;
extern int yyparse();
extern FILE* yyin;
extern FILE* yyout;

int yywrap()
{
    return 1;
}

void yyerror(const char *s){
    fprintf(stderr,"Erro na linha %d: %s\n", yylineno, s);
    exit(3);
}

void ler_arquivo();

int main(int argc, char* argv[]) {
    vetor_tokens = malloc(sizeof(typeToken*)*5000);
    if (argc == 2) {
        inputFile = fopen(argv[1], "r");
    } else {
        inputFile = fopen("entrada.txt", "r");
    }
    
    if (inputFile == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    outputFile = fopen("saida.txt", "w"); 

    if (outputFile == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        return 1;
    }

    yyin = inputFile;
    yyout = outputFile;


    int resultado_analise = yyparse();

    fclose(outputFile);
    fclose(inputFile);

    if (resultado_analise == 0) {
        printf("Análise sintática bem-sucedida.\n");
        // contrução do vetor de tokens
        ler_arquivo();
        // gerador de codigo
        intermediaryCodeCreate (vetor_tokens,0,ocupVetor-1);

    } else {
        printf("Erro na análise sintática.\n");
    }

    return 0;
}

void ler_arquivo(){
    FILE *arqLido = fopen("saida.txt", "r");   
    if (arqLido == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        return;

    }
    int value_token=-1;
    char lexema[1024];

    
    while(fscanf(arqLido, "%d %[^\n]", &value_token, lexema)==2){
        typeToken* token = criar_token( 0, value_token, lexema);
        vetor_tokens[ocupVetor] = token;
        ocupVetor++;
    }  
}
