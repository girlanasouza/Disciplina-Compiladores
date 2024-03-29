%{
#include <stdio.h>

extern FILE* yyin;

void yyerror(const char *s);

extern int yyparse();

extern int yylex();

%}

%define parse.trace

%token KW_CHAR          
%token KW_INT           
%token KW_REAL          
%token KW_BOOL           
%token KW_IF             
%token KW_THEN           
%token KW_ELSE           
%token KW_LOOP           
%token KW_INPUT          
%token KW_OUTPUT         
%token KW_RETURN         
%token OPERATOR_LE       
%token OPERATOR_GE       
%token OPERATOR_EQ       
%token OPERATOR_DIF      
%token OPERATOR_ATRIB    
%token TK_IDENTIFIER     
%token LIT_INT           
%token LIT_REAL          
%token LIT_CHAR          
%token LIT_STRING        
%token SG_SEMICOLON      
%token TOKEN_ERROR       


%left '+' '-'
%left '*' '/' '%'
%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%left '(' ')'



%%
programa: programa lista_com 
        | programa lista_decl 
        | %empty
        ;

lista_decl: lista_decl decl
         | decl
         ;

decl: decl_var
    | decl_func
    ;

decl_var: espec_tipo var SG_SEMICOLON
        | espec_tipo var OPERATOR_ATRIB exp SG_SEMICOLON
        | espec_tipo var OPERATOR_ATRIB vetor SG_SEMICOLON
        ;

espec_tipo: KW_INT
        | KW_REAL
        | KW_CHAR          
        ;

decl_func: espec_tipo TK_IDENTIFIER '(' params ')' com_comp
        ;
                                                                                                                                                                                                                                                                                     
params: lista_param
      | %empty
      ;

lista_param: lista_param ',' param
        | param
        ;

param: espec_tipo var
     ;

decl_locais: decl_locais decl_var
        | %empty
        ;

lista_com: lista_com comando
        | comando
        ;

comando: com_expr
       | com_atrib
       | com_comp
       | com_selecao
       | com_repeticao
       | com_retorno
       | %empty
       ;

com_expr: exp SG_SEMICOLON
        | SG_SEMICOLON
        | com_output
        | com_input
        ;


lista_elem_out: lista_elem_out ',' LIT_STRING 
        | lista_elem_out ',' exp 
        |LIT_STRING 
        | exp
        ;

com_output: KW_OUTPUT '(' lista_elem_out ')' SG_SEMICOLON

com_input: KW_INPUT '(' espec_tipo ')' SG_SEMICOLON


com_atrib: var OPERATOR_ATRIB exp SG_SEMICOLON
        ;

com_comp: '{' decl_locais lista_com '}'
        ;

com_selecao: KW_IF '(' exp ')' comando
        | KW_IF '(' exp ')' com_comp KW_ELSE comando
        ;

com_repeticao: KW_LOOP '(' exp ')' comando 
             ;

com_retorno: KW_RETURN SG_SEMICOLON
           | KW_RETURN exp SG_SEMICOLON
           ;

exp: exp_soma op_relac exp_soma
   | exp_soma
   ;

op_relac: OPERATOR_LE
        | '<'
        | '>'
        | OPERATOR_GE
        | OPERATOR_EQ
        | OPERATOR_DIF
        | '&'
        | '|'
        ;

exp_soma: exp_soma op_soma exp_mult
        | exp_mult
        ;

op_soma: '+'
       | '-'
       ;

exp_mult: exp_mult op_mult exp_simples
        | exp_simples
        ;

op_mult: '*'
       | '/'
       | '%'
       ;

exp_simples: '(' exp ')'
           | var
           | cham_func
           | literais
           
           ;

literais: LIT_INT
        | LIT_REAL
        | LIT_CHAR
        ;

cham_func: TK_IDENTIFIER '(' args ')'
        ;

var: TK_IDENTIFIER
        | TK_IDENTIFIER '[' LIT_INT ']'
        ;

args: lista_arg
    | %empty
    ;

lista_arg: lista_arg ',' exp
        | exp
        ;

lista_elem: lista_elem literais
        | literais 
        ;

vetor: '{' lista_elem '}'
        ;


%%
