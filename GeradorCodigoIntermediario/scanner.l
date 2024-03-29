%{
#include "parser.tab.h"
%}


DIGITO [0-9]
ALFA [a-zA-Z]
ASCII [\(\)\{\}+\-*\/<>&|~\[\]\,\.]
ID ({ALFA}|\.|\_)*{ALFA}({ALFA}|\.|\_)*
string	\"[^\n"]+\"

%x comment1
%x comment2

%option yylineno

%%

"\\\\"[^\\] {
    BEGIN(comment1);
}
<comment1>{
.  { /* Ignorar o comentário de uma linha */ }
    "\n" {
        BEGIN(INITIAL);
    }
}

"\\\\\\" {
    BEGIN(comment2);
}
<comment2>{
(.|\n) { /*ignora*/ }
    "///" {
        BEGIN(INITIAL);
    }
}


[\n\t ]+     {/* ignorar espaços em branco e novas linhas */ }

"char" { 
    fprintf(yyout, "%d %s\n", KW_CHAR, yytext);
    return KW_CHAR;
}

"int" { 
    fprintf(yyout, "%d %s\n", KW_INT, yytext);
    return KW_INT;
}

"real" { 
    fprintf(yyout, "%d %s\n", KW_REAL, yytext);
    return KW_REAL;
}

"bool" { 
    fprintf(yyout, "%d %s\n", KW_BOOL, yytext);
    return KW_BOOL;
}

"if" { 
    fprintf(yyout, "%d %s\n", KW_IF, yytext);
    return KW_IF;
}

"then" { 
    fprintf(yyout, "%d %s\n", KW_THEN, yytext);
    return KW_THEN;
}

"else" {
    fprintf(yyout, "%d %s\n", KW_ELSE, yytext);
    return KW_ELSE;
}

"while" {
    fprintf(yyout, "%d %s\n", KW_LOOP, yytext);
    return KW_LOOP;
}

"return" {
    fprintf(yyout, "%d %s\n", KW_RETURN, yytext);
    return KW_RETURN;
}

"output" {
    fprintf(yyout, "%d %s\n", KW_OUTPUT, yytext);
    return KW_OUTPUT;
}

"input" {
    fprintf(yyout, "%d %s\n", KW_INPUT, yytext);
    return KW_INPUT;
}

";" {
    fprintf(yyout, "%d %s\n", SG_SEMICOLON, yytext);
    return SG_SEMICOLON;
}

"==" {
    fprintf(yyout, "%d %s\n", OPERATOR_EQ, yytext);
    return OPERATOR_EQ;
}

"<=" {
    fprintf(yyout, "%d %s\n", OPERATOR_LE, yytext);
    return OPERATOR_LE;
}

">=" {
    fprintf(yyout, "%d %s\n", OPERATOR_GE, yytext);
    return OPERATOR_GE;
}

"!=" {
    fprintf(yyout, "%d %s\n", OPERATOR_DIF, yytext);
    return OPERATOR_DIF;
}

"=" {
    fprintf(yyout, "%d %s\n", OPERATOR_ATRIB, yytext);
    return OPERATOR_ATRIB;
}

{DIGITO}+    { 
    fprintf(yyout, "%d %s\n", LIT_INT, yytext);
    return LIT_INT;
}

{DIGITO}*"."{DIGITO}+ {
    fprintf(yyout, "%d %s\n", LIT_REAL, yytext);
    return LIT_REAL;
}

{DIGITO}+"."{DIGITO}* {
    fprintf(yyout, "%d %s\n", LIT_REAL, yytext);
    return LIT_REAL;
}

{string} {
    fprintf(yyout, "%d %s\n", LIT_STRING, yytext);
    return LIT_STRING;
}

\'{ALFA}\' {
    fprintf(yyout, "%d %s\n", LIT_CHAR, yytext);
    return LIT_CHAR;
}

{ID} {
    fprintf(yyout, "%d %s\n", TK_IDENTIFIER, yytext);
    return TK_IDENTIFIER;
}

{ASCII} {
    int ascii_code = yytext[0];
    fprintf(yyout, "%d %c\n", ascii_code, yytext[0]);
    return ascii_code;
}


. {
    fprintf(yyout, "Caractere não reconhecido: %s\n", yytext);
    return TOKEN_ERROR;
 }

%%

/*
int main() {
    FILE *inputFile = fopen("entrada.txt", "r");
    FILE *outputFile = fopen("saida.txt", "w"); 

    if (inputFile == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    if (outputFile == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        return 1;
    }

    stdout = outputFile;

    fprintf(yyout, "%-15d %-15d\n", "TOKEN", "LEXEMA");
    yyin = inputFile;

    while (yylex());

    fclose(inputFile);
    fclose(outputFile); // Fecha o arquivo de saída

    return 0;
}
*/