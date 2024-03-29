
typedef struct typeToken typeToken;

typedef struct typeQuadruple typeQuadruple;

typeToken* criar_token(int tipo, int value_token, char* lexema);


void preenche_quadrupla (char* label, char* op, char* arg1, char* arg2, char* result);
void mostra_quadrupla (int posicao);
char* makeLabel ();
char* makeTemp ();

void conditionCodeCreate (typeToken* vetor_tokens[],int inicio, int fim);
char* aritmeticCodeCreate(typeToken* vetor_tokens[],int inicio, int fim);
char* expressionCodeCreate (typeToken* vetor_tokens[], int inicio, int fim);
void declarationCodeCreate(typeToken* vetor_tokens[], int inicio, int fim);
void functionCodeCreate(typeToken* vetor_tokens[], int inicio, int fim);
void avaliateExpression (typeToken* vetor_tokens[], int inicio, int fim);
void intermediaryCodeCreate (typeToken* vetor_tokens[], int inicio, int fim);