/*
Trata-se de um programa escrito nas linguagens usadas pelas ferramentas LEX (FLEX) e YACC (BISON), capazes de gerar código-fonte em Linguagem C com as quais se implementam reconhecedores de símbolos e padrões de escrita, que, operando em conjunto,
funcionam como uma espécie de compilador funcional parcial de uma linguagem de programação, pois permitem a realização das etapas de análise léxica e sintática da compilação de um código em cotejo com as regras gramaticais esquematizadas nesses reconhecedores para a linguagem, no caso específico, batizada de FIRST2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
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

int main(int argc, char* argv[]) {
    
    FILE *inputFile;
    if (argc == 2) {
        inputFile = fopen(argv[1], "r");
    } else {
        inputFile = fopen("entrada.txt", "r");
    }
     

    if (inputFile == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *outputFile = fopen("saida.txt", "w"); //guardar resultado analise lexica

    if (outputFile == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        return 1;
    }

    yyin = inputFile;
    yyout = outputFile;

    fprintf(yyout,"%-15s %-15s\n", "TOKEN", "LEXEMA");

    int resultado_analise = yyparse();
    if (resultado_analise == 0) {
        printf("Análise sintática bem-sucedida.\n");
    } else {
        printf("Erro na análise sintática.\n");
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
