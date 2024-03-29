
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"

//Estrutura para montar dicionarios
struct associacao {
    char *string;
    int valor;
};

//Dicionario para Tokens
struct associacao tokens_links[] = {
        
    {"KW_CHAR",         KW_CHAR},
    {"KW_INT",          KW_INT},
    {"KW_REAL",         KW_REAL},
    {"KW_BOOL",         KW_BOOL},

    {"KW_IF",           KW_IF},
    {"KW_THEN",         KW_THEN},
    {"KW_ELSE",         KW_ELSE},
    {"KW_LOOP",         KW_LOOP},
    {"KW_INPUT",        KW_INPUT},
    {"KW_OUTPUT",       KW_OUTPUT},
    {"KW_RETURN",       KW_RETURN},

    {"OPERATOR_LE",     OPERATOR_LE},
    {"OPERATOR_GE",     OPERATOR_GE},
    {"OPERATOR_EQ",     OPERATOR_EQ},
    {"OPERATOR_DIF",    OPERATOR_DIF},
    {"OPERATOR_ATRIB",  OPERATOR_ATRIB},

    {"TK_IDENTIFIER",   TK_IDENTIFIER},

    {"LIT_INT",         LIT_INT},
    {"LIT_REAL",        LIT_REAL},
    
    {"LIT_CHAR",        LIT_CHAR},
    {"LIT_STRING",      LIT_STRING},

    {"SG_SEMICOLON",    SG_SEMICOLON},

    {"TOKEN_ERROR",     TOKEN_ERROR},

};

//Funcao que busca no dicionario 
char* tokenDescription(int value) {
    
    int tam = sizeof(tokens_links)/sizeof(tokens_links[0]);    
    for (int i = 0; i < tam; i++) {
        if (tokens_links[i].valor == value) {
            return tokens_links[i].string;
        }
    }
    char* description = malloc(sizeof(char)*20);
    strcpy(description,"nao identificado");
    return description;
}

//Dicionario de erros 
struct associacao erros[] = {
        
    {"Simbolo nao pertence a linguagem", 1},
    {"Caracter inválido - usado fora da composição de um identificador", 2},        // {".", "_"}
    {"Caracter inválido - usado fora da composição de um indentificador ou um numero real", 3},  // {"."}
    {"Caracter inválido - usado fora da composição de um operador", 4},             // {"!"}
    {"Caracter inválido - usado fora da indicação de início de comentário",5},      // {"\"}
    {"Faltou fechamento das aspas", 6},
    {"Caracteres invalidos para um identificador", 7},
    {"Descrição incorreta de um literal real", 8},

};

//Funcao que busca no dicionario de erros
char* errorDescription(int value) {

    int tam = sizeof(erros)/sizeof(erros[0]);
    for (int i = 0; i < tam; i++) {
        if (erros[i].valor == value) {
            return erros[i].string;
        }
    }
    char* description = malloc(sizeof(char)*32);
    strcpy(description,"tipo de erro nao identificado");
    return description;
}

//Classificador de identificadores e palavras especiais
int classificador_word(char* buffer) {
    int value = TK_IDENTIFIER;
    if (strlen(buffer) < 7) {
        if (strcmp(buffer,"char")==0) {value = KW_CHAR;}
        else if (strcmp(buffer,"int")==0) {value =  KW_INT;}
        else if (strcmp(buffer,"real")==0) {value = KW_REAL;}
        else if (strcmp(buffer,"bool")==0) {value = KW_BOOL;}
        else if (strcmp(buffer,"if")==0) {value = KW_IF;}
        else if (strcmp(buffer,"then")==0) {value = KW_THEN;}
        else if (strcmp(buffer,"else")==0) {value = KW_ELSE;}
        else if (strcmp(buffer,"loop")==0) {value = KW_LOOP;}
        else if (strcmp(buffer,"input")==0) {value = KW_INPUT;}
        else if (strcmp(buffer,"output")==0) {value = KW_OUTPUT;}
        else if (strcmp(buffer,"return")==0) {value = KW_RETURN;}
    }
    return value;
}

//Classificador de operadores
int classificador_operator(char* buffer) {
    int value;
    if (strlen(buffer)==1) {
        if (buffer[0]=='!') {value = TOKEN_ERROR;}
        else if (buffer[0]=='=') {value = OPERATOR_ATRIB;}
        else {value = buffer[0];}
    }
    else {
        switch (buffer[0]) {
            case 60: value=OPERATOR_LE;break;
            case 62: value=OPERATOR_GE;break;
            case 61: value=OPERATOR_EQ;break;
            case 33: value=OPERATOR_DIF;break;
        }
    }
    return value;
}

//Classificador de literais lexograficos
int classificador_string(char* buffer) {
    int value = TOKEN_ERROR;
    if (buffer[0]==39) {
        if ( (strlen(buffer)==3) && (buffer[2]==39) ) {value = LIT_CHAR;}
    }
    else if (buffer[strlen(buffer)-1]=='"') {value = LIT_STRING;}
    return value;
}

//Reconhecedor de literais e identificadores
bool ehIdentificadorOuLiteral(int token){

    if(token==280|token==281|token==282|token==285|token==286){
        return true;
    }
    return false;

}

//Reconhecedor de simblos invalidos para a linguagem
bool ehCaracterInvalido(char ch) {
    if ( ch<=8 || (ch>=11 && ch<=31) || ch>=127 || ch==35 || ch==36 || ch==38 ||
         ch==58 || ch==63 || ch==64 || ch==94 || ch==96 || ch=='.' || ch=='_' )
        return true;
    else
        return false;
}

//Reconhecedor dos operadores aritmeticos
bool ehOperador(char ch) { 
    // { * + - / }
    if (ch==42 || ch==43 || ch==45 || ch==47)
        return true;
    else
        return false;
}

//Reconhecedor de simbolos relacionais
bool ehRelacional(char ch) {
    // { ! < = > }
    if (ch==33 || ch==60 || ch==61 || ch==62)
        return true;
    else
        return false;
}

//Reconhecedor de algarismos
bool ehAlgarismo(char ch) {
    // { 0 - 9 }
    if (ch>=48 && ch<=57)
        return true;
    else
        return false;
}

//Reconhecedor de simbolos alfabeticos
bool ehAlfabetico(char ch) {
    // { A-Z a-z . _ }
    if ( (ch>=65 && ch<=90) || (ch>=97 && ch<=122) || ch=='.' || ch == '_' )
        return true;
    else
        return false;
}

//Reconhecedor de separadores
bool ehSeparador(char ch) {
    // { space tab enter }
    if (ch==32 || ch==9 || ch==10 || ch==13)
        return true;
    else
        return false;
}


