#
#  - Compiladores  - Etapa analisador Léxico
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

etapa1: lex.yy.c parser.tab.c main.c
	gcc -o etapa1 main.c lex.yy.c parser.tab.c estruturas.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y
    
lex.yy.c: scanner.l
	flex -l scanner.l

clean:
	rm lex.yy.c etapa1