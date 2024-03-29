/*
O analisador de expressão matemática tem a função de ler expressões matemáticas simples, compreendendo números de um único dígito e operadores básicos (soma, subtração, divisão, e multiplicação). O principal objetivo é verificar a corretude da expressão inserida e convertê-la para a notação polonesa reversa (NPR), um formato que organiza a expressão de modo a eliminar qualquer ambiguidade sobre a ordem das operações.
*/


#include <stdio.h>
#include <stdlib.h>

typedef struct tipoNo {
    struct tipoNo* prox;
    char dado; 
} tipoNo;

typedef struct tipoPilha {
    tipoNo* topo;
    int tam;
} tipoPilha;

int criarPilha(tipoPilha* pilha) {
    pilha->topo = NULL;
    pilha->tam = 0;
    return 1;
}

void inserirPilha(tipoPilha* pilha, char dado) {  
    tipoNo* novoNo;
    novoNo = malloc(sizeof(tipoNo)); 
    novoNo->dado = dado;
    novoNo->prox = pilha->topo;
    pilha->tam++;
    pilha->topo = novoNo;
}

char removerPilha(tipoPilha* pilha) {
    char valueAux;
    tipoNo* aux;
    aux = pilha->topo;
    valueAux = aux->dado;
    pilha->topo = aux->prox;
    pilha->tam--;
    free(aux);
    return valueAux;
}

void mostrarPilha(tipoPilha* pilha) {
    tipoNo* aux;
    aux = pilha->topo;
    while (aux != NULL) {
        printf("%c", aux->dado);
        aux = aux->prox;
    }
    printf("\n");
}

// RETORNA VERDADEIRO SE UM CARACTERE FOR UM OPERADOR
int verificarOperador(char caractere){
    int verifica = 0;
    if(caractere==42 ||caractere==43||caractere==45||caractere==47){
        return 1;
    }
    return 0;
}

// RETORNA VERDADEIRO SE UM CARACTERE FOR UM NUMERO
/**
 * Retorna 1 se é um número
 * Retorna -1 Erro (ausência de termo)
 * Retorna -2 para Inconsistência nos dados
*/
int verificarDigito(char caractere){
    int verifica = -2;
    if(caractere>=48 && caractere<=57){
        verifica = 1;
        return verifica;
    }else if(!(caractere>=48 && caractere<=57)){
        verifica = -1;
        return verifica;
    }
    return verifica;
}

/**
 * Retorna 1 sucesso
 * Retorna -1 Erro (ausência de termo)
 * Retorna -2 para Inconsistência nos dados
 * Retorna -3 para Stack empty
 * Retorna -4 Erro (excesso de operador)!!!
 * Retorna -5 Erro (excesso de algarismo)!!!
 * Retorna -6 Erro (ausencia de termo)!!!
 * Retorna -50 erro generico
*/
int read_file(char  filename[], tipoPilha* pilhaDigitos, tipoPilha* pilhaOperadores){

    FILE *arq = fopen(filename, "r");
    char caractere;
    short controle = 1;
    int status = 1;

    fseek(arq, 0, SEEK_SET);

    while (fscanf(arq, "%c", &caractere)==1 ) {
        
        if(controle){
            status = verificarDigito(caractere);
            
            if(status == 1) inserirPilha(pilhaDigitos, caractere);
            else{
                if(verificarOperador(caractere)) inserirPilha(pilhaOperadores, caractere);
            }

        }else{
            if(verificarOperador(caractere)) inserirPilha(pilhaOperadores, caractere);
            else{
                status = verificarDigito(caractere);
                
                if(status == 1) inserirPilha(pilhaDigitos, caractere);
                status=-5;
            }
        }
        
        controle = !controle;
    }
    
    fclose(arq);
    
    if(status==1){
        if ( pilhaDigitos->tam==0 && pilhaOperadores->tam==0) status=-3;
        if ( (pilhaDigitos->tam-pilhaOperadores->tam) != 1) status=-4;
        if ( pilhaDigitos->tam == 1 && pilhaOperadores->tam > 0) status=-6;
    }

    
    return status;
}

int merge_stacks(tipoPilha* pilhaDigitos, tipoPilha* pilhaOperadores, tipoPilha* pilhaSaida){

    int controle = 0;
    char entrada;
    int status = 1;

    while(pilhaDigitos->tam && pilhaOperadores->tam ){   

        if(controle){
            entrada = removerPilha(pilhaDigitos);
        }else{
            entrada = removerPilha(pilhaOperadores);
        }
        
        inserirPilha(pilhaSaida,entrada);
        

        controle = !controle;

    };

    while (pilhaDigitos->tam)
    {
        entrada = removerPilha(pilhaDigitos);
        inserirPilha(pilhaSaida,entrada);
    }
   
    return status;

}

int merge_stacks_erros(tipoPilha* pilhaDigitos, tipoPilha* pilhaOperadores, tipoPilha* pilhaSaida){

    int controle = 0;
    char input;
    int status = 1;

    while (pilhaOperadores->tam)
    {
        input = removerPilha(pilhaOperadores);
        inserirPilha(pilhaSaida,input);
    }

    while (pilhaDigitos->tam)
    {
        input = removerPilha(pilhaDigitos);
        inserirPilha(pilhaSaida,input);
    }
    
    return status;

}


/**
 * Retorna -1 Erro (ausência de termo)
 * Retorna -2 para Inconsistência nos dados
 * Retorna -3 para Stack empty
 * Retorna -4 Erro (excesso de operador)!!!
 * Retorna -5 Erro (excesso de algarismo)!!!
 * Retorna -6 Erro (ausencia de termo)!!!
 * Retorna -50 erro generico
*/

void mostrarErro(int code_error){

    switch (code_error)
    {
    case -1:
        printf("Erro (ausência de termo)\n");
        break;
    case -2:
        printf("Inconsistência nos dados!!!\n");
        break;
    case -3:
        printf("Stack empty\n");
        break;
    case -4:
        printf("Erro (excesso de operador)!!!\n");
        break;
    case -5:
        printf("Erro (excesso de algarismo)!!!\n");
        break;
    case -6:
        printf("Erro (ausencia de termo)!!!\n");
        break;
    
    default:
        break;
    }
}

int main(int argc, char* *argv) {

    tipoPilha* pilhaDigitos = malloc(sizeof(tipoPilha));
    tipoPilha* pilhaOperadores = malloc(sizeof(tipoPilha));
    tipoPilha* pilhaSaida = malloc(sizeof(tipoPilha));

    int status;

    criarPilha(pilhaDigitos);
    criarPilha(pilhaOperadores);
    criarPilha(pilhaSaida);

    status = read_file(argv[1], pilhaDigitos, pilhaOperadores);
    
    if(status==1){
        merge_stacks(pilhaDigitos, pilhaOperadores, pilhaSaida);
        printf("ok\n");
        mostrarPilha(pilhaSaida);
    }else{
        merge_stacks_erros(pilhaDigitos, pilhaOperadores, pilhaSaida);
        mostrarErro(status);
        mostrarPilha(pilhaSaida);
    }

}
