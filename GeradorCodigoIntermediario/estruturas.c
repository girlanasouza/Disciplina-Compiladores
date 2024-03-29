#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"
#include "parser.tab.h"

struct typeToken{
    int tipo, value_token;
    char* lexema;
    struct typeToken* anterior, * proximo;
};

struct typeQuadruple{
    char *lexema, *op, *arg1, *arg2, *result, *label;
};

typeQuadruple quadrupla[1500];
int quantQuadruplas = 0;
int quantTemps = 0;
int quantLabels = 0;

typeToken* criar_token(int tipo, int value_token, char* lexema){
    typeToken* token = malloc(sizeof(typeToken));
    token->anterior=NULL;
    token->proximo=NULL;
    token->lexema=malloc(sizeof(char)*1024);
    strcpy(token->lexema, lexema);
    token->tipo=tipo;
    token->value_token=value_token;
    return token;
}

void preenche_quadrupla (char* label, char* op, char* arg1, char* arg2, char* result) {
    quadrupla[quantQuadruplas].op = op;
    quadrupla[quantQuadruplas].arg1 = arg1;
    quadrupla[quantQuadruplas].arg2 = arg2;
    quadrupla[quantQuadruplas].result = result;
    // acresceta a label antes de preencher a qua se estiver vazia
    if (quadrupla[quantQuadruplas].label==NULL) quadrupla[quantQuadruplas].label = label;
    mostra_quadrupla (quantQuadruplas);
    quantQuadruplas++;
}

void mostra_quadrupla(int posicao) {
    if (quantQuadruplas == 0) {
        printf("--------------------------------------------------------------------------------------\n");
        printf("| N° |      Label    |      Op       |      Arg1     |      Arg2     |     Result    |\n");
        printf("--------------------------------------------------------------------------------------\n");
    }

    const char* format = "|%4d|%15s|%15s|%15s|%15s|%15s|\n";

    printf(format, posicao,
           quadrupla[posicao].label ? quadrupla[posicao].label : "",
           quadrupla[posicao].op ? quadrupla[posicao].op : "",
           quadrupla[posicao].arg1 ? quadrupla[posicao].arg1 : "",
           quadrupla[posicao].arg2 ? quadrupla[posicao].arg2 : "",
           quadrupla[posicao].result ? quadrupla[posicao].result : "");
}

// criar a label
char* makeLabel () {
    char* label = malloc(sizeof(char)*10);
    sprintf(label,"label%d",quantLabels);
    quantLabels++;
    return label;
}

// criar um temporario
char* makeTemp () {
    char* temp = malloc(sizeof(char)*10);
    sprintf(temp,"temp%d",quantTemps);
    quantTemps++;
    return temp;
}

// chamada na main inicio do trecho e o fim do trecho 
// pega o arv todo
void intermediaryCodeCreate (typeToken* vetor_tokens[], int inicio, int fim) {

    int i = inicio;
    int contaChaves = 0;
    
    while (i<=fim) {
        // ate  encontrar o ; separação dos trechos 1° trecho vai ser indentificado e a segunda metade
        if (vetor_tokens[i]->value_token == SG_SEMICOLON) {
            avaliateExpression(vetor_tokens,inicio,i);
            intermediaryCodeCreate(vetor_tokens,i+1,fim);   
            break;
        }
        // verifica a {} continua lendo pq pode ter varias chaves 
        else if (vetor_tokens[i]->value_token == 123) {
            contaChaves++;
            // n chegou no fim e n encontrou o fecha chaves 125 fecha chaves
            while ((i<=fim) && ((vetor_tokens[i]->value_token != 125)||contaChaves>1)) {
                if (vetor_tokens[i]->value_token == 125) contaChaves++;
                i++;
            }
            avaliateExpression(vetor_tokens,inicio,i);
            intermediaryCodeCreate(vetor_tokens,i+1,fim);
            break;
        }
        i++;
    }
    // terminar  
    if (inicio == 0) preenche_quadrupla (NULL, "halt", NULL, NULL, NULL);
}

//  identifca o 1° token pq ele diz q tipo de codigo que vai criado
void avaliateExpression (typeToken* vetor_tokens[],int inicio, int fim) {

    switch (vetor_tokens[inicio]->value_token) {
        case KW_CHAR: declarationCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_INT: declarationCodeCreate (vetor_tokens,inicio,fim); break;        
        case KW_REAL: declarationCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_BOOL: declarationCodeCreate (vetor_tokens,inicio,fim); break;
        
        case KW_IF: conditionCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_ELSE: conditionCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_LOOP: conditionCodeCreate (vetor_tokens,inicio,fim); break;
        
        case KW_INPUT: functionCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_OUTPUT: functionCodeCreate (vetor_tokens,inicio,fim); break;
        case KW_RETURN: functionCodeCreate (vetor_tokens,inicio,fim); break;
        
        case TK_IDENTIFIER: expressionCodeCreate (vetor_tokens,inicio,fim); break;
        
        // limpa as chaves para etregar o codigo sem as cahves
        case 123:   intermediaryCodeCreate (vetor_tokens,inicio+1,fim-1); break;

        default: break;
    }
}

//  verifica se o 3° elem é uma ( se o 5° elem for uma { entao é uma funcao
void declarationCodeCreate(typeToken* vetor_tokens[],int inicio, int fim) {

    if (vetor_tokens[inicio+2]->value_token == 40) {
        
        if  (vetor_tokens[inicio+4]->value_token == 123)
            preenche_quadrupla (NULL, "call", vetor_tokens[inicio+1]->lexema, NULL, NULL);
        
        // devolve depois do fecha as chaves em diante
        intermediaryCodeCreate(vetor_tokens,inicio+4,fim);
    }
    // analise se ha uma atribuicao, descarta o tipo e devolve para avaliar 
    else if (vetor_tokens[inicio+2]->value_token == OPERATOR_ATRIB) 
        avaliateExpression(vetor_tokens,inicio+1,fim);
}

// verifica output input return, trata o return, funcao 
void functionCodeCreate(typeToken* vetor_tokens[], int inicio, int fim) {
    // verifica return
    if (vetor_tokens[inicio]->value_token == KW_RETURN) {
        if (inicio < fim - 1) {
            // verifica se tem tokens adicionais apos o return
            if (inicio < fim - 2) {
                // avalia a expressão aposs o return e gera uma quadrupla de chamada
                avaliateExpression(vetor_tokens, inicio + 1, fim);
                preenche_quadrupla(NULL, "call", vetor_tokens[inicio]->lexema, NULL, NULL);
            }
            else {
                // gera quadruplas para os parametros e a chamada da função apos o reutrn
                preenche_quadrupla(NULL, "param", vetor_tokens[inicio + 1]->lexema, NULL, NULL);
                preenche_quadrupla(NULL, "call", vetor_tokens[inicio]->lexema, NULL, NULL);
            }
        }
        else {
            // Gera uma quadrupla de chamada da função depois do return
            preenche_quadrupla(NULL, "call", vetor_tokens[inicio]->lexema, NULL, NULL);
        }
    }
    else if ((vetor_tokens[inicio]->value_token == KW_OUTPUT) || (vetor_tokens[inicio]->value_token == KW_INPUT)) {
        // gera quadruplas para o parâmetro e a chamada da função após 
        preenche_quadrupla(NULL, "param", vetor_tokens[inicio + 2]->lexema, NULL, NULL);
        preenche_quadrupla(NULL, "call", vetor_tokens[inicio]->lexema, NULL, NULL);
    }
    else {
        preenche_quadrupla(NULL, "call", vetor_tokens[inicio]->lexema, NULL, NULL);
    }
}
// If else e while
void conditionCodeCreate (typeToken* vetor_tokens[],int inicio, int fim) {
    // guarda temporario
    char* arg1;
    char* labelIF;
    char* labelWHILE;

    if ((vetor_tokens[inicio]->value_token == KW_IF) || (vetor_tokens[inicio]->value_token == KW_LOOP) ) {
        
        if (vetor_tokens[inicio]->value_token == KW_LOOP) {
            // label fixa, adicionar na quadrupla o num da label, aponta para o "false" do while
            labelWHILE = makeLabel();
            quadrupla[quantQuadruplas].label = labelWHILE;
        }
        
        int i = inicio+2;
        int contaParenteses = 1;
        while ( (i<fim) && (vetor_tokens[i]->value_token!=41||contaParenteses>1) ) {
            if (vetor_tokens[i]->value_token==41) contaParenteses++;
            i++;
        }
        
        arg1 = expressionCodeCreate (vetor_tokens, inicio+1, i);
    
        labelIF = makeLabel();
        preenche_quadrupla(NULL, "JMP_IFfalse", arg1, NULL, labelIF);
        intermediaryCodeCreate(vetor_tokens, i+1, fim);
    
        if(vetor_tokens[fim+1]->value_token == KW_ELSE)
            preenche_quadrupla(NULL, "JMP", NULL, NULL, makeLabel());
        
        if (vetor_tokens[inicio]->value_token == KW_LOOP)
            preenche_quadrupla(NULL, "JMP", NULL, NULL, labelWHILE);
        
        quadrupla[quantQuadruplas].label = labelIF;
        
    }
    else {
        intermediaryCodeCreate(vetor_tokens, inicio+1, fim);
        quantLabels--;
        quadrupla[quantQuadruplas].label = makeLabel();
    }
}

// expressao que nao seja de condição
char* expressionCodeCreate (typeToken* vetor_tokens[],int inicio, int fim) {
    char* arg1;
    char* arg2;
    char* result = NULL;

    // verifica se é uma assinatura de função
    if ((vetor_tokens[inicio+1]->value_token == 40) && (vetor_tokens[inicio+2]->value_token == 41) &&
        (vetor_tokens[inicio+3]->value_token == SG_SEMICOLON)) functionCodeCreate(vetor_tokens,inicio,inicio+3);
    
    // verifica se é uma atr
    else if (vetor_tokens[inicio+1]->value_token == OPERATOR_ATRIB) {
        // atr simples
        if (vetor_tokens[inicio+3]->value_token == SG_SEMICOLON) {
            preenche_quadrupla (NULL, "==", vetor_tokens[inicio+2]->lexema, NULL, vetor_tokens[inicio]->lexema);
        }
        // atr complexa, chama o expCode para o que tem depois do igual
        else {
            arg1 = expressionCodeCreate(vetor_tokens, inicio+2, fim);
            preenche_quadrupla (NULL, "==", arg1, NULL, vetor_tokens[inicio]->lexema);
        }
    }
    // verifica outros, cond, relacional, aritmeticas
    else {
        int i = inicio;
        // verifica se encontra relacional
        while (i<fim && (vetor_tokens[i]->value_token != 60) && (vetor_tokens[i]->value_token != 62) &&
              (vetor_tokens[i]->value_token != 269) && (vetor_tokens[i]->value_token != 270) &&
              (vetor_tokens[i]->value_token != 271) && (vetor_tokens[i]->value_token != 272)) i++;


        // se for relacional 
        if (i<fim) {
            // relacional simples, ja adiciona o id no arg1 antes do op
            if( ((i-2) < inicio) || vetor_tokens[i-2]->value_token == 40)
                arg1 = vetor_tokens[i-1]->lexema;
            // relacional complexo, reavalia
            else
                arg1 = expressionCodeCreate(vetor_tokens, inicio, i-1);

            if ( ((i+2) > fim) || vetor_tokens[i+2]->value_token == 41)
                arg2 = vetor_tokens[i+1]->lexema;
            else 
                arg2 = expressionCodeCreate(vetor_tokens, i+1, fim);
            
            result = makeTemp();
            preenche_quadrupla (NULL, vetor_tokens[i]->lexema, arg1, arg2, result);

        }
        // expressao aritmetica
        else result = aritmeticCodeCreate(vetor_tokens,inicio,fim);
    
    }
    return result;
}


// trata a expressao aritmetica
char* aritmeticCodeCreate(typeToken* vetor_tokens[],int inicio, int fim) {
    
    typeToken** pilha = malloc(sizeof(typeToken*)*(fim-inicio+1));
    int ocupPilha = 0;
    typeToken** vetorNP = malloc(sizeof(typeToken*)*(fim-inicio+1));
    int ocupVetNP = 0;

    int i = inicio;  
    // montagem da notação  polonesa
    while (i<=fim) {   
        
        if ( (vetor_tokens[i]->value_token >= 274) && (vetor_tokens[i]->value_token <= 278) ) {
            vetorNP[ocupVetNP] = vetor_tokens[i];
            ocupVetNP++;
        }
        else if (vetor_tokens[i]->value_token == 40) {
            pilha[ocupPilha] = vetor_tokens[i];
            ocupPilha++;
        }
        else if (vetor_tokens[i]->value_token == 41) {
            
            while (pilha[ocupPilha-1]->value_token != 40) {
                vetorNP[ocupVetNP] = pilha[ocupPilha-1];
                ocupVetNP++;
                ocupPilha--;
            }
            ocupPilha--;
        }
        else if ((vetor_tokens[i]->value_token == 42) || (vetor_tokens[i]->value_token == 47)) {
            while (ocupPilha>0 && ((pilha[ocupPilha-1]->value_token == 42) || (pilha[ocupPilha-1]->value_token == 47))) {
                vetorNP[ocupVetNP] = pilha[ocupPilha-1];
                ocupVetNP++;
                ocupPilha--;
            }
            pilha[ocupPilha] = vetor_tokens[i];
            ocupPilha++;
        }
        else if ((vetor_tokens[i]->value_token == 43) || (vetor_tokens[i]->value_token == 45)) {
            while (ocupPilha>0 && (pilha[ocupPilha-1]->value_token == 42 || pilha[ocupPilha-1]->value_token == 43 ||
                                   pilha[ocupPilha-1]->value_token == 45 || pilha[ocupPilha-1]->value_token == 47 )) {
                
                vetorNP[ocupVetNP] = pilha[ocupPilha-1];
                ocupVetNP++;
                ocupPilha--;
            }
            pilha[ocupPilha] = vetor_tokens[i];
            ocupPilha++;
        }
        i++;
    }
    // esvaziamento da pilha
    while (ocupPilha>0) {
        vetorNP[ocupVetNP] = pilha[ocupPilha-1];
        ocupVetNP++;
        ocupPilha--;
    }
    i = 0;
    // criação das quadruplas para todas as partes da expessao
    while (i < ocupVetNP) {
        if (vetorNP[i]->value_token <= 47) {
            typeToken* temp = criar_token(1, TK_IDENTIFIER, makeTemp());
            preenche_quadrupla (NULL, vetorNP[i]->lexema, pilha[ocupPilha-2]->lexema, pilha[ocupPilha-1]->lexema, temp->lexema);
            pilha[ocupPilha-2] = temp;
            ocupPilha--;
        }
        else {
            pilha[ocupPilha] = vetorNP[i];
            ocupPilha++;
        }
        i++;
    }
    // retorno  do resultado da expressao
    return pilha[0]->lexema;
    free(pilha);
    free(vetorNP);
}