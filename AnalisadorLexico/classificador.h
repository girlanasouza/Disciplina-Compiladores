
char* tokenDescription(int value);
char* errorDescription(int value);

int classificador_word(char* buffer);
int classificador_operator(char* buffer);
int classificador_string(char* buffer);

bool ehIdentificadorOuLiteral(int token);
bool ehCaracterInvalido(char ch);
bool ehOperador(char ch);
bool ehRelacional(char ch);
bool ehAlgarismo(char ch);
bool ehAlfabetico(char ch);
bool ehSeparador(char ch);